include ../../Inc.mk

TLIBS=  -larch $(LIBS)

TEST_OBJECTS := main.o http_benchmark.o

TEST_TARGET = hb

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)