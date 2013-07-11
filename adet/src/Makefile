CC = g++
#OPT1 = -g  #debug
OPT1 = -O3 # optimized
CFLAGS = $(OPT1) -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
LFLAGS = $(CFLAGS) -L$(RNGDIR) -L$(MATFUNDIR) -L$(KFDIR)/lib -L$(RBPFDIR)/lib

# LEAKTRACER = /home/djhill1/LeakTracer/LeakTracer.o
# COMPONENT DIRECTORIES
BNUTILDIR = /home/djhill1/programs/BayesianMethods/Util
SRE1DIR = /home/djhill1/programs/BayesianMethods/SRE_1
K2DIR = /home/djhill1/programs/BayesianMethods/Kalman2
ADFDIR = /home/djhill1/programs/BayesianMethods/ADF
RBPFDIR = /home/djhill1/programs/RBPF
KFDIR = /home/djhill1/programs/KalmanFilter
MATFUNDIR = /home/djhill1/programs/MATRIX_TCL/djh_auxfun
RNGDIR = /home/djhill1/programs/rng

INCLUDES =  -I /home/djhill1/programs/MATRIX_TCL/include -I $(RNGDIR) -I $(KFDIR)/src -I $(MATFUNDIR) -I $(RBPFDIR)/src -I $(ADFDIR) -I $(K2DIR) -I $(SRE1DIR) -I $(BNUTILDIR)

OBJS = main_aux.o corr_analysis.o predict_naive.o predict_percep.o predict_ann.o predict_cluster.o naive.o perceptron_lin.o nnet.o agglom_cluster.o coord.o k_means.o k_meanspredict.o make_ifiles.o tabulate_results.o timestamp.o ts_record.o filefun.o predict_bpnn.o nn_node.o nn_layer.o nn.o predict_ptron.o stats.o ml_predictoradet.o bayesian_adet.o errorfun.o mathfun.o adet_filters.o embedding_dim.o stationary_test.o

DEPS = main_aux.h corr_analysis.h predict_naive.h predict_percep.h predict_ann.o predict_cluster.o naive.h perceptron_lin.h nnet.h agglom_cluster.h coord.h k_means.h k_meanspredict.h make_ifiles.h tabulate_results.h timestamp.h ts_record.h filefun.h predict_bpnn.h nn_node.h nn_layer.h nn.h predict_ptron.h stats.h ml_predictor.h ml_predictoradet.h bayesian_adet.h errorfun.h mathfun.h adet_filters.h embedding_dim.h stationary_test.h\
	$(KFDIR)/lib/libkalman.a $(RBPFDIR)/lib/librbpf.a

all: adet adet_leak
#all: adet

# Executables
adet: adet.o $(OBJS) $(DEPS)
	$(CC) $(LFLAGS) -o $@ adet.o $(OBJS) -lrbpf -lkalman -lrng -lmat_tcl_djhfun
	mv adet ../bin

adet_leak: adet.o $(OBJS) $(DEPS)
	$(CC) $(LFLAGS) -o $@ adet.o $(OBJS) $(LEAKTRACER) -lrbpf -lkalman -lrng -lmat_tcl_djhfun
	mv adet_leak ../bin
        
# Modules
adet.o: adet.cpp $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ adet.cpp

main_aux.o: main_aux.h main_aux.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ main_aux.cpp

corr_analysis.o: corr_analysis.h corr_analysis.cpp main_aux.h ts_record.h filefun.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ corr_analysis.cpp

make_ifiles.o: make_ifiles.h make_ifiles.cpp main_aux.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ make_ifiles.cpp

predict_naive.o: predict_naive.h predict_naive.cpp main_aux.h naive.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ predict_naive.cpp

predict_percep.o: predict_percep.h predict_percep.cpp main_aux.h perceptron_lin.h filefun.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ predict_percep.cpp

predict_ann.o: predict_ann.h predict_ann.cpp main_aux.h nnet.h filefun.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ predict_ann.cpp

predict_cluster.o: predict_cluster.h predict_cluster.cpp main_aux.h k_meanspredict.h filefun.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ predict_cluster.cpp

naive.o: naive.h naive.cpp filefun.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ naive.cpp

perceptron_lin.o: perceptron_lin.h perceptron_lin.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ perceptron_lin.cpp

nnet.o: nnet.h nnet.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ nnet.cpp

agglom_cluster.o: agglom_cluster.h agglom_cluster.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ agglom_cluster.cpp

coord.o: coord.h coord.cpp
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ coord.cpp

k_means.o: k_means.h k_means.cpp coord.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ k_means.cpp

k_meanspredict.o: k_meanspredict.h k_meanspredict.cpp k_means.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ k_meanspredict.cpp

tabulate_results.o: tabulate_results.h tabulate_results.cpp
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ tabulate_results.cpp
        
timestamp.o: timestamp.h timestamp.cc
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ timestamp.cc

ts_record.o: ts_record.h ts_record.cc timestamp.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ ts_record.cc

filefun.o: filefun.h filefun.cc ts_record.h main_aux.h stats.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ filefun.cc

predict_bpnn.o: predict_bpnn.h predict_bpnn.cpp main_aux.h nn.h filefun.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ predict_bpnn.cpp

nn_node.o: nn_node.h nn_node.cc
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ nn_node.cc

nn_layer.o: nn_layer.h nn_layer.cc nn_node.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ nn_layer.cc

nn.o: nn.h nn.cc nn_layer.h ml_predictor.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ nn.cc
        
predict_ptron.o: predict_ptron.h predict_ptron.cpp nn.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ predict_ptron.cpp
        
stats.o: stats.h stats.cc ts_record.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ stats.cc
        
ml_predictoradet.o: ml_predictoradet.h ml_predictoradet.cpp main_aux.h filefun.h nn.h naive.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ ml_predictoradet.cpp
        
bayesian_adet.o: bayesian_adet.h bayesian_adet.cc main_aux.h filefun.h adet_filters.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ bayesian_adet.cc
        
adet_filters.o: adet_filters.h adet_filters.cc bayesian_adet.h $(ADFDIR)/mog_adf.h $(ADFDIR)/mog_adf.cc $(ADFDIR)/mog_adf_ol.h $(ADFDIR)/mog_adf_ol.cc $(K2DIR)/kalman2.h $(K2DIR)/kalman2.cc $(SRE1DIR)/sre1.h $(SRE1DIR)/sre1.cc $(BNUTILDIR)/datafun.h $(BNUTILDIR)/datafun.cc $(BNUTILDIR)/statfun.h $(BNUTILDIR)/statfun.cc
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ adet_filters.cc        

errorfun.o: errorfun.h errorfun.cc filefun.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ errorfun.cc

mathfun.o: mathfun.h mathfun.cc
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ mathfun.cc
        
embedding_dim.o: embedding_dim.h embedding_dim_v05.cc main_aux.h filefun.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ embedding_dim_v05.cc

stationary_test.o: stationary_test.h stationary_test_v01.cc main_aux.h filefun.h
	$(CC) $(CFLAGS) -c $(INCLUDES) -o $@ stationary_test_v01.cc
        
%.h:

clean:
	rm *.o

