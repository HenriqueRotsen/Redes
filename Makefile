CXX = gcc
CFLAGS = -Wall -c
LIBS = -lm
BIN_FOLDER = bin
INC_FOLDER = include
OBJ_FOLDER = obj
SRC_FOLDER = src
CXXFLAGS = -Wall -c -I$(INC_FOLDER)

#especificar qual versão do compilador utilizar

SERVER_OBJS = $(OBJ_FOLDER)/common.o $(OBJ_FOLDER)/server.o
CLIENT_OBJS = $(OBJ_FOLDER)/common.o $(OBJ_FOLDER)/client.o
HDRS = $(wildcard $(INC_FOLDER)/*.h)
SRCS = $(wildcard $(SRC_FOLDER)/*.c)

EXE = $(BIN_FOLDER)/server $(BIN_FOLDER)/client

all: $(EXE)

$(BIN_FOLDER)/server: $(HDRS) $(SERVER_OBJS)
	$(CXX) -g -o $(BIN_FOLDER)/server $(SERVER_OBJS) $(LIBS)

$(BIN_FOLDER)/client: $(HDRS) $(CLIENT_OBJS)
	$(CXX) -g -o $(BIN_FOLDER)/client $(CLIENT_OBJS) $(LIBS)

$(OBJ_FOLDER)/server.o: $(HDRS) $(SRCS)
	$(CXX) -g $(CXXFLAGS) -o $(OBJ_FOLDER)/server.o $(SRC_FOLDER)/server.c

$(OBJ_FOLDER)/client.o: $(HDRS) $(SRCS)
	$(CXX) -g $(CXXFLAGS) -o $(OBJ_FOLDER)/client.o $(SRC_FOLDER)/client.c

$(OBJ_FOLDER)/common.o: $(HDRS) $(SRCS)
	$(CXX) -g $(CXXFLAGS) -o $(OBJ_FOLDER)/common.o $(SRC_FOLDER)/common.c


clean:
	rm $(BIN_FOLDER)/* $(OBJ_FOLDER)/* 