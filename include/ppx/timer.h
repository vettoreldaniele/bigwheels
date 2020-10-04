#ifndef PPX_TIMER_H
#define PPX_TIMER_H

//
//
// PLATFORM NOTE: Linux
//   clock_gettime and clock_getres defaults to CLOCK_MONOTONIC_RAW for clk_id parameter.
//   Use PPX_TIMER_FORCE_MONOTONIC to force CLOCK_MONOTONIC.
//

#include <cstdint>

// clang-format off
#define PPX_TIMER_SECONDS_TO_NANOS  1000000000
#define PPX_TIMER_MILLIS_TO_NANOS   1000000
#define PPX_TIMER_MICROS_TO_NANOS   1000
#define PPX_TIMER_NANOS_TO_SECONDS  0.000000001
#define PPX_TIMER_NANOS_TO_MILLIS   0.000001
#define PPX_TIMER_NANOS_TO_MICROS   0.001
// clang-format on

// Select which clock to use
// clang-format off
#if defined(PPX_TIMER_FORCE_MONOTONIC)
    #define PPX_TIMER_CLK_ID CLOCK_MONOTONIC
#else
    #define PPX_TIMER_CLK_ID CLOCK_MONOTONIC_RAW
#endif
// clang-format on

namespace ppx {

// clang-format off
enum TimerResult
{
    TIMER_RESULT_SUCCESS                 =  0,
    TIMER_RESULT_ERROR_NULL_POINTER      = -1,
    TIMER_RESULT_ERROR_INITIALIZE_FAILED = -2,
    TIMER_RESULT_ERROR_TIMESTAMP_FAILED  = -3,
    TIMER_RESULT_ERROR_SLEEP_FAILED      = -4,
    TIMER_RESULT_ERROR_CORRUPTED_DATA    = -5,
};
// clang-format on

class Timer
{
public:
    Timer() {}
    ~Timer() {}

    static TimerResult InitializeStaticData();

    static TimerResult Timestamp(uint64_t* pTimestamp);
    static double      TimestampToSeconds(uint64_t timestamp);
    static double      TimestampToMillis(uint64_t timestamp);
    static double      TimestampToMicros(uint64_t timestamp);
    static double      TimestampToNanos(uint64_t timestamp);

    static TimerResult SleepSeconds(double seconds);
    static TimerResult SleepMillis(double millis);
    static TimerResult SleepMicros(double micros);
    static TimerResult SleepNanos(double nanos);

    TimerResult Start();
    TimerResult Stop();
    uint64_t    StartTimestamp() const;
    uint64_t    StopTimestamp() const;
    double      SecondsSinceStart() const;
    double      MillisSinceStart() const;
    double      MicrosSinceStart() const;
    double      NanosSinceStart() const;

private:
    bool     mInitialized    = false;
    uint64_t mStartTimestamp = 0;
    uint64_t mStopTimestamp  = 0;
};

} // namespace ppx

#endif // PPX_CORE_TIMER_H
