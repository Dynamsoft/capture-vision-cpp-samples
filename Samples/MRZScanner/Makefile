CC=gcc
CCFLAGS=-c -std=c++11

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT_DIR := $(abspath $(MAKEFILE_DIR)/../..)

DLRMODEL_PATH=$(ROOT_DIR)/Dist/Models
DCPRES_PATH=$(ROOT_DIR)/Dist/ParserResources
CONFUSABLE_CHARS_PATH=$(ROOT_DIR)/Dist/ConfusableChars.data
OVERLAPPING_CHARS_PATH=$(ROOT_DIR)/Dist/OverlappingChars.data
DS_JSON_PATH=$(ROOT_DIR)/Dist/Templates
DS_LIB_PATH=$(ROOT_DIR)/Dist/Lib/Linux/x64
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense -lDynamsoftDocumentNormalizer -lDynamsoftLabelRecognizer -lDynamsoftIdentityUtility

STDLIB=-lstdc++

TARGET=$(MAKEFILE_DIR)/MRZScanner
OBJECT=$(MAKEFILE_DIR)/MRZScanner.o
SOURCE=$(MAKEFILE_DIR)/MRZScanner.cpp

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
	$(CC) $(CCFLAGS) $(SOURCE) -o $(OBJECT)

# the clean target
.PHONY : clean
clean:
	rm -f $(OBJECT) $(TARGET) $(DS_LIB_PATH)/ConfusableChars.data $(DS_LIB_PATH)/OverlappingChars.data -r $(DS_LIB_PATH)/Models $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates
