-- uart.vhd: UART controller - receiving part
-- Author(s): xjusti00
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_RX is
port(	
		CLK: 	    	in std_logic;
		RST: 	    	in std_logic;
		DIN: 	      in std_logic;
		DOUT: 	   out std_logic_vector(7 downto 0);
		DOUT_VLD: 	out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal cnt1 	: std_logic_vector(4 downto 0) := (others => '0');
signal cnt2 	: std_logic_vector(3 downto 0) := (others => '0');
signal recive	: std_logic;
signal counter	: std_logic;
signal data		: std_logic;
begin
	FSM: entity work.UART_FSM(behavioral)
	port map (
				CLK 		=> CLK,
				RST		=> RST,
				DIN		=> DIN,
				CNT1		=> cnt1,
				CNT2		=> cnt2,
				RX_EN		=> recive,
				CNT_EN 	=> counter,
				DOUT_VLD	=> data
			);
		SET: process (CLK) begin 
			if rising_edge(CLK) then 
				if counter = '1' then 
					cnt1 <= cnt1 + 1;
					else 
						cnt1 <= "00000";
						cnt2 <= "0000";
				end if;
				if recive = '1' then
					if cnt1 = "10000" then
					cnt1 <= cnt1 + 1;
					cnt1 <= "00000";
					case cnt2 is 
						when "0000" => DOUT(0) <= DIN;
						when "0001" => DOUT(1) <= DIN;
						when "0010" => DOUT(2) <= DIN;
						when "0011" => DOUT(3) <= DIN;
						when "0100" => DOUT(4) <= DIN;
						when "0101" => DOUT(5) <= DIN;
						when "0110" => DOUT(6) <= DIN;
						when "0111" => DOUT(7) <= DIN;
						when others => null;
						end case;
						cnt2 <= cnt2 + 1; 
					end if;
				end if;
			end if;
		end process;
end behavioral;
