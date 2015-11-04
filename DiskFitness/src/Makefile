CC = gcc
MPICC = mpicc
CFLAGS = -O2 -g
CPPFLAGS =
LDFLAGS = -static
MPI_LDFLAGS =
LIBS = -lm

OBJS = config.o config_aux.o utils.o tests.o
MPI_OBJS = mpi_utils.o

all: diskfitness mpi_diskfitness

mpi_diskfitness: $(OBJS) $(MPI_OBJS) mpi_main.o
	$(MPICC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(MPI_LDFLAGS) $(LIBS)

diskfitness: $(OBJS) main.o 
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

mpi_%.o: mpi_%.c
	$(MPICC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f core *.o diskfitness mpi_diskfitness

install: all
	cp diskfitness ..
