CC=gcc
CCFLAGS=-c -std=c++11

DLRMODEL_PATH=../../Dist/Models
DCPRES_PATH=../../Dist/ParserResources
DS_JSON_PATH=../../Dist/Templates		
DS_LIB_PATH=../../Dist/Lib/Linux/x64
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=VINScanner
OBJECT=VINScanner.o
SOURCE=VINScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	cp -r $(DLRMODEL_PATH) $(DS_LIB_PATH)
	cp -r $(DCPRES_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)
	
# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean: 
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/Models $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates
