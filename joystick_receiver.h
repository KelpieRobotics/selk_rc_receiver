#pragma once

#include <iostream>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include "selk_rc_msgs/build/cpp/rc_channels.pb.h"
#include "rcdrivers/SBUS.h"

class Receiver {
public:
	Receiver(const std::string& ip, uint16_t port, const char* dev);
	void run();

private:
	int sockfd;
	std::string ip;
	int port;
	const char* dev;
	SBUS sbus;
	std::chrono::time_point<std::chrono::steady_clock> last_write;

	void initializeSocket();
	void initializeSBUS();
	void processPacket(const uint8_t* data, size_t length);
};


