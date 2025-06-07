#include "joystick_receiver.h"

int main()
{
    Receiver receiver("192.168.137.20", 5005, "/dev/ttyS8");
    receiver.run();

    return 0;
}
