library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.utils.all;

entity cell is
	port (
		CLK 		: in std_logic;
		RESET 		: in std_logic;
		STATE 		: out std_logic;
		INIT_STATE 	: in std_logic;
		NEIGH 		: in std_logic_vector(7 downto 0);
		DIRECTION 	: in DIRECTION_T;
		EN 			: in std_logic;
		ANIM_EN		: in std_logic
	);
	-- NEIGH(7 downto 0)
	-- X X X X X
	-- X 0 1 2 X
	-- X 3 X 4 X
	-- X 5 6 7 X
	-- X X X X X

end cell;

architecture Behavioral of cell is
	signal NEXT_STATE : std_logic;
	signal SHIFT_STEPS : std_logic_vector(3 downto 0) := (others => '0');
begin
	STATE <= NEXT_STATE;
	
	cell_proc: process (CLK, RESET)
	begin
		if (RESET = '1') then
			NEXT_STATE <= INIT_STATE;
		elsif rising_edge(CLK) then
			if (ANIM_EN = '1') then	
				if NEXT_STATE = '1' then
					NEXT_STATE <= not ((NEIGH(1) xnor NEIGH(3)) xnor (NEIGH(4) xnor NEIGH(6)));
				else
					NEXT_STATE <= '1';
				end if;
			elsif (EN = '1') then
				case (DIRECTION) is
					when DIR_DR =>    NEXT_STATE <= NEIGH(0);
					when DIR_DOWN =>  NEXT_STATE <= NEIGH(1);
					when DIR_DL =>    NEXT_STATE <= NEIGH(2);
					when DIR_RIGHT => NEXT_STATE <= NEIGH(3);
					when DIR_LEFT =>  NEXT_STATE <= NEIGH(4);
					when DIR_UR =>    NEXT_STATE <= NEIGH(5);
					when DIR_UP =>    NEXT_STATE <= NEIGH(6);
					when DIR_UL =>    NEXT_STATE <= NEIGH(7);
				end case;
			else
				NEXT_STATE <= INIT_STATE;
			end if;
		end if;
	end process;
end Behavioral;