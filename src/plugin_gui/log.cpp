#include <clap/include/clap/clap.h>
#include <clap/include/clap/ext/log.h>

#include <cstdarg>
#include <cstdio>
#include "clap/include/clap/host.h"
#include "include/plugin_gui.h"
#include "log.hpp"

typedef void (*log_func_f)(const clap_host_t *host, clap_log_severity severity, const char *msg);

static void default_log_func(const clap_host_t *host, clap_log_severity severity, const char *msg) {
    switch (severity) {
        case CLAP_LOG_DEBUG:
            printf("[dbg] ");
            break;

        case CLAP_LOG_INFO:
            printf("[inf] ");
            break;

        case CLAP_LOG_WARNING:
            printf("[wrn] ");
            break;

        case CLAP_LOG_ERROR:
            printf("[err] ");
            break;

        case CLAP_LOG_FATAL:
            printf("[ftl] ");
            break;

        default:
            printf("[???] ");
            break;
    }

    printf("%s\n", msg);
}

static log_func_f active_log_func = default_log_func;
static const clap_host_t *log_host = nullptr;

void gui_set_log_func(void (*log_func)(const clap_host_t *host, clap_log_severity severity, const char *msg), const clap_host_t *host) {
    active_log_func = log_func;
    log_host = host;
}

void log_debug(const char *fmt, ...) {
    char buf[512];

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, 512, fmt, arg);
    va_end(arg);

    active_log_func(log_host, CLAP_LOG_DEBUG, buf);
}

void log_info(const char *fmt, ...) {
    char buf[512];

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, 512, fmt, arg);
    va_end(arg);

    active_log_func(log_host, CLAP_LOG_INFO, buf);
}

void log_warn(const char *fmt, ...) {
    char buf[512];

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, 512, fmt, arg);
    va_end(arg);

    active_log_func(log_host, CLAP_LOG_WARNING, buf);
}

void log_error(const char *fmt, ...) {
    char buf[512];

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buf, 512, fmt, arg);
    va_end(arg);

    active_log_func(log_host, CLAP_LOG_ERROR, buf);
}