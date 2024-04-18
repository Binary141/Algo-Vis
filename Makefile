target: run

build: main.o search.o display.o astar.o greedy.o dfs.o bfs.o
	g++ -o algo-vis build/main.o build/display.o build/astar.o build/greedy.o build/bfs.o build/dfs.o build/search.o -lSDL2 -lSDL2_ttf -lSDL2_image

debug: search.o display.o dfs.o bfs.o greedy.o astar.o debug-main.o
	g++ -o algo-vis build/main.o build/bfs.o build/dfs.o build/astar.o build/greedy.o build/display.o build/search.o -lSDL2 -lSDL2_ttf
	gdb ./algo-vis

search.o: src/search.cpp src/search.h
	g++ -o build/search.o -c src/search.cpp

display.o: src/display.cpp src/display.h
	g++ -o build/display.o -c src/display.cpp

greedy.o: src/greedy.cpp src/search.h
	g++ -o build/greedy.o -c src/greedy.cpp

astar.o: src/astar.cpp src/search.h
	g++ -o build/astar.o -c src/astar.cpp

bfs.o: src/bfs.cpp src/search.h
	g++ -o build/bfs.o -c src/bfs.cpp

dfs.o: src/dfs.cpp src/search.h
	g++ -o build/dfs.o -c src/dfs.cpp

main.o: src/main.cpp
	g++ -o build/main.o -c src/main.cpp

debug-main.o: src/main.cpp
	g++ -c -g src/main.cpp

run: build
	./algo-vis

clean:
	rm -f algo-vis
	rm -f build/*.o
