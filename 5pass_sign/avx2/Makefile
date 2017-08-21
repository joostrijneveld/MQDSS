CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -funroll-loops -std=c11 -march=native

SOURCES = mq.c randombytes.c sign.c gf31.c fips202.c
HEADERS = mq.h randombytes.h sign.h gf31.h fips202.h params.h

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASURE = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measure

tests: $(TESTS)
measure: $(MEASURE)

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
	-$(RM) test/keypair
	-$(RM) test/sign
	-$(RM) test/open
	-$(RM) test/mq
