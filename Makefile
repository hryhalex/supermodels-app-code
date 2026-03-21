CC = gcc
CXX = g++
CFLAGS = -Iinclude -Wall -Wextra -std=c11
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17
LDFLAGS = -lsqlite3 -lm

# Пути к OpenSSL (для Mac Apple Silicon)
OPENSSL_INCLUDE = -I/opt/homebrew/opt/openssl@3/include
OPENSSL_LIB = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

# Google Test (универсальные пути)
ifneq ($(wildcard /usr/include/gtest/gtest.h),)
    GTEST_INCLUDE = -I/usr/include
    GTEST_LIB = -L/usr/lib -lgtest -lgtest_main -pthread
else ifneq ($(wildcard /opt/homebrew/include/gtest/gtest.h),)
    GTEST_INCLUDE = -I/opt/homebrew/include
    GTEST_LIB = -L/opt/homebrew/lib -lgtest -lgtest_main -pthread
else
    GTEST_INCLUDE = -I$(HOME)/Documents/googletest/googletest/include
    GTEST_LIB = -L$(HOME)/Documents/googletest/build/lib -lgtest -lgtest_main -pthread
endif

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
TARGET = $(BINDIR)/Greenhouse.exe

# Тесты
TEST_SOURCES = $(wildcard tests_gtest/test_*.cpp)
TEST_OBJECTS = $(patsubst tests_gtest/%.cpp, $(OBJDIR)/test_%.o, $(TEST_SOURCES))
TEST_TARGET = $(BINDIR)/run_tests

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(OPENSSL_LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(OPENSSL_INCLUDE) -c $< -o $@

# Сборка и запуск тестов
$(TEST_TARGET): $(TEST_OBJECTS) $(filter-out $(OBJDIR)/main.o, $(OBJECTS)) | $(BINDIR)
	$(CXX) $^ -o $(TEST_TARGET) $(GTEST_LIB) $(LDFLAGS) $(OPENSSL_LIB)

$(OBJDIR)/test_%.o: tests_gtest/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDE) $(OPENSSL_INCLUDE) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o $(TARGET) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

coverage-test: clean
	mkdir -p obj
	$(CC) $(CFLAGS) $(OPENSSL_INCLUDE) -fprofile-arcs -ftest-coverage -c src/*.c
	mv *.o obj/
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDE) $(OPENSSL_INCLUDE) -fprofile-arcs -ftest-coverage -c tests_gtest/*.cpp
	mv *.o obj/
	$(CXX) obj/*.o -o $(TEST_TARGET) $(GTEST_LIB) $(LDFLAGS) $(OPENSSL_LIB) --coverage
	./$(TEST_TARGET)
	lcov --directory . --capture --output-file coverage.info --ignore-errors inconsistent
	lcov --remove coverage.info '/usr/*' '/opt/homebrew/*' 'tests_gtest/*' 'src/main.c' --output-file coverage.info --ignore-errors unused
	lcov --list coverage.info

.PHONY: all clean run test coverage-test
