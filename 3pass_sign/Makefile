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

SOURCES = mq.c randombytes.c sign.c fips202.c
HEADERS = mq.h randombytes.h sign.h fips202.h params.h

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))
MEASUREMENTS = $(patsubst measure/measure_%.c,measure/measure_%,$(wildcard measure/measure_*.c))

all: tests measurements

tests: $(TESTS)
measurements: $(MEASUREMENTS)

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
	-$(RM) test/keypair
	-$(RM) test/sign
	-$(RM) test/open
