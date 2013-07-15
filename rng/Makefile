CC = g++
#OPT1 = -g  #debug
OPT1 = -O3 # optimized
CFLAGS = $(OPT1)
LFLAGS = $(CFLAGS) -L.

OBJS = rng.o
DEPS = rng.h

LIB = librng.a

all: $(LIB)
clean:
	rm *.o

# Executables
$(LIB): $(OBJS)
	rm -f $@
	ar cq $@ $(OBJS)
        
# Modules
rng.o: rng.h rng.C
	$(CC) $(CFLAGS) -c -o $@ rng.C


