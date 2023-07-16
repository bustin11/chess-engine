/**
 * @file logger.hpp
 *
 * Thanks to github.com/bustub
 *
 * The lower the LOG_LEVEL, the more stuff that gets printed out
 *
 */

#pragma once

#include <ctime>
#include <string>

namespace chess {

// can't mutate the contents of the string itself
using cstr = const char *;

// internal linkage + computed at compiled time
static constexpr auto PastLastSlash(cstr a, cstr b) -> cstr {
  return *a == '\0'  ? b
         : *a == '/' ? PastLastSlash(a + 1, a + 1)
                     : PastLastSlash(a + 1, b);
}

static constexpr auto PastLastSlash(cstr a) -> cstr {
  return PastLastSlash(a, a);
}

#define __SHORT_FILE__                                                         \
  ({                                                                           \
    constexpr cstr sf__{PastLastSlash(__FILE__)};                              \
    sf__;                                                                      \
  })

// {} scopr forces the expression to be computed at compile time
#define __SHORT_FILE                                                           \
  ({                                                                           \
    constexpr cstr sf__{PastLastSlash(__FILE__)};                              \
    sf__;                                                                      \
  })


static constexpr int LOG_LEVEL_OFF = 1000;
static constexpr int LOG_LEVEL_ERROR = 500;
static constexpr int LOG_LEVEL_WARN = 400;
static constexpr int LOG_LEVEL_INFO = 300;
static constexpr int LOG_LEVEL_DEBUG = 200;
static constexpr int LOG_LEVEL_TRACE = 100;
static constexpr int LOG_LEVEL_ALL = 0;

#define LOG_LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define LOG_OUTPUT_STREAM stdout

#define LOG_LEVEL 450
#ifndef LOG_LEVEL
#ifndef NDEBUG
static constexpr int LOG_LEVEL = LOG_LEVEL_DEBUG;
#else
static constexpr int LOG_LEVEL = LOG_LEVEL_INFO;
#endif
#endif

// some compilers don't support __FUNCTION__
#if !defined(__FUNCTION__) && !defined(__GNUC__)
#define __FUNCTION__ ""
#endif

void OutputLogHeader(cstr file, int line, cstr func, int level);

// :: means global namespace

#ifdef LOG_ERROR_ENABLED
#undef LOG_ERROR_ENABLED
#endif
#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR_ENABLED
#define LOG_ERROR(...)                                                         \
  OutputLogHeader(__SHORT_FILE, __LINE__, __FUNCTION__, LOG_LEVEL_ERROR);      \
  ::fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__);                                   \
  fprintf(LOG_OUTPUT_STREAM, "\n");                                            \
  ::fflush(stdout)
#else
// no-op
#define LOG_ERROR(...) ((void)0)
#endif

#ifdef LOG_WARN_ENABLED
#undef LOG_WARN_ENABLED
#endif
#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN_ENABLED
#define LOG_WARN(...)                                                          \
  OutputLogHeader(__SHORT_FILE, __LINE__, __FUNCTION__, LOG_LEVEL_WARN);       \
  ::fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__);                                   \
  fprintf(LOG_OUTPUT_STREAM, "\n");                                            \
  ::fflush(stdout)
#else
// no-op
#define LOG_WARN(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO_ENABLED
#define LOG_INFO(...)                                                          \
  OutputLogHeader(__SHORT_FILE, __LINE__, __FUNCTION__, LOG_LEVEL_INFO);       \
  ::fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__);                                   \
  fprintf(LOG_OUTPUT_STREAM, "\n");                                            \
  ::fflush(stdout)
#else
// no-op
#define LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG_ENABLED
#define LOG_DEBUG(...)                                                         \
  OutputLogHeader(__SHORT_FILE, __LINE__, __FUNCTION__, LOG_LEVEL_DEBUG);      \
  ::fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__);                                   \
  fprintf(LOG_OUTPUT_STREAM, "\n");                                            \
  ::fflush(stdout)
#else
// no-op
#define LOG_DEBUG(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE_ENABLED
#define LOG_TRACE(...)                                                         \
  OutputLogHeader(__SHORT_FILE, __LINE__, __FUNCTION__, LOG_LEVEL_TRACE);      \
  ::fprintf(LOG_OUTPUT_STREAM, __VA_ARGS__);                                   \
  fprintf(LOG_OUTPUT_STREAM, "\n");                                            \
  ::fflush(stdout)
#else
// no-op
#define LOG_TRACE(...) ((void)0)
#endif

// example [INFO] [file_name:line_number:func()] 2023:7:02 10:57:00
inline void OutputLogHeader(cstr file, int line, cstr func, int level) {
  time_t t = ::time(nullptr);
  tm *curTime = localtime(&t); // NOLINT
  char time_str[32];
  ::strftime(time_str, 32, LOG_LOG_TIME_FORMAT, curTime);
  cstr type;
  switch (level) {
  case LOG_LEVEL_ERROR:
    type = "ERROR";
    break;
  case LOG_LEVEL_WARN:
    type = "WARN ";
    break;
  case LOG_LEVEL_INFO:
    type = "INFO ";
    break;
  case LOG_LEVEL_DEBUG:
    type = "DEBUG";
    break;
  case LOG_LEVEL_TRACE:
    type = "TRACE";
    break;
  default:
    type = "UNKWN";
  }
  ::fprintf(LOG_OUTPUT_STREAM, "%s [%s:%d:%s] %s - ", time_str, file, line,
            func, type);
}

} // namespace chess
