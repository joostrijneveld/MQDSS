CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -funroll-loops -std=c11

# toggle this when compiling for non-avx2 platforms
PLATFORM = Haswell
# PLATFORM = generic64
# note that the C reference code can take a very long time to run

ifeq ($(PLATFORM), Haswell)
	CFLAGS += -mavx2
	OBJS = gen_MQ.s gen_G.s gen_ROL256.s
else
	CFLAGS += -DREFERENCE
endif

SOURCES = mq.c randombytes.c sign.c
HEADERS = mq.h randombytes.h sign.h params.h

OBJS += ../KeccakCodePackage/bin/$(PLATFORM)/libkeccak.a

# this can be done much nicer with more recent KCP versions
CFLAGS += -I../KeccakCodePackage/Modes\
		  -I../KeccakCodePackage/Common\
		  -I../KeccakCodePackage/Constructions\
		  -DKeccakP200_excluded\
		  -DKeccakP400_excluded\
		  -DKeccakP800_excluded\

# these headers are not AVX2-specific, but need to be included in any case
CFLAGS += -I../KeccakCodePackage/SnP/KeccakP-1600/OptimizedAVX2

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASUREMENTS = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measurements

tests: $(TESTS)
measurements: $(MEASUREMENTS)

../KeccakCodePackage/bin/$(PLATFORM)/libkeccak.a:
	if [ ! -d "../KeccakCodePackage" ] ; then\
		git clone https://github.com/gvanas/KeccakCodePackage ../KeccakCodePackage;\
		cd ../KeccakCodePackage; git reset --hard f40509b;\
	fi
	cd ../KeccakCodePackage; make $(PLATFORM)/libkeccak.a

gen_%.s: asmgen/gen_%.py
	python3 $^ > $@

test/%: test/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $<

measure/%: measure/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $<

.PHONY: clean
.PRECIOUS: $(OBJS)

clean:
	rm -f *.o
	rm -f measure/tools.o
	rm -f $(TESTS)
	rm -f $(MEASUREMENTS)
	rm -f gen_*.s
	rm -rf asmgen/__pycache__

distclean:
	rm -rf ../KeccakCodePackage
