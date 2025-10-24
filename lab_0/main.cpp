#include "ServerManager.cpp"


int main()
{
    ServerManager server;
    std::string command;

    std::cout << "\nКоманды:"
                 "\n  start [PORT] - запустить сервер на порту PORT."
                 "\n  stop         - остановить сервер."
                 "\n  exit         - завершить менеджер." << std::endl;

    while (true)
    {
        std::cout << "\nServerX:/$ ";
        std::cin >> command;

        // Запуск сервера
        if (command == "start")
        {
            int arg;
            std::cin >> arg;
            server.start(arg);
        }
        // Остановка сервера
        else if (command == "stop")
            server.stop();
        // Выход из менеджера
        else if (command == "exit")
        {
            server.stop();
            break;
        }
    }

    return 0;
}
