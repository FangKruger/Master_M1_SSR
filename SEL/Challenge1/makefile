CC = gcc
CFLAGS = -g -Wall -Werror
SRC = tp1.c
OBJ = tp1.o

main: $(OBJ)
	$(CC) -o tp1 $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

depend:
	makedepend $(SRC)
		
clean :
	rm  *.o
	rm tp1
	rm *.txt
 
