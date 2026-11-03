CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude
THREAD_FLAGS ?= -pthread

APP_SOURCES := $(wildcard src/*.cpp)
APP_HEADERS := $(wildcard include/*.h)
BUILD_DIR := build
APP := $(BUILD_DIR)/miniso
TEST_CONCURRENCIA := $(BUILD_DIR)/test_concurrencia
TEST_MEMORIA := $(BUILD_DIR)/test_memoria
TEST_PROCESOS := $(BUILD_DIR)/test_procesos

.PHONY: all test test-concurrencia test-memoria test-procesos clean

all: $(APP)

$(APP): $(APP_SOURCES) $(APP_HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(THREAD_FLAGS) $(APP_SOURCES) -o $@

$(TEST_CONCURRENCIA): src/concurrencia.cpp tests/test_concurrencia.cpp include/concurrencia.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(THREAD_FLAGS) src/concurrencia.cpp tests/test_concurrencia.cpp -o $@

$(TEST_MEMORIA): src/memoria.cpp tests/test_memoria.cpp include/memoria.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) src/memoria.cpp tests/test_memoria.cpp -o $@

$(TEST_PROCESOS): src/procesos.cpp tests/test_procesos.cpp include/procesos.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) src/procesos.cpp tests/test_procesos.cpp -o $@

$(BUILD_DIR):
	mkdir -p $@

test: test-concurrencia test-memoria test-procesos

test-concurrencia: $(TEST_CONCURRENCIA)
	./$(TEST_CONCURRENCIA)

test-memoria: $(TEST_MEMORIA)
	./$(TEST_MEMORIA)

test-procesos: $(TEST_PROCESOS)
	./$(TEST_PROCESOS)

clean:
	rm -rf $(BUILD_DIR)