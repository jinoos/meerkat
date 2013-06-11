.SUFFUXES : .h .c .o

OBJS = meerkat.o client.o server.o logging.o message.o request.o meerkat_server.o hash.o service.o

CC = gcc
CFLAGS = -Wall -g -c 

LDFLAGS_APR	= $(shell pkg-config --libs apr-1)
LDFLAGS_LIBXML = $(shell pkg-config --libs libxml-2.0)
LDFLAGS_LIBEVENT = $(shell pkg-config --libs libevent)

INC_APR	= $(shell pkg-config --cflags apr-1)
INC_LIBXML = $(shell pkg-config --cflags libxml-2.0)
INC_LIBEVENT = $(shell pkg-config --cflags libevent)

LDFLAGS	= $(LDFLAGS_APR) $(LDFLAGS_LIBXML) $(LDFLAGS_LIBEVENT)
INC = -I../include $(INC_APR) $(INC_LIBXML) $(INC_LIBEVENT)

SRCS = $(OBJS:.o=.c)

ARS = 


TARGET = meerkat_server

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(ARS) $(LDFLAGS)

.c.o :
	$(CC) $(INC) $(CFLAGS) $<

run :
	./meerkat_server -c ../conf/sample.xml --debug

gdb :
	gdb ./meerkat_server

clean : 
	rm -rf $(OBJS) $(TARGET) core 

client :
	$(CC) -Wall -g -o meerkat_client meerkat_client.c $(ARS) $(LDFLAGS) $(INC) 

testrun :
	$(CC) -Wall -g -o testrun testrun.c $(ARS) $(LDFLAGS) $(INC)
