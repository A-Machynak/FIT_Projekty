Implementační dokumentace k 2. úloze do IPP 2021/2022

Jméno a příjmení: Augustin Machyňák

Login: xmachy02

# Popis řešení interpret.py
Interpret prvně pomocí knihovny `xml.etree` přečte vstupní soubor, zkontroluje jeho formát 
a uloží každou instrukci (třída `Instruction`) a její argumenty (třída `Argument`), 
kde každý argument se skládá z typu (bool, nil, string, label...) a hodnoty a instrukce se skládá z 0 - 3 argumentů, pořadového čísla (`order`) a operačního kódu (`opcode`).
Následně jsou postupně procházeny instrukce a volány příslušné funkce s příslušnými
argumenty. 
S proměnnými se v interpreteru pracuje pouze přes rozhraní třídy `Frame`, 
které obsahuje funkce pro definování, získání a nastavení hodnoty proměnné. 

# Popis řešení test.php
Testovací rámec test.php využívá `RecursiveDirectoryIterator` pro (rekurzivní, pokud je nastaven `--recursive`,) procházení adresářů. Prvně jsou nalezeny všechny soubory, které obsahují `.src`, `.in`, `.out` nebo `.rc`, cesty k souborům uloženy do třídy `TestFile` 
a ten následně uložen do pole. Následně jsou postupně procházeny tyto soubory, spuštěn interpreter, popř. parser, pomocí `exec()` a porovnány výsledky.

## Rozšíření interpreteru
### FLOAT a STACK
Interpreter je rozšiřen o FLOAT a STACK instrukce.

### NVI
Při návrhu a implementaci byl využit návrhový vzor z OOP. 
Konkrétně návrhový vzor `Factory method` ve třídě `ArgumentValueFactory` (viz řádek ~150). 

`ArgumentValueFactory` obsahuje metodu `create(type, text)`, 
která vrací hodnotu nebo instanci konkrétní třídy na základě parametru `type`.
Tento návrhový vzor je implementován poměrně neobvyklým způsobem, 
ale bohužel nebyl k dispozici žádný rozumný programovací jazyk, 
ve kterém by použití návrhových vzorů mělo alespoň nějaký význam.

V test.php byl také využit návrhový vzor `Iterator` ve třídě `RecursiveDirectoryIteratorWrapper` (viz řádek ~210), která prvně projde všechny soubory v adresáři 
(popř. rekurzivně), uloží a pomocí metody `getNext()` je možné je postupně procházet.

