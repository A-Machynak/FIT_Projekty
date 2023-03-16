
all:
	cmake -S . -B build/ -G "Unix Makefiles"
	make -C build/
ifeq ($(OS),Windows_NT)
	copy .\build\src\visualization\CA_Visualization.exe .
else
	cp build/src/visualization/CA_Visualization .
endif
