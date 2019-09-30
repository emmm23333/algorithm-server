CXX		= g++
SRC_DIR	= ../../source
LIB_DIR = ../../lib
OBJ_DIR	= ../../objs
BIN_DIR	= ../../bin
EXTERNALS_DIR	= ../../externals
THIRDPARTY_DIR	= ../../thirdparty
APP_NAME = algorithmservice

#C source file
CS      := $(wildcard $(SRC_DIR)/common/*.c)
C_SOURCES = $(filter-out $(NO_CS),$(CS))
#C++ source file
CPPS    += $(wildcard $(SRC_DIR)/common/console_app.cpp)
CPPS    += $(wildcard $(SRC_DIR)/common/system_helper.cpp)
CPPS    += $(wildcard $(SRC_DIR)/algorithmservice/*.cpp)
CPPS    += $(wildcard $(SRC_DIR)/algorithmservice/business/*.cpp)
CPPS    += $(wildcard $(SRC_DIR)/json_data/*.cpp)
SOURCES  = $(filter-out $(NO_CPPS), $(CPPS))

# RELEASE
CXXFLAGS_R = -std=c++11 -Wall -O3 -DNDEBUG
LIBS_R     = -lpthread -ldl -lrt -luuid -lustd -lopencv_imgproc -lopencv_imgcodecs -lopencv_core -llib3MF -lOpenMeshCore \
			 -lcommon -loffscreenRenderBase -lSnUtils -lceres -lalgorithm1 -lalgorithm2 -lprint3DCommon -lprint3DFilling -lprint3DQKJ \
			 -lglog -lCGAL_Core -lprint3DSlicing -lSn3DCork \
			 -lmosquittopp -lboost_serialization -lalibabacloud-oss-cpp-sdk -lcrypto -lssl -lcurl -lpthread
OBJ_DIR_R  = $(OBJ_DIR)/release
OBJECTS_R := $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR_R)%.o, $(SOURCES))  #C++ source file
OBJECTS_R += $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR_R)%.o, $(C_SOURCES))  #c source file
TARGET_R   = $(BIN_DIR)/$(APP_NAME)

INCLUDES = -I$(EXTERNALS_DIR)/ustd/include \
		   -I$(THIRDPARTY_DIR)/rapidjson/include \
		   -I$(THIRDPARTY_DIR)/aliyun-oss-cpp-sdk-master/sdk/include \
		   -I$(EXTERNALS_DIR)/SnUtils/Sn3DAlgorithm/include \
		   -I$(EXTERNALS_DIR)/SnUtils/Sn3DAlgorithm/include/Sn3DAlgorithm \
		   -I$(EXTERNALS_DIR)/SnUtils/SnUtils/include \
		   -I$(EXTERNALS_DIR)/AlgorithmThird/openmesh-4.1/include \
		   -I$(EXTERNALS_DIR)/AlgorithmThird/eigen3/include \
		   -I$(EXTERNALS_DIR)/mosquitto-master/lib \
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
		   -L$(EXTERNALS_DIR)/SnUtils/SnUtils/lib/Release \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/3mf/lib/Release \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/opencv/lib \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/CGAL-4.11.1/lib64 \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/GLOG/lib \
		   -L$(EXTERNALS_DIR)/AlgorithmThird/ceres/lib64 \
		   -L$(EXTERNALS_DIR)/mosquitto-master/build/lib \
		   -L$(EXTERNALS_DIR)/mosquitto-master/build/lib/cpp/ \
		   -L$(THIRDPARTY_DIR)/aliyun-oss-cpp-sdk-master/build/lib

.PHONY: all release config clean

all: config release
release: config $(TARGET_R)

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

clean:
	rm -f $(TARGET_R)
	rm -fr $(OBJ_DIR)

