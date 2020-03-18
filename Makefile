######################################################################################################################################
# CONFIGURAÇÕES BÁSICAS (objetos, flags, etc.)
######################################################################################################################################

# Variáveis
SRC_DIR := src
BUILD_DIR := build
TARGET := bin/svrp
GRB_PATH := /home/eduinnarelli/opt/gurobi900 # pasta em que gurobi se encontra
INCLUDES := -I include -I $(join ${GRB_PATH}, /linux64/include)
LIBS := -L $(join ${GRB_PATH}, /linux64/lib) -l gurobi_c++ -l gurobi90

# Listar todos arquivos .cpp e relacionar com objetos .o
SOURCES :=  $(shell find $(SRC_DIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.cpp=.o))

# Flags
LINKING_FLAGS := -O3 -std=c++11 -lemon $(LIBS)
COMPILATION_FLAGS := -g -O3 -std=c++11 $(INCLUDES) 

######################################################################################################################################
# COMPILAÇÃO
######################################################################################################################################

$(TARGET): $(OBJECTS)
		@echo "Linking..."
		@echo "g++ $^ -o $(TARGET) $(LINKING_FLAGS)"; g++  $^ -o $(TARGET) $(LINKING_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
		@mkdir -p $(dir $@)
		@echo "g++ $(COMPILATION_FLAGS) -c -o $@ $<"; g++ $(COMPILATION_FLAGS) -c -o $@ $<

######################################################################################################################################

clean:
		@echo "Cleaning..."
		@echo "rm -rf $(BUILD_DIR) $(TARGET)"; rm -rf $(BUILD_DIR) $(TARGET)
