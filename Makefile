target: run

build: search.o display.o main.o
	g++ -o algo-vis main.o display.o search.o -lSDL2 -lSDL2_ttf
	# g++ -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf

search.o:
	g++ -c search.cpp

display.o:
	g++ -c display.cpp
	# g++ -c display.cpp

main.o:
	g++ -c main.cpp
	# g++ -c main.cpp

run: clean build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
