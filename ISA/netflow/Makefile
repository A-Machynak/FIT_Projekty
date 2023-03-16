# Makefile pro spuštění cmake

all:
	mkdir -p build/
	cd build/;\
	cmake .. -G "Unix Makefiles"
	cd build/src;\
	make;\
	mv flow ../../

clean:
	rm -rf build/
	rm -f flow
	rm -f xmachy02.tar

zip:
	tar -cf xmachy02.tar flow.1 Makefile CMakeLists.txt src/* manual.pdf