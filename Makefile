SRC = src
INCLUDE = include
TEST = test
OBJ = obj
BIN = bin
TARGET = Ocomp

LIB_SRC = $(wildcard $(SRC)/*.c)
TEST_SRC = $(wildcard $(TEST)/*.c)

LIB_OBJ = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(LIB_SRC))
TEST_OBJ = $(patsubst $(TEST)/%.c,$(OBJ)/%.o,$(TEST_SRC))

TEST_BIN = $(BIN)/test_suite

CC = clang
CFLAGS = -std=c23 -Wall -Wextra -pedantic -g -fPIC -I$(INCLUDE) -Wno-newline-eof \
         -DEDU_RUNTIME_INCLUDE_DIR='"$(CURDIR)/$(INCLUDE)"' \
         -DEDU_RUNTIME_SRC='"$(CURDIR)/$(SRC)/edu_runtime.c"'

LDFLAGS = -lm

all: directories compile

run: all
	./$(BIN)/$(TARGET)
compile: $(LIB_OBJ)
	$(CC) -o $(BIN)/$(TARGET) $(LIB_OBJ) $(CDFLAGS) $(LDFLAGS)

directories:
	mkdir -p $(OBJ) $(BIN)

$(OBJ)/%.o: $(SRC)/%.c #$(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(TEST)/%.c #$(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

test: directories $(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $(TEST_BIN)

clean:
	rm -rf $(OBJ) $(BIN)

install:
	make
	cp $(BIN)/hello.out /usr/local/bin/hello
	
uninstall:
	rm -f $(DESTDIR)/usr/local/bin/hello

.PHONY: all directories tests clean install uninstall run-tests
	
