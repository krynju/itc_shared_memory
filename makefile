all: main

main: main.cpp monitor.h
	g++ -g -o main.out main.cpp monitor.h queue.h sema.h -I . -pthread $(F)

