#Makefile for iverilog target
PROJ=conquestMulti

# Change this directory to verilog
IVERILOG_DIR = /usr/local
IVERILOG_LIB = $(IVERILOG_DIR)/lib/ivl
IVERILOG_INC = $(IVERILOG_DIR)/include/iverilog


# Anaconda Python configuration
PYTHON_VERSION = 3.9
ANACONDA_PATH = /home/meng/anaconda3

# Assuming Python headers and libraries follow the usual naming convention
PYTHON_INC = -I$(ANACONDA_PATH)/include/python$(PYTHON_VERSION)
# Note: The library name might need to be adjusted based on the actual file in the Anaconda lib directory
PYTHON_LIBS = -L$(ANACONDA_PATH)/lib -lpython$(PYTHON_VERSION)

TARGET=$(IVERILOG_LIB)/$(PROJ).tgt
CONFIG_FILE=$(PROJ).conf $(PROJ)-s.conf
CONFIG_TARGET=$(IVERILOG_LIB)/$(PROJ).conf $(IVERILOG_LIB)/$(PROJ)-s.conf

OBJECTS=$(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS=$(wildcard ../include/*.h)

.PHONY: all clean

all: $(TARGET) $(CONFIG_TARGET)


clean:
	@rm -rf *.o
	@rm -rf $(TARGET)


%.o: %.cpp	$(HEADERS)
	@g++ -Wall -ggdb3 -std=c++14 -c $< -o $@ $(PYTHON_INC) -I../include/ -I$(IVERILOG_INC) -I$(IVERILOG_DIR) -fpic
	@echo "Compiling $<..."

$(TARGET): $(OBJECTS)
	@g++ $(OBJECTS) -Wall -ggdb3 -o $@ -std=c++11 -fpic -shared -lyices  $(PYTHON_LIBS) -Wl,-rpath,/home/meng/anaconda3/lib
	@echo "Linking..."

$(CONFIG_TARGET): $(CONFIG_FILE)
	@cp -f $(CONFIG_FILE) -t $(IVERILOG_LIB)
	@echo "Copying config file..."

