include ../../Inc.mk

TLIBS=-larch $(LIBS)  

TEST_OBJECTS := channel_client.o

TEST_TARGET = cc

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)