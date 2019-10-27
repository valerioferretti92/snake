# make all / make --> sna ke and sqlite3_shell executables in ./build
# make snake --> create snake executable in folder ./build
# make sqlite3_shell --> create sqlite3 shell executable to test sql commands in ./build
# make clean --> clear all executables and object files in ./build

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
LIB_DIR = lib
SQLITE3_DIR = ${LIB_DIR}/sqlite3
SNAKE_DLIBS = -ldl -lncurses -pthread
SQLITE3_SHELL_DLIBS = -ldl -pthread
OBJS = ${BUILD_DIR}/sqlite3.o ${BUILD_DIR}/snake.o
EXE = snake
SQLITE3_SHELL = sqlite3

#all
all: snake sqlite3_shell

#snake
snake: ${BUILD_DIR}/${EXE}
${BUILD_DIR}/${EXE}:${OBJS}
	g++ ${OBJS} -o $@ ${SNAKE_DLIBS}
${BUILD_DIR}/snake.o: ${SRC_DIR}/snake.cpp
	g++ -c ${SRC_DIR}/snake.cpp -o $@
${BUILD_DIR}/sqlite3.o: ${SQLITE3_DIR}/sqlite3.c
	gcc -c ${SQLITE3_DIR}/sqlite3.c -o $@

#sqlite3_shell
sqlite3_shell: ${BUILD_DIR}/sqlite3
${BUILD_DIR}/sqlite3: ${SQLITE3_DIR}/sqlite3.c ${SQLITE3_DIR}/shell.c
	gcc -I ${INCLUDE_DIR} ${SQLITE3_DIR}/sqlite3.c ${SQLITE3_DIR}/shell.c -o ${BUILD_DIR}/sqlite3 ${SQLITE3_SHELL_DLIBS}

#remove executables and object files
clean:
	rm ${BUILD_DIR}/*
