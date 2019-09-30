CXX         = g++
AR          = ar
OBJ_DIR     = obj
INC_DIR     = ../../include
SRC_DIR     = ../../source
TARGET_DIR  = ../../lib
INCLUDES    = -I$(INC_DIR) -I$(SRC_DIR)
SOURCES    := $(wildcard $(SRC_DIR)/ustd/*.cpp)
SOURCES    += $(wildcard $(SRC_DIR)/ustd/detail/*.cpp)

CXXFLAGS_D := -std=c++11 -static-libstdc++ -Wall -g
OBJ_DIR_D   = obj/debug
OBJECTS_D  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR_D)/%.o, $(SOURCES))
TARGET_D   := $(TARGET_DIR)/libustd_d.a

CXXFLAGS_R := -std=c++11 -static-libstdc++ -Wall
OBJ_DIR_R   = obj/release
OBJECTS_R  := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR_R)/%.o, $(SOURCES))
TARGET_R   := $(TARGET_DIR)/libustd.a

DIRS       := $(TARGET_DIR) $(OBJ_DIR) $(OBJ_DIR_D) $(OBJ_DIR_R)

.PHONY: all debug release clean

all: debug release

debug: $(TARGET_DIR) $(OBJ_DIR) $(OBJ_DIR_D) $(TARGET_D)

release: $(TARGET_DIR) $(OBJ_DIR) $(OBJ_DIR_R) $(TARGET_R)

$(OBJ_DIR_D)/%.o: $(SRC_DIR)/%.cpp
	@echo
	@echo "Compiling $< ==> $@"
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS_D) -c $< -o $@

$(TARGET_D): $(OBJECTS_D)
	@echo
	@echo "$(OBJECTS_D) ==> $@"
	$(AR) rcs $(TARGET_D) $(OBJECTS_D)

$(OBJ_DIR_R)/%.o: $(SRC_DIR)/%.cpp
	@echo
	@echo "Compiling $< ==> $@"
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) $(CXXFLAGS_R) -c $< -o $@

$(TARGET_R): $(OBJECTS_R)
	@echo
	@echo "$(OBJECTS_R) ==> $@"
	$(AR) rcs $(TARGET_R) $(OBJECTS_R)
          
$(DIRS):
	mkdir -p $@

install:
	cp $(TARGET_DIR)/*.a ../../../../lib

clean:
	rm -fr $(OBJ_DIR) $(TARGET_D) $(TARGET_R)

