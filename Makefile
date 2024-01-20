target: run

build: display.o main.o
	g++ -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf

display.o:
	g++ -c display.cpp

main.o:
	g++ -c main.cpp

run: clean build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
