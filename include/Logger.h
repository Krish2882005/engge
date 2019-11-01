#pragma once
#include <memory>
#include "spdlog/spdlog.h"
#include "Locator.h"
#undef Yield

namespace ng
{
template<typename T>
using basic_string_view_t = fmt::basic_string_view<T>;

using string_view_t = spdlog::string_view_t;

class Logger
{
  public:
    Logger();
    virtual ~Logger() = default;

    template<typename... Args>
    void trace(string_view_t message, const Args &... args);
    template<typename... Args>
    void info(string_view_t message, const Args &... args);
    template<typename... Args>
    void warn(string_view_t message, const Args &... args);
    template<typename... Args>
    void error(string_view_t message, const Args &... args);
    template<typename... Args>
    void critical(string_view_t message, const Args &... args);

private:
    std::shared_ptr<spdlog::logger> _out;
};

template<typename... Args>
void Logger::trace(string_view_t message, const Args &... args)
{
    _out->trace(message, args...);
}

template<typename... Args>
void Logger::info(string_view_t message, const Args &... args)
{
    _out->info(message, args...);
}

template<typename... Args>
void Logger::warn(string_view_t message, const Args &... args)
{
    _out->warn(message, args...);
}

template<typename... Args>
void Logger::error(string_view_t message, const Args &... args)
{
    _out->error(message, args...);
}

template<typename... Args>
void Logger::critical(string_view_t message, const Args &... args)
{
    _out->critical(message, args...);
}

template<typename... Args>
static void trace(string_view_t message, const Args &... args)
{
    Locator::getLogger().trace(message, args...);
}

template<typename... Args>
static void info(string_view_t message, const Args &... args)
{
    Locator::getLogger().info(message, args...);
}

template<typename... Args>
static void warn(string_view_t message, const Args &... args)
{
    Locator::getLogger().warn(message, args...);
}

template<typename... Args>
static void error(string_view_t message, const Args &... args)
{
    Locator::getLogger().error(message, args...);
}

template<typename... Args>
static void critical(string_view_t message, const Args &... args)
{
    Locator::getLogger().critical(message, args...);
}
} // namespace ng
