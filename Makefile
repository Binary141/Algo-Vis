target: run

build:
	g++ main.cpp -o out -lSDL2

run: build
	./out

clean:
	rm out
