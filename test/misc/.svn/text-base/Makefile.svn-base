include ../../Inc.mk

TLIBS=  -larch $(GTEST_LIB) $(LIBS)

TEST_OBJECTS := misc_test.o

TEST_TARGET = misc_test

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)