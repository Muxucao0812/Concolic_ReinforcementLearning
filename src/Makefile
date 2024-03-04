# Makefile for iverilog target with Anaconda Python integration

# Project name
PROJ = conquestMulti

# Icarus Verilog configuration
IVERILOG_DIR = /usr/local
IVERILOG_LIB_DIR = $(IVERILOG_DIR)/lib/ivl
IVERILOG_INC_DIR = $(IVERILOG_DIR)/include/iverilog

# System Python configuration
PYTHON_VERSION = 3.8
CONDA_DIR = /home/meng/miniconda3
PYTHON_INC = $(CONDA_DIR)/envs/concolic/include/python$(PYTHON_VERSION)
PYTHON_LIBS = $(CONDA_DIR)/envs/concolic/lib  -lpython$(PYTHON_VERSION)


# Target configuration
TARGET = $(IVERILOG_LIB_DIR)/$(PROJ).tgt
CONFIG_FILES = $(PROJ).conf $(PROJ)-s.conf
CONFIG_TARGETS = $(addprefix $(IVERILOG_LIB_DIR)/,$(CONFIG_FILES))

# Source and header files
CPP_SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard ../include/*.h)
OBJECTS = $(CPP_SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET) $(CONFIG_TARGETS)

clean:
	@rm -rf $(OBJECTS) $(TARGET)

# Compilation of source files
%.o: %.cpp $(HEADERS)
	@g++ -Wall -ggdb3 -std=c++14 -c $< -o $@ -I../include -I$(IVERILOG_INC_DIR) -I$(PYTHON_INC) -L$(PYTHON_LIBS)  -fpic -L/home/meng/miniconda3/envs/concolic/lib/libpython3.8.so
	@echo /home/meng/miniconda3/envs/concolic/lib/libpython3.8.so
	@echo "Compiling $<..."

# Linking to create the target
$(TARGET): $(OBJECTS)
	@g++ -Wall -ggdb3 -std=c++11 -o $@ $(OBJECTS) -fpic -shared -lyices -I$(PYTHON_INC) -L$(PYTHON_LIBS) -Wl,-rpath,/home/meng/miniconda3/envs/concolic/lib --enable-shared
	@echo "Linking..."

# Copying configuration files
$(CONFIG_TARGETS): $(CONFIG_FILES)
	@cp -f $(CONFIG_FILES) $(IVERILOG_LIB_DIR)
	@echo "Copying config file..."
