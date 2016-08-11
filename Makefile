C = gcc

all: pktgen router

pktgen: pktgen.o
	$(CC) pktgen.c -o $@

router: router.o   
	$(CC) router.c -o $@ 

clean: 
	rm -f *.o pktgen router
tar: 
	tar -cvf allFiles.tar *.c Makefile
