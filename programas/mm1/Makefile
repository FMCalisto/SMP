TARGETS = mm1
PAPILIB=../../papi/papi-5.2.0/src/libpapi.a
CFLAGS=-O1

all: $(TARGETS)

mm1: mm1.c
	$(CC) $(CFLAGS) -I ../../papi/papi-5.2.0/src mm1.c $(PAPILIB) -o mm1

clean:
	rm -f $(TARGETS) *.o *.stderr *.stdout core *~
