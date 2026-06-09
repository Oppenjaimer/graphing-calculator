

# =================================================================

SRC_DIR = src
INC_DIR = include
EXT_DIR = external
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/plot

IMGUI_DIR = $(EXT_DIR)/imgui
RLIMGUI_DIR = $(EXT_DIR)/rlimgui
TINYEXPR_DIR = $(EXT_DIR)/tinyexpr

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -I$(INC_DIR) -I$(IMGUI_DIR) -I$(RLIMGUI_DIR) -I$(TINYEXPR_DIR)
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

EXT_SRC_FILES = $(wildcard $(IMGUI_DIR)/*.cpp) \
                $(wildcard $(RLIMGUI_DIR)/*.cpp) \
                $(wildcard $(TINYEXPR_DIR)/*.cpp)
EXT_OBJ_FILES = $(patsubst $(EXT_DIR)/%.cpp, $(BUILD_DIR)/$(EXT_DIR)/%.o, $(EXT_SRC_FILES))

ALL_OBJ_FILES = $(OBJ_FILES) $(EXT_OBJ_FILES)

all: $(TARGET)

$(TARGET): $(ALL_OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(ALL_OBJ_FILES) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/$(EXT_DIR)/%.o: $(EXT_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
