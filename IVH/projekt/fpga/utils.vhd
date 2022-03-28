library ieee;
use ieee.std_logic_1164.all;

package utils is

type DIRECTION_T is (DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN, DIR_UL, DIR_UR, DIR_DL, DIR_DR);
function GETID (X, Y : integer) return integer;

end utils;

package body utils is

function GETID (X, Y : integer) return integer is
	constant COLS : integer := 16;
	constant ROWS : integer := 8;
begin
	return (((X mod COLS) * ROWS) + (Y mod ROWS));
end;


end utils;