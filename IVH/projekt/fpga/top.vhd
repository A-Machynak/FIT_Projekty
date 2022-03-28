library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.utils.all;

architecture main of tlv_gp_ifc is
	signal A : std_logic_vector(3 downto 0) := (others => '0');
	signal R : std_logic_vector(7 downto 0); 
	signal DATA_MOVE : std_logic_vector(0 to 127) :=
		"00011000" &
		"00111100" &
		"01011010" &
		"00011000" &
		"00000000" &
		"00100100" &
		"01000010" &
		"11111111" &
		"11111111" &
		"01000010" &
		"00100100" &
		"00000000" &
		"00011000" &
		"01011010" &
		"00111100" &
		"00011000";
	signal DATA_END : std_logic_vector(0 to 127) :=
		"00000000" &
		"11111111" &
		"00000000" &
		"00000000" &
		"10101010" &
		"00000000" &
		"01010101" &
		"00000000" &
		"00000000" &
		"01010101" &
		"00000000" &
		"10101010" &
		"00000000" &
		"00000000" &
		"11111111" &
		"00000000";
		
	signal STATE : std_logic_vector(0 to 127);
	signal STATE_STATIC : std_logic_vector(0 to 127) := DATA_MOVE;
	signal CURRENT_DIRECTION : DIRECTION_T := DIR_RIGHT;
	signal C_ENABLE : std_logic := '1';
	signal C_RESET : std_logic := '0';
	signal C_ANIM_EN : std_logic := '0';

	signal SHIFT_COUNT : std_logic_vector(3 downto 0) := (others => '0');
	signal SHIFTS : std_logic_vector(1 downto 0) := (others => '0');
	signal CURRENT_DATA : std_logic_vector(1 downto 0) := "00";

	-- clk for cells
	signal CLK_c : std_logic := '0';
begin
	dis : entity work.display port map (
		CLK => CLK,
		SMCLK => SMCLK,
		RESET => RESET,
		DATA => STATE,
		A => A,
		R => R
	);

    -- map output --
    X(6) <= A(3);
    X(8) <= A(1);
    X(10) <= A(0);
    X(7) <= '0'; -- en_n
    X(9) <= A(2);
    X(16) <= R(1);
    X(18) <= R(0);
    X(20) <= R(7);
    X(22) <= R(2);
    X(17) <= R(4);
    X(19) <= R(3);
    X(21) <= R(6);
    X(23) <= R(5);
    ----------------------


    -- clock gen for cells --
    clk_gen : entity work.clk_gen port map (
    	CLK => CLK,
    	CLK_out => CLK_c
    );
    -------------------------


    -- create and connect cells --
    gen_cx: for x in 0 to 15 generate
    	gen_cy: for y in 0 to 7 generate
    		cell_t : entity work.cell
    			port map (
    				CLK => CLK_c,
    				RESET => C_RESET,
    				STATE => STATE(GETID(x, y)),
    				INIT_STATE => STATE_STATIC(GETID(x, y)),
    				NEIGH(0) => STATE(GETID(x - 1, y + 1)), -- UL
    				NEIGH(1) => STATE(GETID(x, y + 1)), 	-- UP
    				NEIGH(2) => STATE(GETID(x + 1, y + 1)), -- UR
    				NEIGH(3) => STATE(GETID(x - 1, y)), 	-- LEFT
    				NEIGH(4) => STATE(GETID(x + 1, y)), 	-- RIGHT
    				NEIGH(5) => STATE(GETID(x - 1, y - 1)), -- DL
    				NEIGH(6) => STATE(GETID(x, y - 1)), 	-- DOWN
    				NEIGH(7) => STATE(GETID(x + 1, y - 1)), -- DR
    				DIRECTION => CURRENT_DIRECTION,
    				EN => C_ENABLE,
    				ANIM_EN => C_ANIM_EN
    			);
    	end generate gen_cy;
    end generate gen_cx;
    ---------------------------

    shift_proc: process(CLK_c, RESET)
    begin
    	if RESET = '1' then
    		C_RESET <= '1';
    		CURRENT_DATA <= (others => '0');
    		CURRENT_DIRECTION <= DIR_RIGHT;
    		STATE_STATIC <= DATA_MOVE;
    	elsif CLK_c'event and CLK_c = '1' then
    		C_RESET <= '0';
    		SHIFT_COUNT <= SHIFT_COUNT + 1;

    		if (CURRENT_DATA = "11" or CURRENT_DATA = "10") and SHIFT_COUNT = "1000" then
    			SHIFTS <= SHIFTS + 1;
	    		SHIFT_COUNT <= (others => '0');
    		elsif SHIFT_COUNT = "1111" then
    			SHIFTS <= SHIFTS + 1;
	    		SHIFT_COUNT <= (others => '0');
	    	end if;

			if SHIFTS = "11" then
	    		SHIFTS <= (others => '0');
	    		case (CURRENT_DATA) is
	    			when "00" => 
	    				CURRENT_DATA <= CURRENT_DATA + 1;
	    				STATE_STATIC <= DATA_MOVE;
	    				CURRENT_DIRECTION <= DIR_LEFT;
	    				C_RESET <= '1';
	    			when "01" => 
	    				CURRENT_DATA <= CURRENT_DATA + 1;
	    				STATE_STATIC <= DATA_MOVE;
	    				CURRENT_DIRECTION <= DIR_DOWN;
	    				C_RESET <= '1';
	    			when "10" => 
	    				CURRENT_DATA <= CURRENT_DATA + 1;
	    				STATE_STATIC <= DATA_MOVE;
	    				CURRENT_DIRECTION <= DIR_UR;
	    				C_RESET <= '1';
	    			when "11" => 
	    				STATE_STATIC <= DATA_END;
	    				C_RESET <= '1';
	    				C_ANIM_EN <= '1';
	    				C_ENABLE <= '0';
	    			when others => 
	    				CURRENT_DATA <= "01";
	    				STATE_STATIC <= DATA_MOVE;
	    				CURRENT_DIRECTION <= DIR_RIGHT;
	    				C_RESET <= '1';
	    		end case;
	    	end if;
    	end if;
    end process;
end main;

