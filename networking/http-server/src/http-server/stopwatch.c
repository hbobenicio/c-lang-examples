#include "stopwatch.h"

#include <assert.h>

static void assert_clock_gettime(struct timespec* ts)
{
    assert(ts != NULL);

    int rc = clock_gettime(CLOCK_MONOTONIC, ts);
    assert(rc == 0);
    (void) rc;
}

void stopwatch_start(struct stopwatch* sw)
{
    assert_clock_gettime(&sw->start_ts);
}

void stopwatch_stop(struct stopwatch* sw)
{
    assert_clock_gettime(&sw->end_ts);
}

size_t stopwatch_get_ns(struct stopwatch* sw)
{
    size_t diff_s  = sw->end_ts.tv_sec  - sw->start_ts.tv_sec;
    size_t diff_ns = sw->end_ts.tv_nsec - sw->start_ts.tv_nsec;
    return (diff_s * 1000 * 1000 * 1000) + diff_ns;
}

size_t stopwatch_get_us(struct stopwatch* sw)
{
    size_t diff_s  = sw->end_ts.tv_sec  - sw->start_ts.tv_sec;
    size_t diff_ns = sw->end_ts.tv_nsec - sw->start_ts.tv_nsec;
    return (diff_s * 1000 * 1000) + (diff_ns / 1000);
}
