all :build

build:
	g++ *.cpp -g -o main -Wall -Wextra -Werror
run: build
	./main
