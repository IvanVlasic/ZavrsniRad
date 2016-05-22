CXX = g++
WARN = -Wall 
CFLAGS = $(WARN) -std=c++11
LDFLAGS = $(WARN) -lecf

OBJS = RegEvalOp.o \
			 main.o

DEPS = RegEvalOp.h TrainingTestValuesOperator.h

BINFILE = reg

.PHONY = all clean

all: $(BINFILE)

$(BINFILE): $(OBJS)
				$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp $(DEPS)
				$(CXX) $(CFLAGS) $< -c -o $@

clean:
				rm -f *~ $(OBJS) $(BINFILE)
