CC=cc

CFLAGS=-x c -std=c17
CFLAGS+= -Wall -Werror
CFLAGS+= -fstack-protector-all
CFLAGS+= -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wmissing-declarations
CFLAGS+= -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+= -Wsign-compare

#CFLAGS+= -O3 -DNDEBUG
CFLAGS+= -g

LDFLAGS=

all: scm scmrpl

# generate scmrpl.o from scm.c, disabling eval()
scmrpl.o: scm.c
	${CC} ${CFLAGS} -DNO_EVAL=1 $< -c -o $@

scm: scmmem.o scmerr.o scmrdr.o scmval.o scmprt.o scmspl.o scmevl.o scm.o
	${CC} $^ ${LDFLAGS} -o $@

scmrpl: scmmem.o scmerr.o scmrdr.o scmval.o scmprt.o scmspl.o scmrpl.o
	${CC} $^ ${LDFLAGS} -o $@

.PHONY: test
test: scm scmrpl
	env PATH=$$(pwd):$${PATH} kyua test || true
	kyua report-html --force

.PHONY: deps
deps:
	mkmf > .dependencies
	cat .dependencies |grep "^scm.o:" |sed s/"^scm.o"/"scmrpl.o"/ |sed s/" scmevl.h"/""/

.PHONY: clean
clean:
	rm -f scm scmrpl
	rm -f *.o
	rm -f *.core
	rm -f *~

include .dependencies
