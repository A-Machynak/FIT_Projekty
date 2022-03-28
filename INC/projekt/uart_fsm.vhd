-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): Augustin Machynak - xmachy02
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
--use ieee.numeric_std.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK  : in std_logic;
   RST  : in std_logic;
   FSM_DIN : in std_logic;
   FSM_DOUT : out std_logic_vector(7 downto 0);
   FSM_DOUT_VLD : out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is 
   -- States
   type state_type is (st1_waitStart, st2_startRead, st3_readByte, st4_stopBit, st5_endRead); 
   signal state : state_type := st1_waitStart;
   
   -- Internal signals
   signal FSM_DOUT_i : std_logic_vector(7 downto 0) := (others => '0');
   signal FSM_Count : std_logic_vector(7 downto 0) := (others => '0');

begin
   FSM_PROC: process (CLK)
   begin
      if (CLK'event and CLK = '0') then
         if (RST = '1') then
            state <= st1_waitStart;
            FSM_DOUT <= (others => '0');
            FSM_Count <= (others => '0');
            FSM_DOUT_VLD <= '0';
         else
            FSM_DOUT_VLD <= '0';
            case (state) is
               when st1_waitStart =>
                  if FSM_DIN = '0' then
                     state <= st2_startRead;
                     FSM_Count <= "00000001";
                  end if;
               when st2_startRead =>
                  FSM_Count <= FSM_Count + 1;
                  if FSM_Count = "00001000" then
                     if FSM_DIN = '0' then
                        state <= st3_readByte;
                     else
                        state <= st1_waitStart;
                     end if;
                  end if;
               when st3_readByte =>
                  FSM_Count <= FSM_Count + 1;
                  case (FSM_Count) is
                     when "00011000" => FSM_DOUT_i(0) <= FSM_DIN; -- 24
                     when "00101000" => FSM_DOUT_i(1) <= FSM_DIN; -- 40
                     when "00111000" => FSM_DOUT_i(2) <= FSM_DIN; -- 56
                     when "01001000" => FSM_DOUT_i(3) <= FSM_DIN; -- 72
                     when "01011000" => FSM_DOUT_i(4) <= FSM_DIN; -- 88
                     when "01101000" => FSM_DOUT_i(5) <= FSM_DIN; -- 104
                     when "01111000" => FSM_DOUT_i(6) <= FSM_DIN; -- 120
                     when "10001000" => FSM_DOUT_i(7) <= FSM_DIN; -- 136
                     when "10010111" => state <= st4_stopBit; -- 151
                     when others => -- nop
                  end case;
                  -- nop
               when st4_stopBit =>
                  FSM_Count <= FSM_Count + 1;
                  if FSM_DIN = '1' then
                     state <= st5_endRead;
                  else -- didn't receive stop bit - incorrect read
                     state <= st1_waitStart;
                  end if;
               when st5_endRead =>
                  FSM_Count <= FSM_Count + 1;
                  if FSM_Count = "10100000" then -- 160
                     FSM_DOUT <= FSM_DOUT_i;
                     FSM_DOUT_i <= (others => '0');
                     FSM_DOUT_VLD <= '1';
                     state <= st1_waitStart;
                  end if;
               when others =>
                  FSM_DOUT_VLD <= '0';
                  FSM_DOUT <= (others => '0');
                  state <= st1_waitStart;
            end case;
         end if;
      end if;
   end process;
end behavioral;