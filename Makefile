target: run

build: display.o main.o
	gcc -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf
	# g++ -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf

display.o:
	gcc -c display.cpp
	# g++ -c display.cpp

main.o:
	gcc -c main.cpp
	# g++ -c main.cpp

run: clean build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
