#
# Copyright (c) 2003 by Stefan Kurtz and The Institute for
# Genomic Research.  This is OSI Certified Open Source Software.
# Please see the file LICENSE for licensing information and
# the file ACKNOWLEDGEMENTS for names of contributors to the
# code base.
#
# Modified by Ludvig Sundström 2018 under permission by Stefan Kurtz.


INCLUDE=-I'include'

CFLAGS=$(INCLUDE) $(SIZEFLAG) -g

#-DSTARTFACTOR=0.5

OBJ=         obj/construct.o\
             obj/naive_search.o\
             obj/access.o\
             obj/space.o\
             obj/io.o\
             obj/clock.o\
             obj/seterror.o\
             obj/scanpref.o\
             obj/linkloc.o\
             obj/depthtab.o\
             obj/dfs.o\
             obj/oversucc.o\
             obj/iterator.o\
             obj/normaltree.o

TEST_OBJ = obj/test_search.o

dirs:
	mkdir -p obj bin

run: clean dirs mcc
	./bin/mcc data/data.xml

runtest: clean test
	./bin/test

test: dirs ${OBJ} ${TEST_OBJ}
	${CC} ${CFLAGS} ${OBJ} ${TEST_OBJ} test/test.c -o bin/test

mcc: ${OBJ}
	${CC} ${CFLAGS} ${OBJ} src/main.c -o bin/$@

obj/%.o:src/%.c
	$(CC) $(CFLAGS) -c src/$*.c -o $@

obj/test_%.o:test/test_%.c
	$(CC) $(CFLAGS) -c test/test_$*.c -o $@

.PHONY:clean
clean:
	rm -rf obj bin
