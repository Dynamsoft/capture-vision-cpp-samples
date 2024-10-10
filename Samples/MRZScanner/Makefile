CC=gcc
CCFLAGS=-c -std=c++11

DLRMODEL_PATH=../../Distributables/CharacterModel
DCPRES_PATH=../../Distributables/ParserResources
CONFUSABLE_CHARS_PATH=../../Distributables/ConfusableChars.data
DS_JSON_PATH=../../Distributables/Templates
DS_LIB_PATH=../../Distributables/Lib/Linux/x64
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=MRZScanner
OBJECT=MRZScanner.o
SOURCE=MRZScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	cp -r $(DLRMODEL_PATH) $(DS_LIB_PATH)
	cp -r $(DCPRES_PATH) $(DS_LIB_PATH)
	cp $(CONFUSABLE_CHARS_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)

# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean: 
	rm -f $(OBJECT) $(TARGET) $(DS_LIB_PATH)/ConfusableChars.data -r $(DS_LIB_PATH)/CharacterModel $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates
