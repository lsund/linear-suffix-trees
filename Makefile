LIBBASEDIR=include
INCLUDEDIR=include

# the following flags determines the maximal allowed input size of
# the suffix tree construction
# STREESMALL means that the maximal sequence length is 2 MB
# STREELARGE means that the maximal sequence length is 134 MB
# STREEHUGE  means that the maximal sequence length is 500 MB

#SIZEFLAG=-DSTREESMALL
#SIZEFLAG=-DSTREELARGE
SIZEFLAG=-DSTREEHUGE

override CFLAGS+=-I'include' $(SIZEFLAG)
##CFLAGS=${DEFINECFLAGS} -I$(INCLUDEDIR) $(SIZEFLAG)
##LDFLAGS=${DEFINELDFLAGS}

LDFLAGS+=-m64

#-DSTARTFACTOR=0.5

LD=${CC}

LIBBASE=${LIBBASEDIR}/libbase.a

PROTOFILES=  access.c\
             scanpref.c\
             linkloc.c\
             depthtab.c\
             ex2leav.c\
             dfs.c \
             overmax.c\
             oversucc.c\
             addleafcount.c\
             iterator.c

OBJECTS=     obj/construct.o\
             obj/boyermoore.o\
             obj/access.o\
             obj/space.o\
             obj/clock.o\
             obj/mapfile.o\
             obj/seterror.o\
             obj/scanpref.o\
             obj/linkloc.o\
             obj/depthtab.o\
             obj/ex2leav.o\
             obj/dfs.o\
             obj/overmax.o\
             obj/oversucc.o\
             obj/addleafcount.o\
             obj/iterator.o

TEST_OBJ = obj/test_search.o

dirs:
	mkdir -p obj bin

stats: clean dirs mccreight
	./bin/mccreight data/data.xml

plain: clean dirs loc
	./bin/loc data/data.xml

mccreight: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} src/stree.c -o bin/$@

loc: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} src/loc.c -o bin/$@

obj/%.o:src/%.c
	$(LD) $(CFLAGS) -c src/$*.c -o $@

.PHONY:clean
clean:
	rm -rf obj bin
