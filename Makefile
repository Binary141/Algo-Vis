target: run

build: display.o main.o
	g++ -o algo-vis main.o display.o -lSDL2

display.o:
	g++ -c display.cpp

main.o:
	g++ -c main.cpp

run: build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
