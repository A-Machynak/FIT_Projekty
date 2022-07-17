# IPK Projekt 2

Implementace klienta a serveru Simple File Transfer protokolu pro linux dle RFC 913.

## Instalace
Přeložení klienta a serveru:
```
$ make
```

Spuštění serveru:
```
$ ./ipk-simpleftp-server {-i rozhraní} {-p ­­port} [-u cesta_soubor] [-f cesta_k_adresari]
```

Spuštění klienta:
```
$ ./ipk-simpleftp-client [-h IP] {-p port} [-f cesta_k_adresari]
```

## Použití
Server je implementován dle RFC 913 a tudíž podporuje všechny příkazy tohoto protokolu kromě příkazu `TYPE {A|B|C}`, který nemá žádný vliv na způsob přenosu.

Příklad:
```
Server:
$ ./ipk-simpleftp-server -u ./data/credentials -f ./data/server_working_directory -i eth0
[sftp INFO]: Found interface eth0 with ip 172.29.170.27
[sftp INFO]: Bound successfully (IP/host: 172.29.170.27 Port: 115)
[sftp INFO]: Listen()ing on socket...

Klient:
$ ./ipk-simpleftp-client -f ./data/client_working_directory -h 172.29.170.27
[sftp INFO]: Connected successfully (IP/host: 172.29.170.27 Port: 115)
[sftp INFO]: Write "DONE" to disconnect

+ IPK_SFTP - SFTP Service
USER user1
+ User-id valid, send password
PASS 123456 
! Logged in
LIST V
+ 
test_file1      f       25
test_file4      f       20
test_folder     d       4096
LIST V test_folder
+ test_folder
test_file1      f       25
test_file2      f       14
LIST V ../
- Couldn't list content because: Directory/File not found
DONE
+
[sftp INFO]: Disconnecting...
```

### Autor
Augustin Machyňák - xmachy02@stud.fit.vutbr.cz
