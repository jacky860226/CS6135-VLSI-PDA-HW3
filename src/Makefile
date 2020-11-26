CXX := g++
TARGET := ../bin/hw3
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra
SRC_DIRS := .
SRCS := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TEST_CASE_ID := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TEST_CASE_ID):;@:)
endif

test: $(TARGET)
	@echo test on case $(TEST_CASE_ID) 0.1
	/usr/bin/time -p ./$(TARGET)  ../testcase/$(TEST_CASE_ID).hardblocks ../testcase/$(TEST_CASE_ID).nets ../testcase/$(TEST_CASE_ID).pl ../output/$(TEST_CASE_ID).floorplan 0.1
	../verifier/verify ../testcase/$(TEST_CASE_ID).hardblocks ../testcase/$(TEST_CASE_ID).nets ../testcase/$(TEST_CASE_ID).pl ../output/$(TEST_CASE_ID).floorplan 0.1
	@echo test on case $(TEST_CASE_ID) 0.15
	/usr/bin/time -p ./$(TARGET)  ../testcase/$(TEST_CASE_ID).hardblocks ../testcase/$(TEST_CASE_ID).nets ../testcase/$(TEST_CASE_ID).pl ../output/$(TEST_CASE_ID).floorplan 0.15
	../verifier/verify ../testcase/$(TEST_CASE_ID).hardblocks ../testcase/$(TEST_CASE_ID).nets ../testcase/$(TEST_CASE_ID).pl ../output/$(TEST_CASE_ID).floorplan 0.15

.PHONY: all clean test
-include $(DEPS)