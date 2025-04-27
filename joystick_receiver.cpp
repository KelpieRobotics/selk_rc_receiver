#include "joystick_receiver.h"

Receiver::Receiver(const std::string& ip, uint16_t port) : ip(ip), port(port) {
    initializeSocket();
}

void Receiver::initializeSocket()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    std::cout << "Socket created" << std::endl;
    if(sockfd < 0)
    {
        std::cerr << "Failed to create a socket. " << std::endl;
        std::exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    std::cout << "Socket binded" << std::endl;

    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        std::exit(1);
    }
}


void Receiver::run()
{
    uint8_t buffer[1024];
    sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    while(true)
    {
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&senderAddr, &senderLen);
        if(len > 0)
        {
            processPacket(buffer, static_cast<size_t>(len));
        }
    }
}


void Receiver::processPacket(const uint8_t* data, size_t length)
{
    std::vector<uint8_t> encoded(data, data + length);
   
    if(!encoded.empty() && encoded.back() == 0x00)
    {
        encoded.pop_back();
    }

    try {
        std::vector<uint8_t> decoded(encoded.size());
        
        cobs_decode_result result = cobs_decode(decoded.data(), decoded.size(), encoded.data(), encoded.size());
        decoded.resize(result.out_len);
        
        // debugging using raw hex data
        std::cout << "Raw decoded data: ";
        for(size_t i = 0; i < decoded.size(); ++i) {
            std::cout << std::hex << (int)decoded[i] << " ";
        }  
        std::cout << std::dec << std::endl;
        // rc channels parsing
        rc_interface::RCChannels msg;
        
        if(msg.ParseFromArray(decoded.data(), static_cast<int>(decoded.size())))
        {
            std::cout << "rc1 = " << msg.rc1() << std::endl;
            std::cout << "rc2 = " << msg.rc2() << std::endl;
        } else {
            std::cerr << "Failed to parse protobuf message." << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "COBS decode failed: " << e.what() << std::endl;
        return;
    }
}


int main()
{
    Receiver receiver("127.0.0.1", 5005);
    receiver.run();

    return 0;
}