INCLUDE=-I'include'

CFLAGS=$(INCLUDE) $(SIZEFLAG) -g

OBJ=         obj/stree.o\
             obj/naive_search.o\
			 obj/get_alpha.o\
             obj/spaceman.o\
             obj/io.o\
             obj/skip_count.o\
			 obj/init.o\
             obj/clock.o\
             obj/scan.o\
             obj/insert.o\
             obj/location.o\
             obj/util.o\
			 obj/chain.o\
             obj/construct.o

TEST_OBJ = obj/test_search.o obj/test_experiments.o

dirs:
	mkdir -p obj bin

run: clean dirs mcc
	./bin/mcc /home/lsund/Data/testdata/akz/data.xml

quarter: clean dirs mcc
	./bin/mcc /home/lsund/Data/testdata/doctronic/data-diffsize/quarter.xml

half: clean dirs mcc
	./bin/mcc /home/lsund/Data/testdata/doctronic/data-diffsize/half.xml

full: clean dirs mcc
	./bin/mcc /home/lsund/Data/testdata/doctronic/data-diffsize/full.xml

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
