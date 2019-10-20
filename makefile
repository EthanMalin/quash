CC=gcc
DEPS = inputblock.h quashutils.h path.h quashcontext.h
OBJ = main.o inputblock.o quashutils.o path.o quashcontext.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm main *.o *~
