CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -O3

OBJS = randombytes.o sign.o params.h mq.o gen_MQ.S gen_G.S gen_ROL256.S measure/tools.o
OBJS += ../KeccakCodePackage/bin/Haswell/libkeccak.a

DEPS = params.h mq.h randombytes.h sign.h measure/tools.h

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

gen_%.S: asmgen/gen_%.py
	python3 $^ > $@

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
	rm -f measure/tools.o
	rm -f $(TESTS)
	rm -f $(MEASUREMENTS)
	rm -f gen_*.S
	rm -rf asmgen/__pycache__
