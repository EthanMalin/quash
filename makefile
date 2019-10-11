CC=gcc
CFLAGS=-I
DEPS = inputblock.h quashutils.h path.h quashcontext.h
OBJ = main.o inputblock.o quashutils.o path.o quashcontext.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm main *.o *~
