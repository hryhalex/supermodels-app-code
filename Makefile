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

.PHONY: all clean run
