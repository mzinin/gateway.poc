#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

void foo(int i)
{
    if (i == 5)
        throw std::runtime_error("Bad number");
    std::cout << "Number " << i << std::endl;
}

void worker(int id)
{
    for (int i = 0; i < 10; ++i)
    {
        try
        {
            std::cout << "Worker " << id << ": ";
            foo(i);
        }
        catch (const std::exception&)
        {
            std::cout << "exception\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main()
{
    std::cout << "Hello, gateway!" << std::endl;

    std::thread t1{worker, 1};
    std::thread t2{worker, 2};

    t1.join();
    t2.join();

    return 0;
}
