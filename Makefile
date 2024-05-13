CXX = g++
CXXFLAGS = -std=c++0x -Wall -pedantic-errors -g

SRCS = sqlite.cpp
OBJS = ${SRCS:.cpp=.o}
HEADERS = 

MAIN = sqlite

all: ${MAIN}
	@echo   Simple database named sqlite has been compiled

${MAIN}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${MAIN}

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${PROGS} ${MAIN} ${OBJS} *.o *~. 
