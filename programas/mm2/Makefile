TARGETS = mm2
PAPILIB=../../papi/papi-5.2.0/src/libpapi.a
CFLAGS=-O1

all: $(TARGETS)

mm2: mm2.c
	$(CC) $(CFLAGS) -I ../../papi/papi-5.2.0/src mm2.c $(PAPILIB) -o mm2

clean:
	rm -f $(TARGETS) *.o *.stderr *.stdout core *~
