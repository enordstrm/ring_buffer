CC=gcc
CXX=g++
CFLAGS=-g -O2 -Wall -Wextra -Iinclude -rdynamic $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,build/%.o,$(SOURCES))

TEST_SOURCES=$(wildcard tests/test_*.cpp)
TEST_OBJECTS=$(patsubst %.cpp,build/%.o,$(TEST_SOURCES))
TEST_TARGET=bin/test_ring_buffer

TARGET_NAME=ringbuffer
TARGET=build/lib$(TARGET_NAME).a
SO_TARGET=build/lib$(TARGET_NAME).so

# Default target
target: $(TARGET) $(SO_TARGET)

# Coverage
ifeq ($(COVERAGE), 1)
CFLAGS+=-ftest-coverage -fprofile-arcs --coverage
LDFLAGS+=-coverage
LDLIBS+=-lgcov

$(OBJECTS:.o=.gcda): test
$(TEST_OBJECTS:.o=.gcda): test

coverage/coverage.info: $(OBJECTS:.o=.gcda) $(TEST_SOURCES:.o=.gcda)
	@mkdir -p $(@D)
	lcov -d build/src --capture -o $@

coverage/index.html: coverage/coverage.info
	@mkdir -p $(@D)
	genhtml -o coverage $<

.PHONY: coverage
coverage: coverage/index.html
endif

# All
all: target test

# Static library
$(TARGET): CFLAGS += -fPIC
$(TARGET): $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

# SO library
$(SO_TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

# Objects
build/src/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -c -o $@

# Unit test objects
build/tests/%.o: tests/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $< -c -o $@

# Unit test target
$(TEST_TARGET): $(TEST_OBJECTS) $(TARGET)
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) $(TEST_OBJECTS) -Lbuild $(TARGET) -o $(TEST_TARGET)

# Unit test runner
.PHONY: test
test: $(TEST_TARGET)
	@$(TEST_TARGET)	

# Clean
clean:
	rm -rf build/*
	rm -rf bin/*

