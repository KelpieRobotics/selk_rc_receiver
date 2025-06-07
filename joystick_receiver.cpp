#include "joystick_receiver.h"

Receiver::Receiver(const std::string& ip, uint16_t port, const char* dev) : ip(ip), port(port), sbus(), dev(dev), last_write(std::chrono::steady_clock::now()) {
    initializeSocket();
    initializeSBUS();
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

void Receiver::initializeSBUS()
{
    rcdrivers_err_t err = sbus.install(dev, false);  // false for non-blocking
    if (err != RCDRIVERS_OK)
    {
        std::cerr << "SBUS install error: " << err << std::endl;
        return;
    }

    std::cout << "SBUS installed" << std::endl;
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


void print_msg(kelpie_rc_msgs::RCChannels& msg) {
    std::cout << "\t" << "rc1 = " << msg.rc1() << std::endl;
    std::cout << "\t" << "rc2 = " << msg.rc2() << std::endl;
    std::cout << "\t" << "rc3 = " << msg.rc3() << std::endl;
    std::cout << "\t" << "rc4 = " << msg.rc4() << std::endl;
    std::cout << "\t" << "rc5 = " << msg.rc5() << std::endl;
    std::cout << "\t" << "rc6 = " << msg.rc6() << std::endl;
    std::cout << "\t" << "rc7 = " << msg.rc7() << std::endl;
    std::cout << "\t" << "rc8 = " << msg.rc8() << std::endl;
    std::cout << "\t" << "rc9 = " << msg.rc9() << std::endl;
    std::cout << "\t" << "rc10 = " << msg.rc10() << std::endl;
    std::cout << "\t" << "rc11 = " << msg.rc11() << std::endl;
    std::cout << "\t" << "rc12 = " << msg.rc12() << std::endl;
    std::cout << "\t" << "rc13 = " << msg.rc13() << std::endl;
    std::cout << "\t" << "rc14 = " << msg.rc14() << std::endl;
    std::cout << "\t" << "rc15 = " << msg.rc15() << std::endl;
    std::cout << "\t" << "rc16 = " << msg.rc16() << std::endl;
    std::cout << "---" << std::endl;
}

void Receiver::processPacket(const uint8_t* data, size_t length)
{
        // debugging using raw hex data
        std::cout << "Raw decoded data: ";
        for(size_t i = 0; i < length; ++i) {
            std::cout << std::hex << (int)data[i] << " ";
        }
        std::cout << std::dec << std::endl;
        // rc channels parsing
        kelpie_rc_msgs::RCChannels msg;

        if(msg.ParseFromArray(data, static_cast<int>(length)))
        {
            // print_msg(msg);
            sbus_packet_t packet = {
                .channels = {
                    static_cast<uint16_t>(msg.rc1()),
                    static_cast<uint16_t>(msg.rc2()),
                    static_cast<uint16_t>(msg.rc3()),
                    static_cast<uint16_t>(msg.rc4()),
                    static_cast<uint16_t>(msg.rc5()),
                    static_cast<uint16_t>(msg.rc6()),
                    static_cast<uint16_t>(msg.rc7()),
                    static_cast<uint16_t>(msg.rc8()),
                    static_cast<uint16_t>(msg.rc9()),
                    static_cast<uint16_t>(msg.rc10()),
                    static_cast<uint16_t>(msg.rc11()),
                    static_cast<uint16_t>(msg.rc12()),
                    static_cast<uint16_t>(msg.rc13()),
                    static_cast<uint16_t>(msg.rc14()),
                    static_cast<uint16_t>(msg.rc15()),
                    static_cast<uint16_t>(msg.rc16()),
                },
                .ch17 = false,
                .ch18 = false,
                .failsafe = false, // TODO: Change?
                .frameLost = false,
            };
            auto now = std::chrono::steady_clock::now();
            if(now - last_write > std::chrono::milliseconds(20)) {
                sbus.write(packet);
                last_write = now;
            }
            else {
                std::cout << "Sending too fast, throttling..." << std::endl;
            }
        } else {
            std::cerr << "Failed to parse protobuf message." << std::endl;
        }
}
