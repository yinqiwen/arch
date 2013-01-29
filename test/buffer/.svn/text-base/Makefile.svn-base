include ../../Inc.mk


TLIBS= -larch $(GTEST_LIB) $(LIBS)

TEST_OBJECTS := BufferUnitTest.o

TEST_TARGET = ch

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)