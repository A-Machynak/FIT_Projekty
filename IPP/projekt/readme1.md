Implementační dokumentace k 1. úloze do IPP 2021/2022

Jméno a příjmení: Augustin Machyňák

Login: xmachy02


# Popis řešení
Pro zjednodušení byly použity 3 třídy:
- `c_type` - reprezentující argument v instrukci
- `c_instruction_info` - reprezentující instrukci a její formát (počet a typy argumentů)
- `c_instruction` - reprezentující konkrétní instanci instrukce, která obsahuje již konkrétní argumenty

Následně proměnné `$instructions` (typu `array`) byly přiřazeny klíče a hodnoty, 
kde klíče jsou názvy instrukcí a hodnoty jsou instance `c_instruction_info`.
To umožňuje při nalezení názvu instrukce jednoduché zjištění očekávaných argumentů 
(použitím `$instructions['Nazev_Instrukce']`) či jestli tato instrukce vůbec existuje.

Kromě toho je řešení poměrně sebevysvětlující. 
Čtení probíhá řádek po řádku, při přečtení řádku je odebráno vše za znakem komentáře (`#`), 
řádek je rozdělen podle mezer na pole stringů, pokud první položka tohoto pole je klíčem v poli
`$instructions`, tak je vytvořena nová instance `c_instruction`, zkontrolován počet argumentů 
a každý argument je postupně zkontrolován a přidán. 

Jakmile jsou všechny argumenty v pořádku, je pomocí metody `toXML()` získán XML string 
obsahující XML reprezentaci této instrukce a uložen. 

Pokud řádek nezačíná řetězcem, který reprezentuje jednu z instrukcí, tak je následně provedena kontrola, 
jestli se nejedná o `label` (zakončen `:`). 
Pokud se nejedná o label, nastane chyba - neznámá instrukce (opcode).
