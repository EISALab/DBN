CC = g++
#OPT1 = -g  #debug
OPT1 = -O3 # optimized
CFLAGS = $(OPT1) -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
OFLAGS = $(CFLAGS)

LEAKTRACER = /home/djhill1/LeakTracer/LeakTracer.o

OBJFILES = main_aux.o corr_analysis.o predict_naive.o predict_percep.o predict_ann.o predict_cluster.o naive.o perceptron_lin.o nnet.o agglom_cluster.o coord.o k_means.o k_meanspredict.o rng.o make_ifiles.o tabulate_results.o timestamp.o ts_record.o filefun.o predict_bpnn.o nn_node.o nn_layer.o nn.o
DEPS     = main_aux.h corr_analysis.h predict_naive.h predict_percep.h predict_ann.o predict_cluster.o naive.h  perceptron_lin.h nnet.h agglom_cluster.h coord.h k_means.h k_meanspredict.h rng.h make_ifiles.h tabulate_results.h timestamp.h ts_record.h filefun.h predict_bpnn.h nn_node.h nn_layer.h nn.h predictor.h

all: adet adet_leak

# Executables
adet: adet.o $(OBJFILES) $(DEPS)
	$(CC) $(OFLAGS) -o $@ adet.o $(OBJFILES)

adet_leak: adet.o $(OBJFILES) $(DEPS)
	$(CC) $(OFLAGS) -o $@ adet.o $(OBJFILES) $(LEAKTRACER)
        
# Modules
adet.o: adet.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ adet.cpp

main_aux.o: main_aux.h main_aux.cpp rng.h
	$(CC) $(CFLAGS) -c -o $@ main_aux.cpp

corr_analysis.o: corr_analysis.h corr_analysis.cpp main_aux.h
	$(CC) $(CFLAGS) -c -o $@ corr_analysis.cpp

make_ifiles.o: make_ifiles.h make_ifiles.cpp main_aux.h
	$(CC) $(CFLAGS) -c -o $@ make_ifiles.cpp

predict_naive.o: predict_naive.h predict_naive.cpp main_aux.h naive.h
	$(CC) $(CFLAGS) -c -o $@ predict_naive.cpp

predict_percep.o: predict_percep.h predict_percep.cpp main_aux.h perceptron_lin.h filefun.h
	$(CC) $(CFLAGS) -c -o $@ predict_percep.cpp

predict_ann.o: predict_ann.h predict_ann.cpp main_aux.h nnet.h filefun.h
	$(CC) $(CFLAGS) -c -o $@ predict_ann.cpp

predict_cluster.o: predict_cluster.h predict_cluster.cpp main_aux.h k_meanspredict.h filefun.h
	$(CC) $(CFLAGS) -c -o $@ predict_cluster.cpp

naive.o: naive.h naive.cpp filefun.h
	$(CC) $(CFLAGS) -c -o $@ naive.cpp

perceptron_lin.o: perceptron_lin.h perceptron_lin.cpp
	$(CC) $(CFLAGS) -c -o $@ perceptron_lin.cpp

nnet.o: nnet.h nnet.cpp
	$(CC) $(CFLAGS) -c -o $@ nnet.cpp

agglom_cluster.o: agglom_cluster.h agglom_cluster.cpp
	$(CC) $(CFLAGS) -c -o $@ agglom_cluster.cpp

coord.o: coord.h coord.cpp
	$(CC) $(CFLAGS) -c -o $@ coord.cpp

k_means.o: k_means.h k_means.cpp coord.h
	$(CC) $(CFLAGS) -c -o $@ k_means.cpp

k_meanspredict.o: k_meanspredict.h k_meanspredict.cpp k_means.h
	$(CC) $(CFLAGS) -c -o $@ k_meanspredict.cpp

rng.o: rng.h rng.C
	$(CC) $(CFLAGS) -c -o $@ rng.C

tabulate_results.o: tabulate_results.h tabulate_results.cpp
	$(CC) $(CFLAGS) -c -o $@ tabulate_results.cpp
        
timestamp.o: timestamp.h timestamp.cc
	$(CC) $(CFLAGS) -c -o $@ timestamp.cc

ts_record.o: ts_record.h ts_record.cc timestamp.h
	$(CC) $(CFLAGS) -c -o $@ ts_record.cc

filefun.o: filefun.h filefun.cc ts_record.h main_aux.h
	$(CC) $(CFLAGS) -c -o $@ filefun.cc

predict_bpnn.o: predict_bpnn.h predict_bpnn.cpp main_aux.h nn.h filefun.h
	$(CC) $(CFLAGS) -c -o $@ predict_bpnn.cpp

nn_node.o: nn_node.h nn_node.cc rng.h
	$(CC) $(CFLAGS) -c -o $@ nn_node.cc

nn_layer.o: nn_layer.h nn_layer.cc nn_node.h
	$(CC) $(CFLAGS) -c -o $@ nn_layer.cc

nn.o: nn.h nn.cc nn_layer.h predictor.h
	$(CC) $(CFLAGS) -c -o $@ nn.cc
        
%.h:

clean:
	rm *.o

