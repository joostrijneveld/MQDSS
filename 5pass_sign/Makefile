CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -funroll-loops -std=c11

# toggle this when compiling for non-avx2 platforms
PLATFORM = Haswell
# PLATFORM = generic64

ifeq ($(PLATFORM), Haswell)
	CFLAGS += -mavx2
else
	CFLAGS += -DREFERENCE
endif

SOURCES = mq.c randombytes.c sign.c gf31.c
HEADERS = mq.h randombytes.h sign.h gf31.h params.h

OBJS = ../KeccakCodePackage/bin/$(PLATFORM)/libkeccak.a

# this can be done much nicer with more recent KCP versions
CFLAGS += -I../KeccakCodePackage/Modes\
		  -I../KeccakCodePackage/Common\
		  -I../KeccakCodePackage/Constructions\
		  -DKeccakP200_excluded\
		  -DKeccakP400_excluded\
		  -DKeccakP800_excluded

# these headers are not AVX2-specific, but need to be included in any case
CFLAGS += -I../KeccakCodePackage/SnP/KeccakP-1600/OptimizedAVX2

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASURE = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measure

tests: $(TESTS)
measure: $(MEASURE)

../KeccakCodePackage/bin/$(PLATFORM)/libkeccak.a:
	if [ ! -d "../KeccakCodePackage" ] ; then\
		git clone https://github.com/gvanas/KeccakCodePackage ../KeccakCodePackage;\
		cd ../KeccakCodePackage; git reset --hard f40509b;\
	fi
	cd ../KeccakCodePackage; make $(PLATFORM)/libkeccak.a

test/%: test/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $<

measure/%: measure/%.c $(SOURCES) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(OBJS) $<

.PHONY: clean
.PRECIOUS: $(OBJS)

clean:
	rm -f *.o
	rm -f measure/*.o
	rm -f $(TESTS)
	rm -f $(MEASURE)

distclean:
	rm -rf ../KeccakCodePackage
