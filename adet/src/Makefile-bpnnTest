CC = g++
OPT1 = -g
CFLAGS = $(OPT1) -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DMV_VECTOR_BOUNDS_CHECK
OFLAGS = $(CFLAGS)

LEAKTRACER = /home/djhill1/LeakTracer/LeakTracer.o

OBJFILES = rng.o nn_node.o nn_layer.o nn.o
DEPS     = rng.h nn_node.h nn_layer.h nn.h

all: BPNN BPNN_leak

# Executables
BPNN: bpnn.o $(OBJFILES) $(DEPS)
	$(CC) $(OFLAGS) -o $@ bpnn.o $(OBJFILES)

BPNN_leak: bpnn.o $(OBJFILES) $(DEPS)
	$(CC) $(OFLAGS) -o $@ bpnn.o $(OBJFILES) $(LEAKTRACER)
        
# Modules
bpnn.o: bpnn.cc $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ bpnn.cc

rng.o: rng.h rng.C
	$(CC) $(CFLAGS) -c -o $@ rng.C

nn_node.o: nn_node.h nn_node.cc rng.h
	$(CC) $(CFLAGS) -c -o $@ nn_node.cc

nn_layer.o: nn_layer.h nn_layer.cc nn_node.h
	$(CC) $(CFLAGS) -c -o $@ nn_layer.cc

nn.o: nn.h nn.cc nn_layer.h
	$(CC) $(CFLAGS) -c -o $@ nn.cc

%.h:

clean:
	rm *.o

