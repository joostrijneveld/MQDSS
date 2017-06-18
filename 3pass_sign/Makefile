CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -O3 -funroll-loops -std=c11

SOURCES = mq.c randombytes.c sign.c
HEADERS = mq.h randombytes.h sign.h params.h

OBJS = gen_MQ.s gen_G.s gen_ROL256.s
OBJS += ../KeccakCodePackage/bin/Haswell/libkeccak.a

# I feel like there should be a better way to do this..
CFLAGS += -I../KeccakCodePackage/Modes\
		  -I../KeccakCodePackage/Common\
		  -I../KeccakCodePackage/Constructions\
		  -DKeccakP200_excluded\
		  -DKeccakP400_excluded\
		  -DKeccakP800_excluded\
		  -I../KeccakCodePackage/SnP/KeccakP-1600/OptimizedAVX2\

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASUREMENTS = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measurements

tests: $(TESTS)
measurements: $(MEASUREMENTS)

gen_%.s: asmgen/gen_%.py
	python3 $^ > $@

test/%: test/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $< -lcrypto

measure/%: measure/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $< -lcrypto

.PHONY: clean
.PRECIOUS: $(OBJS)

clean:
	rm -f *.o
	rm -f measure/tools.o
	rm -f $(TESTS)
	rm -f $(MEASUREMENTS)
	rm -f gen_*.s
	rm -rf asmgen/__pycache__
