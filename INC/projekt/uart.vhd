-- uart.vhd: UART controller - receiving part
-- Author(s): Augustin Machynak - xmachy02
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

-------------------------------------------------
entity UART_RX is
port(	
   CLK:      in std_logic;
	RST:      in std_logic;
	DIN:      in std_logic;
	DOUT:     out std_logic_vector(7 downto 0);
	DOUT_VLD: out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
begin
	UART_FSM: entity work.UART_FSM
	port map(
		CLK => CLK,
		RST => RST,
		FSM_DIN => DIN,
		FSM_DOUT => DOUT,
		FSM_DOUT_VLD => DOUT_VLD
	);
end behavioral;
