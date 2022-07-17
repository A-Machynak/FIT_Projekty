
CPPFLAGS=-g -Wall -Werror -pedantic
LDFLAGS=-g

hinfosvc: hinfosvc.o
	g++ $(LDFLAGS) -o hinfosvc hinfosvc.o BasicHTTP.o

hinfosvc.o: hinfosvc.cpp BasicHTTP.o
	g++ $(CPPFLAGS) -c hinfosvc.cpp

BasicHTTP.o: src/BasicHTTP.cpp inc/BasicHTTP.h
	g++ $(CPPFLAGS) -c src/BasicHTTP.cpp

clean:
	rm -f BasicHTTP.o hinfosvc.o hinfosvc xmachy02.zip

zip:
	zip xmachy02.zip inc/* src/* hinfosvc.cpp Makefile Readme.md
