# Makefile for TaskMaster C++ Project

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g
LDFLAGS = -pthread

# Directories
SRC_DIR = src
INCL_DIR = incl
OBJ_DIR = obj
BIN_DIR = bin

# Target
TARGET = $(BIN_DIR)/taskmaster

# Source files
SRCS = $(SRC_DIR)/main_new.cpp \
       $(SRC_DIR)/ConfigParser.cpp \
       $(SRC_DIR)/Program.cpp \
       $(SRC_DIR)/ProcessManager.cpp \
       $(SRC_DIR)/SignalHandler.cpp \
       $(SRC_DIR)/TaskmasterShell.cpp \
       $(SRC_DIR)/Logger.cpp \
       $(SRC_DIR)/Utils.cpp \
       aux/parse_utils.cpp

# Object files
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Include paths
INCLUDES = -I$(INCL_DIR)

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Rules
.PHONY: all clean fclean re directories

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR)/$(SRC_DIR)
	@mkdir -p $(OBJ_DIR)/aux
	@mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	@echo "$(BLUE)Linking $(TARGET)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)✅ Build successful!$(NC)"

$(OBJ_DIR)/%.o: %.cpp
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(NC)"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "$(RED)Cleaning binary...$(NC)"
	@rm -rf $(BIN_DIR)

re: fclean all

# Debug info
debug:
	@echo "$(BLUE)=== Build Configuration ===$(NC)"
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "TARGET: $(TARGET)"
	@echo "SOURCES:"
	@echo "$(SRCS)" | tr ' ' '\n'
	@echo ""

help:
	@echo "$(BLUE)=== TaskMaster Makefile ===$(NC)"
	@echo "Available targets:"
	@echo "  $(GREEN)all$(NC)      - Build the project"
	@echo "  $(RED)clean$(NC)    - Remove object files"
	@echo "  $(RED)fclean$(NC)   - Remove object files and binary"
	@echo "  $(YELLOW)re$(NC)       - Rebuild the project"
	@echo "  $(BLUE)debug$(NC)    - Show build configuration"
	@echo "  $(BLUE)help$(NC)     - Show this help"