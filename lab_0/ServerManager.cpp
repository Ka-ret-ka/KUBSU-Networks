#include <iostream>
#include <thread>
#include <vector>
#include "HTTPManager.cpp"


class ServerManager
{
private:
    // Запущен ли сервер
    bool __is_run;
    // Дескриптор серверного сокета
    int __fd;
    // Структура адреса
    sockaddr_in __addr;
    // Указатель на адрес
    sockaddr *__addr_p;
    // Длина адреса
    socklen_t __addr_len;
    // Вектор для хранения потоков
    std::vector<std::thread> __threads;
    // Поток сервера
    std::thread __server_thread;
    // 
    HTTPManager __http_manager;


    int __print_info(std::string str)
    {
        std::cout << "ServerX: " << str << std::endl;
        return 0;
    }


    int __print_error(std::string str)
    {
        std::cerr << "ServerX: " << "Error: " << str << std::endl;
        return 1;
    }


    /// @brief Инициализация сервера.
    /// @param port порт сервера.
    /// @return 0, если успешно, иначе 1.
    int __init(int port)
    {
        // Создание сокета
        // AF_INET - IPv4, SOCK_STREAM - TCP, 0 - протокол по умолчанию
        if ((this->__fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            return this->__print_error("Не удалось создать сокет");

        // Флаг для настройки сокета
        int opt = 1;
        // Настройка опций сокета
        // SO_REUSEADDR позволяет переиспользовать порт после перезапуска
        if (setsockopt(this->__fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
            return this->__print_error("Не удалось настроить сокет");

        this->__addr = sockaddr_in{
            // Настройка адреса сервера
            .sin_family = AF_INET,
            // Порт преобразуем в сетевой порядок
            .sin_port = htons(port),
            // Принимать соединения с любых интерфейсов
            .sin_addr = in_addr{
                .s_addr = INADDR_ANY,
            },
        };
        this->__addr_p = (sockaddr *)&this->__addr;
        this->__addr_len = sizeof(this->__addr);

        // Привязка сокета к адресу
        if (bind(this->__fd, this->__addr_p, this->__addr_len))
            return this->__print_error("Не удалось привязать сокет");

        // Начало прослушивания
        if (listen(this->__fd, 10))
            return this->__print_error("Не удалось прослушать");

        return 0;
    }


    /// @brief Запуск сервера - обработка входящих соединений.
    void __run()
    {
        this->__is_run = true;
        int client_socket;
        // Главный цикл сервера
        while (this->__is_run)
        {
            // accept() блокирует выполнение до тех пор, пока не придет новое соединение
            // Когда соединение приходит, возвращает новый сокет для общения с клиентом
            if ((client_socket = accept(this->__fd, this->__addr_p, &this->__addr_len)) == -1)
                if (this->__is_run)
                    this->__print_error("Не удалось принять соединение");
            // Создание потока для обработки клиента
            this->__threads.emplace_back([this](int client_socket)
                                         { this->__http_manager.handle_client(client_socket); }, client_socket);
        }
    }


public:
    ServerManager()
    {
        this->__is_run = false;
    }


    /// @brief Инициализация и запуск сервера.
    /// @param port порт сервера.
    /// @return 0, если успешно, иначе 1.
    int start(int port)
    {
        if (this->__is_run)
            return this->__print_error("Cервер уже запущен");
        else
        {
            this->__init(port);
            this->__server_thread = std::thread([this]()
                                                { this->__run(); });
        }
        return this->__print_info("Запуск сервера на порту " + std::to_string(port));
    }


    /// @brief Остановка сервера.
    /// @return 0, если успешно, иначе 1.
    int stop()
    {
        if (this->__is_run)
        {
            this->__is_run = false;
            shutdown(this->__fd, SHUT_RDWR);
            close(this->__fd);
            this->__server_thread.join();
            // Ожидаем завершения всех потоков
            for (auto &t : this->__threads)
                if (t.joinable())
                    t.join();
            return this->__print_info("Работа сервера остановлена");
        }
        return 1;
    }
};
