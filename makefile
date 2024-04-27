all :build

build:
	g++ *.cpp -O2 -g -o main -Wall -Wextra -Werror
run: build
	./main
