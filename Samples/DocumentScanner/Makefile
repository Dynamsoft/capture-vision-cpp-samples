CC=gcc
CCFLAGS=-c -std=c++11

DS_LIB_PATH=../../Dist/Lib/Linux/x64
DS_JSON_PATH=../../Dist/Templates/DDN-PresetTemplates.json
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=DocumentScanner
OBJECT=DocumentScanner.o
SOURCE=DocumentScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	mkdir -p $(DS_LIB_PATH)/Templates/
	cp $(DS_JSON_PATH) $(DS_LIB_PATH)/Templates/

# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean: 
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/Templates