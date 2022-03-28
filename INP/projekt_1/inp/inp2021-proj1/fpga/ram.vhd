-- ram.vhd : RAM memory
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdenek Vasicek <vasicek AT fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use IEEE.STD_LOGIC_UNSIGNED.ALL; 

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity ram is
 port (
   CLK   : in std_logic; -- hodiny

   ADDR  : in std_logic_vector(9 downto 0);  -- adresa bunky
   WDATA : in std_logic_vector(7 downto 0);  -- data pro zapis
   RDATA : out std_logic_vector(7 downto 0); -- nactena data (v dalsim taktu, pokud EN=1)
   WREN  : in std_logic;                     -- 0 - cteni, 1 - zapis
   EN    : in std_logic                      -- 1 - povoleni prace s pameti
 );
end ram;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of ram is

   type t_ram is array (0 to 2**10-1) of std_logic_vector (7 downto 0);
   signal ram: t_ram := (others => X"00");
   signal rd : std_logic_vector (7 downto 0) := (others => '0');
begin
   RDATA <= rd;
   
   -- RAM rd / wr
   sram_mem: process (CLK)
   begin
      if (CLK'event) and (CLK = '1') then
         if (EN = '1') then
            if (WREN = '0') then
               rd <= ram(conv_integer(ADDR));
            else
               ram(conv_integer(ADDR)) <= WDATA;
               rd <= WDATA;
            end if;
         end if;
      end if;
   end process;
   
end behavioral;

