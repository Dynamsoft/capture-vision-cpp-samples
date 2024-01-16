CC=gcc
CCFLAGS=-c -std=c++11

DLRMODEL_PATH="../../../Distributables/CharacterModel"
DS_LIB_PATH="../../../Distributables/Lib/Linux/x64"
DS_JSON_PATH=../../../Distributables/*.json
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=CaptureFromMultipleImages
OBJECT=CaptureFromMultipleImages.o
SOURCE=CaptureFromMultipleImages.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	cp -r $(DLRMODEL_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)

# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean: 
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/CharacterModel