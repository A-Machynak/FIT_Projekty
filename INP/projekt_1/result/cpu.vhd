-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Augustin Machynak, xmachy02
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	signal program_counter : std_logic_vector(11 downto 0) := (others => '0');
	signal program_counter_en : std_logic := '0';
	signal program_counter_inc : std_logic := '0'; -- 0=decrease, 1=increase
	
	signal data_ptr : std_logic_vector(9 downto 0) := (others => '0');
	
	signal bracket_counter : std_logic_vector(7 downto 0) := (others => '0');
	signal bracket_skip : std_logic := '0';
	signal bracket_return : std_logic := '0';
	
	type fsm_state is (s_initial, s_fetch, s_decode, s_execute);
	signal p_state : fsm_state;
	signal n_state : fsm_state;
	
	signal current_cell_data : std_logic_vector(7 downto 0) := (others => '0');
	
	type instructions is (i_ptr_inc, i_ptr_dec, i_cell_inc, i_cell_dec, i_while_open, i_while_close, i_print, i_load_and_store, i_while_end, i_exit, i_unknown);
	signal decoded_instruction : instructions;
	
	signal execute_en : std_logic := '0';
	signal execute_finished : std_logic := '0';
	signal execute_cycle : std_logic_vector(2 downto 0) := (others => '0');
	
	signal current_data : std_logic_vector(7 downto 0) := (others => '0');
	
	signal kill : std_logic := '0';

