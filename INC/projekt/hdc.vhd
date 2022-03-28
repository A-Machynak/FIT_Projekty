-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK : in std_logic;           --clock 
   RST : in std_logic;           --reset
   DIN : in std_logic;           --input
   
   CNT_CLK : in std_logic_vector (4 downto 0); --counter for clocks
   CNT_BIT : in std_logic_vector (4 downto 0); --counter for bits
   CNT_EIGHT : in std_logic_vector (4 downto 0); --counter of 8 clock signals
   --STATE ENCODING VECTOR,    
   STATE_ENC: out std_logic_vector (2 downto 0)  
   --000 = waiting for start bit,
   --001 = counts 8 clock signals
   --010 = data reading
   --011 = waiting for the end bit
   --100 = valid bit is broadcast
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
type FSMstate is (WAIT_FOR_START_BIT,COUNT_8,READ_DATA,WAIT_FOR_END_BIT,VALID);
signal present_state : FSMstate := WAIT_FOR_START_BIT; 
begin


next_state_logic: process(CLK,RST)  --logic of the state change
begin
  if (RST='1') then --reset immediately
    --Outputs are set to broadcast the starting state
    STATE_ENC <= "000";
    present_state <= WAIT_FOR_START_BIT;

  end if;
  
  if falling_edge(clk) then
  case present_state is
    
  when WAIT_FOR_START_BIT => --waiting for DIN to turn to 0, otherwise keep waiting
    if (DIN='0') then 
      --Output vector is set to broadcast the second state - waiting for 8 clock signals to pass
      STATE_ENC <= "001";
      present_state <= COUNT_8 ; --the next state will be "waiting for 8 clocks"
    end if;
  
when COUNT_8 =>
    if (CNT_EIGHT = "01000") then --if counter reaches 8 clocks, reading of data can start
      STATE_ENC <= "010";
      present_state <= READ_DATA; --read data
    end if;

when READ_DATA =>
    if (CNT_BIT = "01000") then --if all 8 bits are read, wait for stop bit
      STATE_ENC <= "011";
      present_state <= WAIT_FOR_END_BIT;
    end if;    
    
when WAIT_FOR_END_BIT =>
    if (DIN='1') then --if STOP bit was read   
      STATE_ENC <= "100";
      present_state <= VALID;
    end if;

when VALID =>
    if (CNT_CLK = "00000") then --after broadcasting the valid bit for one clock, return to the starting state
      STATE_ENC <= "000";
      present_state <= WAIT_FOR_START_BIT;
    end if;
    
end case;      
end if;
      

end process;



end behavioral;