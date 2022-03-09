#ifndef _GUNDAM_COMPONENT_TIMER_H
#define _GUNDAM_COMPONENT_TIMER_H

#include <ctime>

namespace GUNDAM {
#ifdef _TIMER_NUMBER
namespace timer {
constexpr int kTimerNum = _TIMER_NUM;
static clock_t timer_begin;
static clock_t interval_begin;
static clock_t timer[kTimerNum];
static int count[kTimerNum];
}  // namespace timer
#endif

inline void TimerStart() {
#ifdef _TIMER_NUMBER
  for (int i = 0; i < timer::kTimerNum; ++i) {
    timer::timer[i] = 0;
    timer::count[i] = 0;
  }
  timer::timer_begin = timer::interval_begin = clock();
#endif
}

inline void TimerIntervalReset() {
#ifdef _TIMER_NUMBER
  timer::interval_begin = clock();
#endif
}

inline void TimerAddUpInterval(int index) {
#ifdef _TIMER_NUMBER
  auto now_time = clock();
  timer::timer[index] += (now_time - timer::interval_begin);
  timer::count[index]++;
  timer::interval_begin = now_time;
#endif
}

inline void TimerFinish() {
#ifdef _TIMER_NUMBER
  clock_t end_time = clock();
  std::cout << "Total time: "
            << static_cast<double>(end_time - timer::timer_begin) /
                   CLOCKS_PER_SEC
            << " s" << std::endl;

  for (int i = 0; i < timer::kTimerNum; ++i) {
    std::cout << "Timer " << i << ": "
              << static_cast<double>(timer::timer[i]) / CLOCKS_PER_SEC << " s"
              << ", Count: " << timer::count[i] << std::endl;
  }
#endif
}

}  // namespace GUNDAM

#endif