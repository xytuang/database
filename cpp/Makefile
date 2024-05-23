CXX = g++
CXXFLAGS = -std=c++0x -Wall -pedantic-errors -g

SRCS = main.cpp
OBJS = ${SRCS:.cpp=.o}
DEPS = constants.h enums.h row.h structs.h table.h 

MAIN = main

all: ${MAIN}

${MAIN}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${MAIN}

.cpp.o: ${DEPS}
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${PROGS} ${MAIN} ${OBJS} *.o *~. 
