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

override CFLAGS+=-I$(INCLUDEDIR) $(SIZEFLAG)
##CFLAGS=${DEFINECFLAGS} -I$(INCLUDEDIR) $(SIZEFLAG)
##LDFLAGS=${DEFINELDFLAGS}
SPLINTFLAGS=${SIZEFLAG} -I${INCLUDEDIR} -f ../Splintoptions -DDEBUG

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

OBJECTS=     construct.o\
             access.o\
             scanpref.o\
             linkloc.o\
             depthtab.o\
             ex2leav.o\
             dfs.o\
             overmax.o\
             oversucc.o\
             addleafcount.o\
             iterator.o

.SUFFIXES: .o .a .x

all: libs progs

libs: libstree.a

progs: stree.x loc.x

include Filegoals.mf

libstree.a: $(OBJECTS)
	ar sruv $@ $(OBJECTS)

stree.x: stree.o libstree.a
	$(LD) $(LDFLAGS) stree.o libstree.a $(LIBBASE) -o $@

loc.x: loc.o libstree.a
	$(LD) $(LDFLAGS) loc.o libstree.a $(LIBBASE) -o $@

.PHONY:clean
clean:
	rm -f *.[aox] *~

include Dependencies.mf
