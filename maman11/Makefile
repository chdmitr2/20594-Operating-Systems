all: binsem.a ut.a
FLAGS = -Wall  -L./ -m32

binsem.a:
	gcc $(FLAGS)  -c binsem.c
	ar rcu libbinsem.a binsem.o
	ranlib libbinsem.a 

ut.a:
	gcc $(FLAGS)  -c ut.c
	ar rcu libut.a ut.o
	ranlib libut.a 

clean:
	rm -f *.o 
	rm -f a.out
	rm -f *~
	rm -f ph
	rm -f *a 
