PROG=pixmaps

all: $(PROG)

run: all
	@./$(PROG)

$(PROG): $(PROG).o
	gcc -o $(PROG) $(PROG).o -lX11 -lXext -lXi -lm

$(PROG).o: $(PROG).c
	gcc $(CFLAGS) -c $(PROG).c

.phony: clean run

clean:
	@rm -f *.o $(PROG)
