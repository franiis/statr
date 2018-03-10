OPT = -fPIC -g3 -I .

.PHONY: clean header

library: statr
	gcc $(OPT) -shared -o statr_lib.so rng.o array_utils.o distr.o -Wall
	cp statr_lib.so ../build/statr_lib.so

tester: statr
	gcc -c -g -I . -o tester-main.o main.c
	gcc -o tester tester-main.o rng.o array_utils.o distr.o

statr: rng.o array_utils.o distr.o

rng.o: utils/rng.c utils/rng.h
	gcc $(OPT) -c -o rng.o utils/rng.c

array_utils.o: utils/array_utils.c utils/array_utils.h
	gcc $(OPT) -c -o array_utils.o utils/array_utils.c

distr.o: distr/runif.c distr/distr.h
	gcc $(OPT) -c -o distr.o distr/runif.c

clean:
	rm *.o

header: 
	gcc -c prech.h -o prech.h.gch