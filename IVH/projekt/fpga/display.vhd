library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity display is
	port (
		data : in std_logic_vector(127 downto 0);
		reset : in std_logic;
		clk : in std_logic; -- hodiny 25 MHz
		smclk : in std_logic; -- hodiny 7.3728MHz
		A : out std_logic_vector(3 downto 0);
		R : out std_logic_vector(7 downto 0)
	);
end entity display;

architecture behv of display is
	signal cnt : std_logic_vector(20 downto 0) := (others => '0');
	signal A_temp : std_logic_vector(3 downto 0) := (others => '1');
	signal R_temp : std_logic_vector(7 downto 0);
begin
	counter : process (clk, reset) 
	begin
		if reset = '1' then
			cnt <= (others => '0');
			A_temp <= (others => '1');
			R_temp <= (others => '0');
		elsif (rising_edge(clk)) then
			cnt <= cnt + 1;
			--if cnt(2) = '1' then		-- testbench
			if cnt(15 downto 13) = "101" then
				cnt <= (others => '0');
				A <= A_temp;
				R <= R_temp;
				A_temp <= A_temp + 1;
				case( A_temp ) is
					when "1111" => 
						R_temp <= data(127 downto 120);
						A_temp <= (others => '0');
					when "0000" => R_temp <= data(119 downto 112);
					when "0001" => R_temp <= data(111 downto 104);
					when "0010" => R_temp <= data(103 downto 96);
					when "0011" => R_temp <= data(95 downto 88);
					when "0100" => R_temp <= data(87 downto 80);
					when "0101" => R_temp <= data(79 downto 72);
					when "0110" => R_temp <= data(71 downto 64);
					when "0111" => R_temp <= data(63 downto 56);
					when "1000" => R_temp <= data(55 downto 48);
					when "1001" => R_temp <= data(47 downto 40);
					when "1010" => R_temp <= data(39 downto 32);
					when "1011" => R_temp <= data(31 downto 24);
					when "1100" => R_temp <= data(23 downto 16);
					when "1101" => R_temp <= data(15 downto 8);
					when "1110" => R_temp <= data(7 downto 0);
					when others => A_temp <= (others => '0');
				end case;
			end if;
		end if;
	end process;
end behv;

