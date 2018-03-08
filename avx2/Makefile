CC = /usr/bin/cc
CFLAGS = -Wall -Wextra -Wpedantic -O3 -mavx2

SOURCES =          mq.c randombytes.c gf31.c fips202.c sign.c
HEADERS = params.h mq.h randombytes.h gf31.h fips202.h api.h

DET_SOURCES = $(SOURCES:randombytes.%=rng.%)

TESTS = $(patsubst test/test_%.c,test/test_%,$(wildcard test/test_*.c))

.PHONY: clean test

all: tests

tests: $(TESTS)

test: $(TESTS:=.exec)

PQCgenKAT_sign: PQCgenKAT_sign.c $(DET_SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(DET_SOURCES) $< $(LDLIBS) -lcrypto

test/%: test/%.c $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $< $(LDLIBS)

test/%.exec: test/%
	@$<

clean:
	rm -f $(TESTS)
	-$(RM) PQCgenKAT_sign
	-$(RM) PQCsignKAT_*.rsp
	-$(RM) PQCsignKAT_*.req
	-$(RM) test/keypair
	-$(RM) test/sign
	-$(RM) test/open
	-$(RM) test/mq
