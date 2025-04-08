#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class FileHeaderException : public std::exception {
public:
    explicit FileHeaderException(const char* message) : msg_(message) {
    }

    const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class InfoHeaderException : public std::exception {
public:
    explicit InfoHeaderException(const char* message) : msg_(message) {
    }

    const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class FilterException : public std::exception {
public:
    explicit FilterException(const char* message) : msg_(message) {
    }

    const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class InputDataException : public std::exception {
public:
    explicit InputDataException(const char* message) : msg_(message) {
    }

    const char* what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

#endif  // EXCEPTIONS_H
