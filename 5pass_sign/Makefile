CC = /usr/bin/gcc
CPP = /usr/bin/g++
CFLAGS = -Wall -Wextra -O3 -mavx2 -faggressive-loop-optimizations -funroll-loops -std=c11

OBJS = randombytes.o sign.o params.h gf31.o mq.o measure/tools.o
OBJS += ../KeccakCodePackage/bin/Haswell/libkeccak.a

DEPS = params.h mq.h randombytes.h sign.h gf31.h measure/tools.h

CFLAGS += -I../KeccakCodePackage/Modes\
		  -I../KeccakCodePackage/Common\
		  -I../KeccakCodePackage/Constructions\
		  -DKeccakP200_excluded\
		  -DKeccakP400_excluded\
		  -DKeccakP800_excluded\
		  -I../KeccakCodePackage/SnP/KeccakP-1600/OptimizedAVX2\

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASURE = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measure

mq_test: $(OBJS) mq_test.o
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto

tests: $(TESTS)
measure: $(MEASURE)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

test/%: test/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto

measure/%: measure/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -lcrypto

.PHONY: clean
.PRECIOUS: $(OBJS)

clean:
	rm -f *.o
	rm -f measure/*.o
	rm -f $(TESTS)
	rm -f $(MEASURE)
