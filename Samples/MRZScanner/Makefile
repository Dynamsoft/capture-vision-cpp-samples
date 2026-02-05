CC=gcc
CCFLAGS=-c -std=c++11

DLRMODEL_PATH=../../Dist/Models
DCPRES_PATH=../../Dist/ParserResources
CONFUSABLE_CHARS_PATH=../../Dist/ConfusableChars.data
OVERLAPPING_CHARS_PATH=../../Dist/OverlappingChars.data
DS_JSON_PATH=../../Dist/Templates
DS_LIB_PATH=../../Dist/Lib/Linux/x64
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense -lDynamsoftDocumentNormalizer -lDynamsoftLabelRecognizer -lDynamsoftIdentityUtility

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
	cp $(OVERLAPPING_CHARS_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)

# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean:
	rm -f $(OBJECT) $(TARGET) $(DS_LIB_PATH)/ConfusableChars.data $(DS_LIB_PATH)/OverlappingChars.data -r $(DS_LIB_PATH)/Models $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates
