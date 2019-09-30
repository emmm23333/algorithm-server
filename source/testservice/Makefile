CXX		= g++
SRC_DIR	= ../../source
LIB_DIR = ../../lib
OBJ_DIR	= ../../objs
BIN_DIR	= ../../bin
EXTERNALS_DIR	= ../../externals
THIRDPARTY_DIR	= ../../thirdparty
APP_NAME = testservice

#C source file
CS      := $(wildcard $(SRC_DIR)/common/*.c)
C_SOURCES = $(filter-out $(NO_CS),$(CS))
#C++ source file
CPPS    += $(wildcard $(SRC_DIR)/testservice/*.cpp)
SOURCES  = $(filter-out $(NO_CPPS), $(CPPS))

# DEBUG
CXXFLAGS_D = -std=c++11 -Wall -g3 -rdynamic -D_DEBUG
LIBS_D     = -lpthread -ldl -lrt -lustd -lcommon -lmosquittopp
OBJ_DIR_D  = $(OBJ_DIR)/debug
OBJECTS_D := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR_D)%.o, $(SOURCES))  #C++ source file
OBJECTS_D += $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR_D)%.o, $(C_SOURCES))  #c source file

TARGET_D   = $(BIN_DIR)/$(APP_NAME)_d

# RELEASE
CXXFLAGS_R = -std=c++11 -Wall -O3 -DNDEBUG
LIBS_R     = -lpthread -ldl -lrt -lustd -lcommon -lmosquittopp
OBJ_DIR_R  = $(OBJ_DIR)/release
OBJECTS_R := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR_R)%.o, $(SOURCES))  #C++ source file
OBJECTS_R += $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR_R)%.o, $(C_SOURCES))  #c source file
TARGET_R   = $(BIN_DIR)/$(APP_NAME)

INCLUDES = -I$(EXTERNALS_DIR)/ustd/include \
		   -I$(EXTERNALS_DIR)/SnUtils/Sn3DAlgorithm/include \
		   -I$(EXTERNALS_DIR)/AlgorithmThird/openmesh-4.1/include \
		   -I$(EXTERNALS_DIR)/AlgorithmThird/eigen3/include \
		   -I$(EXTERNALS_DIR)/mosquitto-master/lib/cpp \
		   -I$(SRC_DIR)/algorithmservice \
		   -I$(SRC_DIR)

LDFLAGS  = -Wl,-rpath=./,-rpath-link=./ \
		   -no-pie \
		   -static-libgcc \
		   -static-libstdc++ \
		   -L$(LIB_DIR) \
		   -L$(EXTERNALS_DIR)/ustd/lib \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/openmesh-4.1/lib/Linux/gcc8/x64/Release \
		   -L$(EXTERNALS_DIR)/SnUtils/Sn3DAlgorithm/lib/Linux/gcc8/x64/Release \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/3mf/lib/Release \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/opencv/lib

.PHONY: all release debug config clean

all: config release debug
release: config $(TARGET_R)
debug: config $(TARGET_D)

config:

$(OBJ_DIR_R)/%.o: $(SRC_DIR)/%.c*
	@echo
	@echo "Compiling $< ==> $@"
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS_R) -c $? -o $@

$(TARGET_R): $(OBJECTS_R)
	@echo
	@echo "Linking $(OBJECTS_R) ==> $@"
	$(CXX) $(OBJECTS_R) $(LDFLAGS) $(LIBS_R) -o $@

$(OBJ_DIR_D)/%.o: $(SRC_DIR)/%.c*
	@echo
	@echo "Compiling $< ==> $@"
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS_D) -c $? -o $@

$(TARGET_D): $(OBJECTS_D)
	@echo
	@echo "Linking $(OBJECTS_D) ==> $@"
	$(CXX) $(OBJECTS_D) $(LDFLAGS) $(LIBS_D) -o $@

clean:
	rm -f $(TARGET_R) $(TARGET_D)
	rm -fr $(OBJ_DIR)

