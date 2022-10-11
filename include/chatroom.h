#ifndef INCLUDE_CHATROOM_H
#define INCLUDE_CHATROOM_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <csignal>

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

const int MAX_LEN = 200;
const int NUM_COLORS = 6;

const std::string def_col = "\033[0m";
const std::string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};

std::string color(int code);

#endif
