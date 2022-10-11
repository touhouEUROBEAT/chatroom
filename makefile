CC = g++
CFLAGS = -lpthread -Wall
INCLUDES = -I ./
SRC_PATH = ./src
TARGET_PATH = ./bin


default: server client

server:
	time $(CC) $(CFLAGS) $(INCLUDES) $(SRC_PATH)/server.cpp $(SRC_PATH)/chatroom.cpp \
		-o ${TARGET_PATH}/server

client:
	time $(CC) $(CFLAGS) $(INCLUDES) $(SRC_PATH)/client.cpp $(SRC_PATH)/chatroom.cpp \
		-o ${TARGET_PATH}/client

clean:
	$(RM) ${TARGET_PATH}/server ${TARGET_PATH}/client
