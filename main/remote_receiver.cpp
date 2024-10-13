
#include <stdio.h>

#include "ir_receiver.h"

extern "C" void app_main(void)
{
    IRReceiver::init();

    while (true)
    {
        IRReceiver::receive();
    }
}
