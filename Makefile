CPP = gcc
FLAGS = -Wall -g 

EXEC = main
OBJS = random.o assignment6.o  

default: ${EXEC}

clean:
	rm -f ${EXEC}
	rm -f *.o

run: ${EXEC}
	./${EXEC}

debug: ${EXEC}
	ddd ./${EXEC}

gdb: ${EXEC}
	gdb ./${EXEC}

# Linking step now includes both assignment2.o and main.o
${EXEC}: ${OBJS}
	${CPP} ${FLAGS} -o ${EXEC} ${OBJS} -lm -lpthread

.c.o:
	${CPP} ${FLAGS} -c $<

valgrind: ${EXEC}
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes  ./${EXEC}

# Object file rules for the source files
random.o: random.c random.h
assignment6.o: assignment6.c
