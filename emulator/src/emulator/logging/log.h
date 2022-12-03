#pragma once

#ifndef LOG_RECORD_MSG_MAX_SIZE
#define LOG_RECORD_MSG_MAX_SIZE 1024
#endif

enum log_outcome {
    LOG_OK = 0,
    LOG_ERR,
    LOG_ERR_IO,
    LOG_ERR_OVERFLOW,
};

struct log_record {
    FILE* stream;
    const char* level;
    const char* tag;
    const char* file;
    int line;
    const char* caused_by;
    const char* fmt;
};

#define log_io_info(format, ...) \
    log_buffered( \
        (struct log_record) { \
            .stream = stderr, \
            .level = "info", \
            .tag = "io", \
            .file = NULL, \
            .line = -1, \
            .caused_by = NULL, \
            .fmt = format \
        }, \
        __VA_ARGS__ \
    )

#define log_io_error(format, ...) \
    log_buffered( \
        (struct log_record) { \
            .stream = stderr, \
            .level = "error", \
            .tag = "io", \
            .file = __FILE__, \
            .line = __LINE__, \
            .caused_by = strerror(errno), \
            .fmt = format \
        }, \
        __VA_ARGS__ \
    )

/**
 * Simple log function that does buffering writes to a internal stack buffer before flush everything to the stream with just one I/O call.
 * Works best for unbuffered streams like stderr.
 * For buffered streams like stdout, please check the log_unbuffered function.
 */
// TODO add color support
enum log_outcome log_buffered(struct log_record record, ...)
{
    int rc, offset = 0;
    char buf[LOG_RECORD_MSG_MAX_SIZE] = {0};

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    // log record timestamp, level, file and line number
    // TODO improve timestamping by printing milisseconds probably with clock_gettime()
    if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
    rc = snprintf(
        buf + offset,
        LOG_RECORD_MSG_MAX_SIZE - offset,
        "[%04d-%02d-%02d %02d:%02d:%02d] ",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec
    );
    if (rc < 0) return LOG_ERR_IO;
    if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
    offset += rc;

    if (record.file != NULL && record.line >= 0) {
        if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
        rc = snprintf(
            buf + offset,
            LOG_RECORD_MSG_MAX_SIZE - offset,
            "[%s:%d] ",
            record.file,
            record.line
        );
        if (rc < 0) return LOG_ERR_IO;
        if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
        offset += rc;
    }

    if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
    rc = snprintf(
        buf + offset,
        LOG_RECORD_MSG_MAX_SIZE - offset,
        "%s: %s: ",
        record.level,
        record.tag
    );
    if (rc < 0) return LOG_ERR_IO;
    if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
    offset += rc;

    // log record msg
    if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
    va_list args;
    va_start(args, record);
    rc = vsnprintf(buf + offset, LOG_RECORD_MSG_MAX_SIZE - offset, record.fmt, args);
    va_end(args);
    if (rc < 0) return LOG_ERR_IO;
    if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
    offset += rc;

    // caused by msg
    if (record.caused_by) {
        if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
        rc = snprintf(buf + offset, LOG_RECORD_MSG_MAX_SIZE - offset, ": %s\n", record.caused_by);
        if (rc < 0) return LOG_ERR_IO;
        if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
        offset += rc;
    } else {
        if (offset >= LOG_RECORD_MSG_MAX_SIZE) return LOG_ERR_OVERFLOW;
        rc = snprintf(buf + offset, LOG_RECORD_MSG_MAX_SIZE - offset, "\n");
        if (rc < 0) return LOG_ERR_IO;
        if (rc >= LOG_RECORD_MSG_MAX_SIZE - offset) return LOG_ERR_OVERFLOW;
        offset += rc;
    }

    // flush to stream
    if (fputs(buf, record.stream) == EOF) {
        return LOG_ERR_IO;
    }

    return LOG_OK;
}
