#include "save_stream_data_local.h"
#include "timer.h"

void func2(int x)
{
 std::cout << "trigger func2, x: " << x << std::endl;
}

int main(int argc, char const *argv[])
{
    Save_stream_data_local demo;

    demo.saveFileInit();

    std::this_thread::sleep_for(std::chrono::milliseconds(50000));

    demo.saveFileDinit();

    // Timer timer;
    // timer.startOnce(1000, std::bind(func2, 3));
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    // timer.stop();

    
    return 0;
}
