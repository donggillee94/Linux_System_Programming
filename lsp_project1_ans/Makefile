ssu_make : main.o graph.o macro_list.o
	gcc main.o graph.o macro_list.o -o ssu_make

main.o:main.c main.h
	gcc -c $*.c 

graph.o:graph.c graph.h
	gcc -c graph.c 

macro_list.o : macro_list.c macro_list.h
	gcc -c macro_list.c 
