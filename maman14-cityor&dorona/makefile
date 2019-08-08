assembler: parse_file.o first_tran.o second_tran.o main.o check_file.o
	gcc -g -ansi -Wall -pedantic parse_file.o first_tran.o second_tran.o main.o check_file.o -o assembler

main.o: main.c parse_file.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o

parse_file.o: parse_file.c parse_file.h
	gcc -c -ansi -Wall -pedantic parse_file.c -o parse_file.o

first_tran.o: first_tran.c first_tran.h	
	gcc -c -ansi -Wall -pedantic first_tran.c -o first_tran.o

second_tran.o: second_tran.c second_tran.h
	gcc -c -ansi -Wall -pedantic second_tran.c -o second_tran.o

check_file.o: check_file.c check_file.h
	gcc -c -ansi -Wall -pedantic check_file.c -o check_file.o