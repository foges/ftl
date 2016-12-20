all: test

INC=include/ftl/enumeration.h \
    include/ftl/ftl.h \
    include/ftl/seq.h \
    include/ftl/zip.h

FLAGS=-O3 -std=c++14

test: test.cpp $(INC)
	g++-6 $< -o $@ $(FLAGS) -Iinclude
	./$@
