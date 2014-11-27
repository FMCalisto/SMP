TARGETS = mm1_l2
PAPILIB=../../papi/papi-5.2.0/src/libpapi.a
CFLAGS=-O1

all: $(TARGETS)

mm1_l2: mm1_l2.c
	$(CC) $(CFLAGS) -I ../../papi/papi-5.2.0/src mm1_l2.c $(PAPILIB) -o mm1_l2

clean:
	rm -f $(TARGETS) *.o *.stderr *.stdout core *~
