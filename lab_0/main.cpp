#include "ServerManager.cpp"


int main()
{
    ServerManager server;
    std::string command;

    std::string help = "Команды:"
                       "\n  help         - вывести помощь."
                       "\n  start [PORT] - запустить сервер на порту PORT."
                       "\n  stop         - остановить сервер."
                       "\n  exit         - выйти из терминала.";

    std::cout << "\nНачало работы терминала" << std::endl;

    while (true)
    {
        std::cout << "\n\033[1;32m" << "admin@ServerX" << "\033[0m" << ":" << "\033[1;34m" << "/" << "\033[0m" << "$ ";
        std::cin >> command;

        if (command == "start")
        {
            int arg;
            std::cin >> arg;
            server.start(arg);
        }
        else if (command == "stop")
        {
            server.stop();
        }
        else if (command == "exit")
        {
            server.stop();
            std::cout << "\nЗавершение работы терминала" << std::endl;
            break;
        }
        else if (command == "help")
        {
            std::cout << help << std::endl;
        }
        else
        {
            std::cout << command << ": команда не найдена" << std::endl;
        }
    }

    return 0;
}
