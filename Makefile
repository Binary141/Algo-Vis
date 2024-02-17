target: run

build: main.o search.o display.o astar.o greedy.o dfs.o bfs.o
	g++ -o algo-vis main.o display.o astar.o greedy.o bfs.o dfs.o search.o -lSDL2 -lSDL2_ttf -lSDL2_image
	# g++ -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf

debug: search.o display.o dfs.o bfs.o greedy.o astar.o debug-main.o
	g++ -o algo-vis main.o bfs.o dfs.o astar.o greedy.o display.o search.o -lSDL2 -lSDL2_ttf
	# g++ -o algo-vis main.o display.o -lSDL2 -lSDL2_ttf
	gdb ./algo-vis

search.o:
	g++ -c search.cpp

display.o:
	g++ -c display.cpp
	# g++ -c display.cpp

greedy.o:
	g++ -c greedy.cpp

astar.o:
	g++ -c astar.cpp

bfs.o:
	g++ -c bfs.cpp

dfs.o:
	g++ -c dfs.cpp

main.o:
	g++ -c main.cpp
	# g++ -c main.cpp

debug-main.o:
	g++ -c -g main.cpp
	# g++ -c main.cpp

run: clean build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f *.o
	rm -f out
