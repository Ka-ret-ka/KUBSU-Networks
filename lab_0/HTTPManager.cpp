#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
#include "UserManager.cpp"


/// @brief Класс для управления HTTP-запросами.
class HTTPManager
{
private:
    // 
    UserManager __user_manager;


    /// @brief Функция для парсинга POST данных.
    /// @param data сырые данные.
    /// @return Словарь.
    std::unordered_map<std::string, std::string> __parse_post_data(const std::string &data)
    {
        std::unordered_map<std::string, std::string> result;
        std::istringstream stream(data);
        std::string pair;

        while (std::getline(stream, pair, '&'))
        {
            size_t pos = pair.find('=');
            if (pos != std::string::npos)
                result[pair.substr(0, pos)] = pair.substr(pos + 1);
        }
        return result;
    }


    /// @brief Функция для чтения текста из файла.
    /// @param filename имя файла.
    /// @param text_file ссылка на содержимое файла.
    /// @return 0 если успешно, -1 если ошибка.
    int __read_html_file(const std::string &filename, std::string &text_file)
    {
        std::ifstream file(filename);
        if (!file.is_open())
            return -1;
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        text_file = buffer.str();
        return 0;
    }


    /// @brief Формирование пакета для загрузки страницы.
    /// @param content содержимое страницы.
    /// @return Пакет с содержимым страницы.
    std::string __generate_response(const std::string &content)
    {
        return "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Connection: close\r\n"
            "\r\n" + content;
    }


    /// @brief Формирование пакета для перенаправление на другой адрес.
    /// @param location адрес новой страницы.
    /// @return Пакет с новым адресом.
    std::string __generate_redirect(const std::string &location)
    {
        return "HTTP/1.1 303 See Other\r\n"
            "Location: " + location + "\r\n"
            "Connection: close\r\n"
            "\r\n";
    }


    /// @brief Отправление ответа - содержимого страницы.
    /// @param client_socket сокет.
    /// @param html_name имя html-файла, без расширения.
    void __send_response(int client_socket, const std::string &html_name)
    {
        std::string html;
        if (this->__read_html_file("./templates" + html_name + ".html", html))
            this->__read_html_file("./templates/error_not_found.html", html);
        std::string response = this->__generate_response(html);
        send(client_socket, response.c_str(), response.size(), 0);
    }


    /// @brief Отправление ответа - перенаправление на другой адрес.
    /// @param client_socket сокет.
    /// @param html_name имя html-файла, без расширения.
    void __send_redirect(int client_socket, const std::string &html_name)
    {
        std::string response = this->__generate_redirect(html_name);
        send(client_socket, response.c_str(), response.size(), 0);
    }


public:
    /// @brief Обработка соединения.
    /// @param client_socket сокет.
    void handle_client(int client_socket)
    {
        char buffer[4096];
        int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            std::string request(buffer);

            // Определяем метод и путь
            std::istringstream request_stream(request);
            std::string method, path;
            request_stream >> method >> path;

            // Обработка GET запросов
            if (method == "GET")
            {
                // При переходе на сайт, пользователя перекидывает на начальную страницу
                if (path == "/")
                    path += "index";
                // Отправить пользователя на нужную страницу
                this->__send_response(client_socket, path);
            }

            // Обработка POST запросов
            else if (method == "POST")
            {
                // Извлекаем тело запроса
                size_t content_pos = request.find("\r\n\r\n");
                if (content_pos != std::string::npos)
                {
                    // Данные запроса
                    std::unordered_map<std::string, std::string> post_data = this->__parse_post_data(request.substr(content_pos + 4));
                    // Если запрос - регистрационные данные
                    if (path == "/register")
                    {
                        // В случае успешной регистрации - перенаправление на страницу авторизации
                        if (this->__user_manager.register_user(post_data["login"], post_data["password"]))
                            this->__send_redirect(client_socket, "/login");
                        else
                            this->__send_response(client_socket, "/error_reg_fail");
                    }
                    // Если запрос - авторизационные данные
                    else if (path == "/login")
                    {
                        // В случае успешной авторизации - перенаправление на страницу личного кабинета
                        if (this->__user_manager.authenticate(post_data["login"], post_data["password"]))
                            this->__send_redirect(client_socket, "/dashboard");
                        else
                            this->__send_response(client_socket, "/error_incorr_cred");
                    }
                }
            }
        }
        close(client_socket);
    }
};
