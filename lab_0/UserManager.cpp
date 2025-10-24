#include <unordered_map>
#include <fstream>


/// @brief Класс для управления пользователями.
class UserManager
{
private:
    // Контейнер, хранящий зарегистрированных пользователей login:password
    std::unordered_map<std::string, std::string> __users;
    // Файл с данными пользователей
    std::string __data_file;


    /// @brief Загрузить данные пользователей с файла.
    void __load_users()
    {
        std::ifstream file(this->__data_file);
        std::string line;
        while (std::getline(file, line))
        {
            size_t pos = line.find(':');
            if (pos != std::string::npos)
            {
                std::string login = line.substr(0, pos);
                std::string password = line.substr(pos + 1);
                this->__users[login] = password;
            }
        }
        file.close();
    }


    /// @brief Сохраннить данные пользователей в файл.
    void __save_users()
    {
        std::ofstream file(this->__data_file);
        for (const auto &user : this->__users)
        {
            file << user.first << ":" << user.second << "\n";
        }
        file.close();
    }


public:
    UserManager()
    {
        this->__data_file = "users.txt";
        this->__load_users();
    }


    /// @brief Регистрация нового пользователя.
    /// @param login логин пользователя.
    /// @param password пароль пользователя.
    /// @return true, если успешно, иначе false.
    bool register_user(const std::string &login, const std::string &password)
    {
        // Проверка на существование пользователя
        if (this->__users.find(login) != this->__users.end())
        {
            return false;
        }
        this->__users[login] = password;
        this->__save_users();
        return true;
    }

    /// @brief Аутентификация пользователя.
    /// @param login логин пользователя.
    /// @param password пароль пользователя.
    /// @return true, если усрпешно, иначе false.
    bool authenticate(const std::string &login, const std::string &password)
    {
        auto it = this->__users.find(login);
        return it != this->__users.end() && it->second == password;
    }


    /// @brief Проверка на существование пользователя.
    /// @param login логин пользователя.
    /// @return true, если существует, иначе false.
    bool user_exists(const std::string &login)
    {
        return this->__users.find(login) != this->__users.end();
    }
};
