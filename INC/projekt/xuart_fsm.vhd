-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xjusti00
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK 		: in std_logic;
   RST 		: in std_logic;
	DIN 		: in std_logic;
	CNT1		: in std_logic_vector(4 downto 0);
	CNT2		: in std_logic_vector(3 downto 0);
	RX_EN		: out std_logic;
	CNT_EN	: out std_logic;
	DOUT_VLD	: out std_logic	
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
type STATE_TYPE is (START_BIT, FIRST_BIT, STOP_BIT, RECIVE_DATA, VALID_DATA);
signal state : STATE_TYPE := START_BIT;

begin	RX_EN <= '1' when state = RECIVE_DATA else '0';	CNT_EN <= '1' when state = RECIVE_DATA else '0';	
	set_state: process (CLK) begin 	
		if rising_edge(CLK) then 
				if RST = '1' then 
					state <= START_BIT;
					else 
						case state is 
						when START_BIT => if DIN = '0' then 
												state <= FIRST_BIT;
												end if;
						when FIRST_BIT => if CNT1 = "00000" then 
												state <= RECIVE_DATA;
												end if;
						when RECIVE_DATA => if CNT2 = "1000" then 
												state <= STOP_BIT;
												end if;
						when STOP_BIT => if DIN = '1' then 
												state <= VALID_DATA;
												end if;
						when VALID_DATA => if DIN = '1' then 
												state <= FIRST_BIT;
												end if;

						when others => null;
						end case;
					end if;
		end if;
	end process;
end behavioral;
