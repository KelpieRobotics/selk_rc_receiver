#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "cobs.h"
#include "rc_channels.pb.h"

class Receiver {
public:
	Receiver(const std::string& ip, uint16_t port);
	void run();

private:
	int sockfd;
	std::string ip;
	int port;
	
	void initializeSocket();
	void processPacket(const uint8_t* data, size_t length);
};


