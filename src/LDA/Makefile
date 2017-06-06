objects = cvs.o cokus.o estimate.o inference.o lda.o util.o

lda.out : $(objects)
			g++ -o lda.out $(objects) -lgsl -lgslcblas -lm
cvs.o : CreateVectorSpace.cc
			g++ -c -o cvs.o CreateVectorSpace.cc
cokus.o : cokus.cc cokus.h
			g++ -c -o cokus.o cokus.cc
estimate.o : estimate.cc lda.h util.h inference.h
			g++ -c -o estimate.o estimate.cc
inference.o : inference.cc lda.h CreateVectorSpace.h
			g++ -c -o inference.o inference.cc
lda.o : lda.cc CreateVectorSpace.h cokus.h lda.h
			g++ -c -o lda.o lda.cc
util.o : util.cc util.h
			g++ -c -o util.o util.cc
clean : 
			rm lda.out $(objects)
