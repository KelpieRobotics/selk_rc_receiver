#include "joystick_receiver.h"

int main()
{
    Receiver receiver("127.0.0.1", 5005, "/dev/ttyAMA1");
    receiver.run();

    return 0;
}
