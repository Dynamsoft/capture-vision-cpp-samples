CC=gcc
CCFLAGS=-c -std=c++11

DCPRES_PATH=../../Dist/ParserResources
DS_LIB_PATH=../../Dist/Lib/Linux/x64
DS_JSON_PATH=../../Dist/Templates
DS_MODEL_PATH=../../Dist/Models
LDFLAGS=-L $(DS_LIB_PATH) -Wl,-rpath=$(DS_LIB_PATH) -Wl,-rpath=./
DS_LIB=-lDynamsoftCaptureVisionRouter -lDynamsoftCore -lDynamsoftUtility -lDynamsoftLicense

STDLIB=-lstdc++

TARGET=DriverLicenseScanner
OBJECT=DriverLicenseScanner.o
SOURCE=DriverLicenseScanner.cpp

# build rule for target.
$(TARGET): $(OBJECT)
	$(CC) -o $(TARGET) $(OBJECT) $(STDLIB) $(DS_LIB) $(LDFLAGS)
	cp -r $(DCPRES_PATH) $(DS_LIB_PATH)
	cp -r $(DS_JSON_PATH) $(DS_LIB_PATH)
	cp -r $(DS_MODEL_PATH) $(DS_LIB_PATH)
# target to build an object file
$(OBJECT): $(SOURCE)
	$(CC) $(CCFLAGS) $(SOURCE)

# the clean target
.PHONY : clean
clean:
	rm -f $(OBJECT) $(TARGET) -r $(DS_LIB_PATH)/ParserResources $(DS_LIB_PATH)/Templates $(DS_LIB_PATH)/Models