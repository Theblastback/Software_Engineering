CC = g++ -std=c++11
CFLAGS = -Wall
LIBS = -lSDL2 -lSDL2_ttf
OBJ = filelib.o myfile.o atr2func.o atr2.o

all: clean main clean_obj

.cc.o:
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)
	@printf "make: object: $(CC) -c -o $@ $< $(CFLAGS) $(LIBS)\n"

main: $(OBJ)
	$(CC) -o atr2 $^ $(CFLAGS) $(LIBS)

clean_obj:
	rm -f $(OBJ)

clean:
	rm -f $(OBJ) atr2