LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
use ieee.std_logic_unsigned.all;

ENTITY display_tb IS

END display_tb;

ARCHITECTURE behavior OF display_tb IS 

    -- Component Declaration for the Unit Under Test (UUT)
    COMPONENT display
    PORT(
         data : IN  std_logic_vector(127 downto 0);
         reset : IN  std_logic;
         clk : IN  std_logic;
         smclk : IN  std_logic;
         A : OUT  std_logic_vector(3 downto 0);
         R : OUT  std_logic_vector(7 downto 0)
        );
    END COMPONENT;

   --Inputs
   signal data : std_logic_vector(127 downto 0) := (others => '0');
   signal reset : std_logic := '0';
   signal clk : std_logic := '0';
   signal smclk : std_logic := '0';

 	--Outputs
   signal A : std_logic_vector(3 downto 0);
   signal R : std_logic_vector(7 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
   constant smclk_period : time := 10 ns;

	signal DATA1 : std_logic_vector(0 to 127) := 
			"11111111" & "10000001" & "11000011" & "10000001" & "11100111" & "10000001" & "11111111" & "10000001" &
			"10101011" & "11010101" & "10101011" & "00101000" & "00000000" & "10000001" & "10000001" & "11111111";

	signal DATA2 : std_logic_vector(0 to 127) := 
			"11111111" & "00000000" & "11111111" & "00000000" & "11111111" & "00000000" & "11111111" & "00000000" &
			"00000000" & "11111111" & "00000000" & "11111111" & "00000000" & "11111111" & "00000000" & "11111111";

	signal DATA3 : std_logic_vector(0 to 127) := 
			"00000000" & "00000001" & "00000011" & "00000111" & "00001111" & "00011111" & "00111111" & "01111111" &
			"11111111" & "11111110" & "11111100" & "11111000" & "11110000" & "11100000" & "11000000" & "10000000";

function convToStr(v: std_logic_vector) return string is
	variable retStr : string(1 to v'length);
begin
	for i in 0 to v'length-1 loop
		if v(i) = '1' then
			retStr(v'length-i) := '1';
		else
			retStr(v'length-i) := '0';
		end if;
	end loop;
	return retStr;
end function;

procedure runTest2x(v: std_logic_vector) is
	variable expected : string(1 to 8);
	variable returned : string(1 to 8);
	variable col : string(1 to 4);
begin
	report "RUNNING 2X TEST";
	report "COLUMN | RECEIVED | EXPECTED";
	wait for clk_period * 2;
	for i in 1 to 16 loop
		wait for clk_period * 2;
		-- convert to string (expected val)
		for j in 0 to 7 loop
			if v(((i-1)*8)+j) = '1' then
				expected(j+1) := '1';
			else
				expected(j+1) := '0';
			end if;
		end loop;
		col := convToStr(A);
		returned := convToStr(R);
		report col & " | " & returned & " | " & expected;
	end loop;

	-- repeat without reset
	for i in 1 to 16 loop
		wait for clk_period * 2;
		-- convert to string (expected val)
		for j in 0 to 7 loop
			if v(((i-1)*8)+j) = '1' then
				expected(j+1) := '1';
			else
				expected(j+1) := '0';
			end if;
		end loop;
		col := convToStr(A);
		returned := convToStr(R);
		report col & " | " & returned & " | " & expected;
	end loop;
end procedure;

procedure runTest(v: std_logic_vector) is
	variable expected : string(1 to 8);
	variable returned : string(1 to 8);
	variable col : string(1 to 4);
begin
	report "RUNNING TEST";
	report "COLUMN | RECEIVED | EXPECTED";
	wait for clk_period * 2;
	for i in 1 to 16 loop
		wait for clk_period * 2;
		-- convert to string (expected val)
		for j in 0 to 7 loop
			if v(((i-1)*8)+j) = '1' then
				expected(j+1) := '1';
			else
				expected(j+1) := '0';
			end if;
		end loop;
		col := convToStr(A);
		returned := convToStr(R);
		report col & " | " & returned & " | " & expected;
	end loop;
end procedure;

BEGIN
	-- Instantiate the Unit Under Test (UUT)
   uut: display PORT MAP (
          data => data,
          reset => reset,
          clk => clk,
          smclk => smclk,
          A => A,
          R => R
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;

   smclk_process :process
   begin
		smclk <= '0';
		wait for smclk_period/2;
		smclk <= '1';
		wait for smclk_period/2;
   end process;

   -- Stimulus process
   stim_proc: process
		variable expected : string(1 to 8);
		variable returned : string(1 to 8);
		variable col : string(1 to 4);
   begin		
		reset <= '1';
      wait for 100 ns;	
		reset <= '0';

		data <= DATA1;
		runTest(DATA1);
		reset <= '1';
      wait for clk_period;	
		reset <= '0';
		data <= DATA2;
		runTest(DATA2);
		reset <= '1';
      wait for clk_period;	
		reset <= '0';
		data <= DATA3;
		runTest(DATA3);
		reset <= '1';
      wait for clk_period;	
		reset <= '0';
		data <= DATA1;
		runTest2x(DATA1);
      wait;
   end process;
END;

