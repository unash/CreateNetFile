CreateNetFile_C = ../CreateNetFile/CreateNetFile.cpp
SupportFunction_C = ../CreateNetFile/SupportFunction.cpp
STDAFX_C =  ../CreateNetFile/stdafx.cpp

STDAFX_O =  ./build/stdafx.o
TARGET = ./build/main
DIR = build
TEST_DATA = tab_file_test.txt

OBJECTS = $(CreateNetFile_C) $(SupportFunction_C) $(STDAFX_O)

all:$(TARGET) | $(DIR)
	cp ../Data/$(TEST_DATA) ./build/
	cd build/ && ./main -f ./$(TEST_DATA)
$(TARGET):$(OBJECTS) | $(DIR)
	g++ -g $(OBJECTS) -o $@
$(STDAFX_O):$(STDAFX_C) | $(DIR)
	g++ -c $(STDAFX_C) -o $@
$(DIR):
	mkdir -p build
clean:
	rm -rf build
	rm -f $(TARGET)