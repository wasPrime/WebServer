#pragma once

#include <iostream>
#include <stdexcept>

enum class ExceptionType {
    INVALID,
    INVALID_SOCKET,
};

class Exception : public std::runtime_error {
public:
    Exception(const std::string& message)
        : std::runtime_error(message), m_type(ExceptionType::INVALID) {
        std::cerr << "Message :: " + message << std::endl;
    }

    Exception(ExceptionType type, const std::string& message)
        : std::runtime_error(message), m_type(type) {
        std::cerr << "Exception Type :: " + exception_type_to_string(m_type) << std::endl
                  << "Message :: " << message << std::endl;
    }

private:
    static std::string exception_type_to_string(ExceptionType type) {
        switch (type) {
            case ExceptionType::INVALID:
                return "Invalid";
            case ExceptionType::INVALID_SOCKET:
                return "Invalid socket";
            default:
                return "Unknoen";
        }
    }

private:
    ExceptionType m_type;
};
