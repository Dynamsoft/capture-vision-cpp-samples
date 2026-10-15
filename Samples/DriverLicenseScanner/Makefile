CC=gcc
CCFLAGS=-c -std=c++11

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT_DIR := $(abspath $(MAKEFILE_DIR)/../..)

DCPRES_PATH=$(ROOT_DIR)/Dist/ParserResources
DS_LIB_PATH=$(ROOT_DIR)/Dist/Lib/Linux/x64
DS_JSON_PATH=$(ROOT_DIR)/Dist/Templates
DS_MODEL_PATH=$(ROOT_DIR)/Dist/Models
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=$(MAKEFILE_DIR)/DriverLicenseScanner
OBJECT=$(MAKEFILE_DIR)/DriverLicenseScanner.o
SOURCE=$(MAKEFILE_DIR)/DriverLicenseScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	cp -r $(DCPRES_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)
	cp -r $(DS_MODEL_PATH) $(DS_LIB_PATH)
# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE) -o $(OBJECT)

# the clean target
.PHONY : clean
clean:
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates $(DS_LIB_PATH)/Models