begin
	process(CLK, RESET)
	begin
		if CLK'event and CLK='1' then
			if RESET='1' or kill='1' then
				p_state <= s_initial;
			elsif EN='1' then
				p_state <= n_state;
			end if;
		end if;
	end process;
	
	process(p_state, EN, execute_finished)
	begin
		n_state <= s_initial;
		CODE_EN <= '0';
		
		case p_state is
			when s_initial =>
				if EN='1' then
					n_state <= s_fetch;
					CODE_EN <= '1';
				end if;
			when s_fetch => 
				n_state <= s_decode;
			when s_decode => 
				execute_en <= '1';
				n_state <= s_execute;
				
			when s_execute => 
				n_state <= s_execute;
				if execute_finished='1' then
					execute_en <= '0';
					n_state <= s_initial;
				end if;
			when others => null;
		end case;
	end process;
	
	process(CODE_DATA)
	begin
		case CODE_DATA is
			when x"3E" => decoded_instruction <= i_ptr_inc;
			when x"3C" => decoded_instruction <= i_ptr_dec;
			when x"2B" => decoded_instruction <= i_cell_inc;
			when x"2D" => decoded_instruction <= i_cell_dec;
			when x"5B" => decoded_instruction <= i_while_open;
			when x"5D" => decoded_instruction <= i_while_close;
			when x"2E" => decoded_instruction <= i_print;
			when x"2C" => decoded_instruction <= i_load_and_store;
			when x"7E" => decoded_instruction <= i_while_end;
			when x"00" => decoded_instruction <= i_exit;
			when others => decoded_instruction <= i_unknown;
		end case;
	end process;
	
	process(CLK, RESET, program_counter_en, program_counter_inc)
	begin
		if CLK'event and CLK='1' then
			if RESET='1' or kill='1' then
				program_counter <= (others => '0');
			elsif program_counter_en='1' and EN='1' then
				if program_counter_inc='0' and program_counter > x"000" then
					program_counter <= program_counter - '1';
					CODE_ADDR <= program_counter - '1';
				elsif program_counter_inc='1' then
					program_counter <= program_counter + '1';
					CODE_ADDR <= program_counter + '1';
				end if;
			end if;
		end if;
	end process;
	
	process(CLK, RESET, DATA_RDATA, decoded_instruction)
	begin
		if CLK'event and CLK='1' then
			execute_cycle <= "000";
			DATA_EN <= '0';
			program_counter_en <= '0';
			
			if RESET='1' or kill='1' then
				execute_finished <= '0';
				data_ptr <= (others => '0');
				current_data <= (others => '0');
			elsif execute_finished='1' then
				execute_finished <= '0';
			elsif execute_en='1' then
				-- skip everything until you find the next bracket
				if bracket_skip='1' then
					execute_finished <= '1';
					program_counter_inc <= '1';
					program_counter_en <= '1';
					
					if decoded_instruction=i_while_close then
						bracket_counter <= bracket_counter - '1';
					elsif decoded_instruction=i_while_open then
						bracket_counter <= bracket_counter + '1';
					end if;
					
					if bracket_counter=x"00" then
						program_counter_inc <= '0';
						program_counter_en <= '0';
						bracket_skip <= '0';
					end if;
				elsif bracket_return='1' then
					execute_finished <= '1';
								
					program_counter_inc <= '0'; -- ! decrease
					program_counter_en <= '1';
					if decoded_instruction=i_while_close then
						bracket_counter <= bracket_counter - '1';
					elsif decoded_instruction=i_while_open then
						bracket_counter <= bracket_counter + '1';
					end if;
					
					if bracket_counter=x"00" then
						program_counter_inc <= '1';
						program_counter_en <= '1';
						bracket_return <= '0';
					end if;
				else
					-- execute
					case decoded_instruction is
						when i_ptr_inc =>
							case execute_cycle is 
								when "000" =>
									-- Write the old data
									DATA_ADDR <= data_ptr;
									DATA_WDATA <= current_data;
									DATA_WREN <= '1';
									DATA_EN <= '1';
									execute_cycle <= "001";
									
									-- Increase the pointer
									if data_ptr="1111111111" then
										data_ptr <= "0000000000";
									else
										data_ptr <= data_ptr + 1;
									end if;
								when "001" =>
									-- Load the new data
									DATA_ADDR <= data_ptr;
									DATA_WREN <= '0';
									DATA_EN <= '1';
									execute_cycle <= "010";
								when "010" =>
									-- Read the new data
									execute_cycle <= "011";
								when "011" =>
									current_data <= DATA_RDATA;
									execute_finished <= '1';
									
									program_counter_inc <= '1';
									program_counter_en <= '1';
								when others => null;
							end case;
							
						when i_ptr_dec =>
							case execute_cycle is 
								when "000" =>
									-- Write the old data
									DATA_ADDR <= data_ptr;
									DATA_WDATA <= current_data;
									DATA_WREN <= '1';
									DATA_EN <= '1';
									execute_cycle <= "001";
									
									-- Decrease the pointer
									if data_ptr="0000000000" then
										data_ptr <= "1111111111";
									else
										data_ptr <= data_ptr - 1;
									end if;
								when "001" =>
									-- Load the new data
									DATA_ADDR <= data_ptr;
									DATA_WREN <= '0';
									DATA_EN <= '1';
									execute_cycle <= "010";
								when "010" =>
									-- Read the new data
									execute_cycle <= "011";
								when "011" =>
									current_data <= DATA_RDATA;
									execute_finished <= '1';
									
									program_counter_inc <= '1';
									program_counter_en <= '1';
								when others => null;
							end case;
							
						when i_cell_inc =>
							current_data <= current_data + '1';
							execute_finished <= '1';
							
							program_counter_inc <= '1';
							program_counter_en <= '1';
						when i_cell_dec =>
							current_data <= current_data - '1';
							
							execute_finished <= '1';
							program_counter_inc <= '1';
							program_counter_en <= '1';
							
						when i_while_open =>
							bracket_counter <= bracket_counter + '1';
							if current_data=x"00" then
								bracket_skip <= '1';
							end if;
							
							execute_finished <= '1';
							program_counter_inc <= '1';
							program_counter_en <= '1';
						when i_while_close =>
							bracket_counter <= bracket_counter - '1';
							if current_data > x"00" then
								bracket_return <= '1';
								program_counter_inc <= '0'; -- start returning
							else
								program_counter_inc <= '1';
							end if;
							
							execute_finished <= '1';
							program_counter_en <= '1';
						when i_print =>
							case execute_cycle is
								when "000" =>
									if OUT_BUSY='0' then
										OUT_DATA <= current_data;
										OUT_WREN <= '1';
										
										execute_cycle <= "001";
									end if;
								when "001" =>
									OUT_WREN <= '0';
									
									execute_finished <= '1';
									program_counter_inc <= '1';
									program_counter_en <= '1';
								when others => null;
							end case;
						when i_load_and_store =>
							case execute_cycle is
								when "000" =>
									IN_REQ <= '1';
									execute_cycle <= "001";
								when "001" =>
									if IN_VLD='1' then
										current_data <= IN_DATA;
										
										execute_finished <= '1';
										program_counter_inc <= '1';
										program_counter_en <= '1';
									end if;
								when others => null;
							end case;
						when i_while_end =>
							-- otherwise we are not in a while loop
							if bracket_counter > x"00" then
								bracket_skip <= '1';
							end if;
							execute_finished <= '1';
							program_counter_inc <= '1';
							program_counter_en <= '1';
						when i_exit =>
							kill <= '1';
						when others =>
							execute_finished <= '1';
							program_counter_inc <= '1';
							program_counter_en <= '1';
					end case;
				end if;
			end if;
		end if;
	end process;
end behavioral;
 
