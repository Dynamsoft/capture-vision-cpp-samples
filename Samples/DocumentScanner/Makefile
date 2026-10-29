CC=gcc
CCFLAGS=-c -std=c++11

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT_DIR := $(abspath $(MAKEFILE_DIR)/../..)

DS_LIB_PATH=$(ROOT_DIR)/Dist/Lib/Linux/x64
DS_JSON_PATH=$(ROOT_DIR)/Dist/Templates/DDN-PresetTemplates.json
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=$(MAKEFILE_DIR)/DocumentScanner
OBJECT=$(MAKEFILE_DIR)/DocumentScanner.o
SOURCE=$(MAKEFILE_DIR)/DocumentScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	mkdir -p $(DS_LIB_PATH)/Templates/
	cp $(DS_JSON_PATH) $(DS_LIB_PATH)/Templates/

# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE) -o $(OBJECT)

# the clean target
.PHONY : clean
clean:
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/Templates