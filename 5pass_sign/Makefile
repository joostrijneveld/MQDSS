CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -funroll-loops -std=c11
# uncomment this when compiling for non-avx2 platforms
# CFLAGS += -DREFERENCE
CFLAGS += -mavx2

SOURCES = mq.c randombytes.c sign.c gf31.c
HEADERS = mq.h randombytes.h sign.h gf31.h params.h

OBJS = ../KeccakCodePackage/bin/Haswell/libkeccak.a
# OBJS += ../KeccakCodePackage/bin/generic64/libkeccak.a

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

tests: $(TESTS)
measure: $(MEASURE)

test/%: test/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $< -lcrypto

measure/%: measure/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $< -lcrypto

.PHONY: clean
.PRECIOUS: $(OBJS)

clean:
	rm -f *.o
	rm -f measure/*.o
	rm -f $(TESTS)
	rm -f $(MEASURE)
