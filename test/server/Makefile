include ../../Inc.mk

TLIBS=  -larch  $(LIBS)

TEST_OBJECTS := channel_server.o

TEST_TARGET = ss

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)