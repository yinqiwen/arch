include ../../Inc.mk

TLIBS=  -larch $(LIBS)

TEST_OBJECTS := MyFramework.o MyManagerHandler.o MyManagerProcess.o MyServiceHandler.o

TEST_TARGET = fr

all:$(TEST_TARGET)

$(TEST_TARGET):	$(TEST_OBJECTS) 
	$(CXX) -o $(TEST_TARGET) $(OBJECTS) $(TEST_OBJECTS) $(TLIBS)
	
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGET)