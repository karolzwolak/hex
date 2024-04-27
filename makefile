all :build

build:
	g++ *.cpp -g2 -g -o main -Wall -Wextra -Werror
run: build
	./main
