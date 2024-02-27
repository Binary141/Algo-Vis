target: run

build: main.o search.o display.o astar.o greedy.o dfs.o bfs.o
	g++ -o algo-vis main.o display.o astar.o greedy.o bfs.o dfs.o search.o -lSDL2 -lSDL2_ttf -lSDL2_image

debug: search.o display.o dfs.o bfs.o greedy.o astar.o debug-main.o
	g++ -o algo-vis main.o bfs.o dfs.o astar.o greedy.o display.o search.o -lSDL2 -lSDL2_ttf
	gdb ./algo-vis

search.o: search.cpp search.h
	g++ -c search.cpp

display.o: display.cpp display.h
	g++ -c display.cpp

greedy.o: greedy.cpp search.h
	g++ -c greedy.cpp

astar.o: astar.cpp search.h
	g++ -c astar.cpp

bfs.o: bfs.cpp search.h
	g++ -c bfs.cpp

dfs.o: dfs.cpp search.h
	g++ -c dfs.cpp

main.o: main.cpp
	g++ -c main.cpp

debug-main.o: main.cpp
	g++ -c -g main.cpp

run: build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
