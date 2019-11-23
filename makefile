# make all / make --> snake and sqlite3_shell executables in ./build
# make snake --> create snake executable in folder ./build
# make sqlite3_shell --> create sqlite3 shell executable to test sql commands in ./build
# make clean --> clear all executables and object files in ./build

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib
INCLUDE_DIR = include
EXT_INCLUDE_DIR = ${LIB_DIR}/ext_include
SQLITE3_DIR = ${LIB_DIR}/src/sqlite3
SNAKE_DLIBS = -ldl -lncurses -pthread
SQLITE3_SHELL_DLIBS = -ldl -pthread
OBJS = ${BUILD_DIR}/sqlite3.o ${BUILD_DIR}/snake.o ${BUILD_DIR}/board.o    \
       ${BUILD_DIR}/database.o ${BUILD_DIR}/point.o ${BUILD_DIR}/printer.o \
			 ${BUILD_DIR}/utils.o ${BUILD_DIR}/main.o
EXE = snake
SQLITE3_SHELL = sqlite3

#all
all: snake sqlite3_shell

#snake
snake: ${BUILD_DIR}/${EXE}
${BUILD_DIR}/${EXE}:${OBJS}
	g++ -g ${OBJS} -o $@ ${SNAKE_DLIBS}
${BUILD_DIR}/snake.o: ${SRC_DIR}/snake.cpp ${INCLUDE_DIR}/snake.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/snake.cpp -o $@
${BUILD_DIR}/board.o: ${SRC_DIR}/board.cpp ${INCLUDE_DIR}/board.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/board.cpp -o $@
${BUILD_DIR}/database.o: ${SRC_DIR}/database.cpp ${INCLUDE_DIR}/database.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/database.cpp -o $@
${BUILD_DIR}/point.o: ${SRC_DIR}/point.cpp ${INCLUDE_DIR}/point.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/point.cpp -o $@
${BUILD_DIR}/printer.o: ${SRC_DIR}/printer.cpp ${INCLUDE_DIR}/printer.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/printer.cpp -o $@
${BUILD_DIR}/utils.o: ${SRC_DIR}/utils.cpp ${INCLUDE_DIR}/utils.h
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/utils.cpp -o $@
${BUILD_DIR}/main.o: ${SRC_DIR}/main.cpp
	g++ -g -I ${INCLUDE_DIR} -c ${SRC_DIR}/main.cpp -o $@
${BUILD_DIR}/sqlite3.o: ${SQLITE3_DIR}/sqlite3.c ${EXT_INCLUDE_DIR}/sqlite3.h
	gcc -g -I ${INCLUDE_DIR} -c ${SQLITE3_DIR}/sqlite3.c -o $@

#sqlite3 shell
sqlite3_shell: ${BUILD_DIR}/sqlite3
${BUILD_DIR}/sqlite3: ${SQLITE3_DIR}/sqlite3.c ${SQLITE3_DIR}/shell.c
	gcc -I ${EXT_INCLUDE_DIR} ${SQLITE3_DIR}/sqlite3.c ${SQLITE3_DIR}/shell.c -o ${BUILD_DIR}/sqlite3 ${SQLITE3_SHELL_DLIBS}

#remove executables and object files
clean:
	rm ${BUILD_DIR}/*
