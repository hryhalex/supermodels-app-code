CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -std=c11
LDFLAGS = -lsqlite3 -lm

# Пути к OpenSSL (для Mac Apple Silicon)
OPENSSL_INCLUDE = -I/opt/homebrew/opt/openssl@3/include
OPENSSL_LIB = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
TARGET = $(BINDIR)/Greenhouse.exe

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(OPENSSL_LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(OPENSSL_INCLUDE) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Тесты
TEST_SOURCES = $(wildcard tests/test_*.c)
TEST_OBJECTS = $(patsubst tests/%.c, $(OBJDIR)/test_%.o, $(TEST_SOURCES))
TEST_TARGET = $(BINDIR)/run_tests

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS) $(filter-out $(OBJDIR)/main.o, $(OBJECTS)) | $(BINDIR)
	$(CC) $^ -o $(TEST_TARGET) $(LDFLAGS) $(OPENSSL_LIB) -lcunit

$(OBJDIR)/test_%.o: tests/test_%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(OPENSSL_INCLUDE) -c $< -o $@

coverage:
	$(CC) $(CFLAGS) $(OPENSSL_INCLUDE) -fprofile-arcs -ftest-coverage -c src/*.c
	$(CC) *.o -o $(TEST_TARGET) $(LDFLAGS) $(OPENSSL_LIB) -lcunit -lgcov
	./$(TEST_TARGET)

.PHONY: all clean run test coverage
