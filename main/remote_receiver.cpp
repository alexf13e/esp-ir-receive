
#include <iostream>

#include "app-window.h"

#include "screen_setup.h"
#include "ir_receiver.h"


extern "C" void app_main(void)
{
    screenSetup();
    IRReceiver::init();

    auto main_window = AppWindow::create();

    main_window->global<Logic>().on_receive_ready([]() {
        std::cout << "Ready to receive" << std::endl;
        while (true)
        {
            IRReceiver::receive();
        }
        std::cout << "Receiving ended" << std::endl;
    });

    main_window->run();   
}