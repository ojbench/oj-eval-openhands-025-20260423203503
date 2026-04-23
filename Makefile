CXXFLAGS=-g -O2 -MMD -std=c++17
BINS=eval cheat anticheat code
SRCS=lang.cpp main.cpp eval.cpp cheat.cpp anticheat.cpp
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

all: code

code: main.o lang.o
	$(CXX) -o $@ $^

eval: eval.o lang.o
	$(CXX) -o $@ $^

cheat: cheat.o lang.o
	$(CXX) -o $@ $^

anticheat: anticheat.o lang.o
	$(CXX) -o $@ $^

.PHONY: clean
clean:
	rm -f $(BINS) $(OBJS) $(DEPS)

-include *.d
