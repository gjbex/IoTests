HDF5_DIR = /usr
CC = gcc
CFLAGS = -O2 -g
CPPFLAGS = -I$(HDF5_DIR)/include
LDFLAGS = -L$(HDF5_DIR)/lib
LIBS = -lhdf5 -lm

WRITER_OBJS = writer_cl_aux.o writer_cl.o utils.o writer.o
READER_OBJS = reader_cl_aux.o reader_cl.o utils.o reader.o

all: writer reader

writer: $(WRITER_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

reader: $(READER_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

clean:
	rm -f *.o writer reader
