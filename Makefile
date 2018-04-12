#-- Imported variables from top level makefile
# BIN_DIR AUX_BIN_DIR CXX CC CFLAGS CXXFLAGS LDFLAGS

##include ../Makedef

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
             obj/stree.o\
             obj/iterator.o

.SUFFIXES: .o .a .x

all: dirs mccreight

dirs:
	mkdir -p obj bin

# libs: libstree.a

# progs: stree.x loc.x


libstree.a: $(OBJECTS)
	ar sruv $@ $(OBJECTS)

stree.x: stree.o libstree.a
	$(LD) $(LDFLAGS) stree.o libstree.a $(LIBBASE) -o $@

loc.x: loc.o libstree.a
	$(LD) $(LDFLAGS) loc.o libstree.a $(LIBBASE) -o $@


mccreight: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o bin/$@

obj/%.o:src/%.c
	$(LD) $(CFLAGS) -c src/$*.c -o $@

.PHONY:clean
clean:
	rm -rf obj bin
