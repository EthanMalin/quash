CC=gcc
DEPS = inputblock.h quashutils.h path.h quashcontext.h
OBJ = main.o inputblock.o quashutils.o path.o quashcontext.o

all: quash

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

quash: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm quash *.o *~
