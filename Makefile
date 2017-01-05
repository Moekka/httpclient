
EXECS=HttpClient
CFLAGS += -Os
LDFLAGS += -L./lib

OBJS=client.o

all: $(EXECS)

.c.o:
	$(CC) -c $(CFLAGS) $<

$(EXECS): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)
clean:
	rm -f *.o *.a $(EXECS)