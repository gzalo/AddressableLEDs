all: effects.exe

effects.exe: effects.cpp serialPort.h
	g++ effects.cpp -o effects.exe -lmingw32 -Wall -O2 -lws2_32 -lsdl2main -lsdl2
	
run: effects.exe
	effects
	
clean:
	del effects.exe
	
PHONY: run