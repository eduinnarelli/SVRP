######################################################################################################################################
# CONFIGURAÇÕES BÁSICAS (objetos, flags, etc.)
######################################################################################################################################

OBJ_DIR = obj
SRC_DIR = src

OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/SVRP.o $(OBJ_DIR)/TabuSearchSVRP.o $(OBJ_DIR)/graph.o

BINARY_NAME = svrp
LINKING_FLAGS = -O3 -std=c++11 -lemon
COMPILATION_FLAGS = -c -O3 -std=c++11 -Iinclude

######################################################################################################################################
# COMPILAÇÃO
######################################################################################################################################

all: create_obj_dir $(OBJS)
		g++ $(OBJS) -o $(BINARY_NAME) $(LINKING_FLAGS)

create_obj_dir:
		mkdir -p $(OBJ_DIR)

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
		g++ -c $< -o $@ $(COMPILATION_FLAGS)

######################################################################################################################################
clean:
		rm -rf $(OBJ_DIR) $(BINARY_NAME)
