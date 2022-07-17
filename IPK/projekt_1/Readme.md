# IPK Projekt 1

Minimalistický http server pro linux.

## Instalace
Pro spuštění serveru použijte:

```
$ make
$ ./hinfosvc [číslo portu]
```

## Použití

Server má v základu implementované pouze 3 cesty:
```
$ ./hinfosvc 8080
[bHttp]: Initializing socket
[bHttp]: Binding socket to 8080
[bHttp]: Listen()ing on socket...

$ curl localhost:8080/hostname
xmachy02
$ curl localhost:8080/cpu-name
Intel(R) Core(TM) i7-7700K CPU @ 4.20GHz
$ curl localhost:8080/load
5.05%
```

Pro implementaci své vlastní cesty je potřeba vytvoření funkce 
`std::string fn(bHttp::context_t ctx)` - například:
```
std::string fn(bHttp::context_t ctx) {
    std::string body = "Response text";

    return bHttp::ResponseBuilder::begin()
        .statusCode(bHttp::OK)
        .setHeader("Content-Type", "text/plain;charset=utf-8")
        .body(body)
        .build();
}
```
a před zapnutím serveru (`server.run()`) přiřazení této funkce k určité cestě:
```
server.addPathFn("/cesta", fn);
```

### Autor
Augustin Machyňák - xmachy02@stud.fit.vutbr.cz
