library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity clk_gen is
	port (
		CLK 	: in std_logic;
		CLK_out	: out std_logic
	);

end clk_gen;

architecture Behavioral of clk_gen is
	signal clk_count : std_logic_vector(22 downto 0) := (others => '0');
begin
	process (CLK)
	begin
		if rising_edge(CLK) then
			clk_count <= clk_count + 1;
			CLK_out <= '1';
			--if clk_count(1) = '1' then -- testbench
			if clk_count(22) = '1' then
				CLK_out <= '0';
				clk_count <= (others => '0');
			end if;
		end if;
	end process;
end Behavioral;