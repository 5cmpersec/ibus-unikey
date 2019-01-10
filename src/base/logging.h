#pragma once

#include <atomic>
#include <string>
#include <memory>

#define SPDLOG_CLOCK_COARSE
#define SPDLOG_NO_NAME
#define SPDLOG_LEVEL_NAMES  { "TRC", "DBG", "INF", "WRN", "ERR", "CRT", "OFF" }

#ifndef NDEBUG
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON
#endif // NDEBUG


#include <spdlog/spdlog.h>

#include "base/port.h"

namespace unikey {

class Logger {
private:
    static Logger& default_instance() {
        static Logger logger;
        return logger;
    }

    Logger() {
        set_logger(spdlog::stderr_logger_mt("stderr"));
    }

    void init() {
        m_logger->set_level(spdlog::level::trace);
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S][%l]: %v");
    }

    std::shared_ptr<spdlog::logger> get_logger() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_logger;
    }

    void set_logger(std::shared_ptr<spdlog::logger> new_logger) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_logger = new_logger;
        init();
    }

public:
    static std::shared_ptr<spdlog::logger> get_default_logger() {
        return default_instance().get_logger();
    }

    static void set_default_logger(const std::string& name, const std::string& file) {
        auto new_logger = spdlog::rotating_logger_mt(name, file, 1048576 * 5, 1);
        default_instance().set_logger(new_logger);
    }

private:
    std::mutex m_mutex;
    std::shared_ptr<spdlog::logger> m_logger;

private:
    DISALLOW_COPY_AND_ASSIGN(Logger);
};

} // namespace unikey

#ifndef NDEBUG
#define BLOG_TRACE(...) SPDLOG_TRACE(unikey::Logger::get_default_logger(), __VA_ARGS__)
#define BLOG_DEBUG(...) unikey::Logger::get_default_logger()->debug(__VA_ARGS__)
#else
#define BLOG_TRACE(...)
#define BLOG_DEBUG(...)
#endif // NDEBUG

#define BLOG_INFO(...) unikey::Logger::get_default_logger()->info(__VA_ARGS__)
#define BLOG_WARNING(...) unikey::Logger::get_default_logger()->warn(__VA_ARGS__)
#define BLOG_ERROR(...) unikey::Logger::get_default_logger()->error(__VA_ARGS__)
#define BLOG_CRITICAL(...) unikey::Logger::get_default_logger()->critical(__VA_ARGS__)

#define BLOG_SET_OUTPUT_FILE(file) unikey::Logger::set_default_logger(file, file)