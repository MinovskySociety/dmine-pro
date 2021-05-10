#ifndef _GUNDAM_DATA_TYPE_DATETIME_H
#define _GUNDAM_DATA_TYPE_DATETIME_H

#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

namespace GUNDAM {
//类型说明：绝对时间
//格式:年-月-日 时:分:秒

class DateTime {
 public:
  DateTime() : t_(0) {}

  DateTime(time_t t) : t_(t) {}

  DateTime(const char* str) {
    if (!str || str[0] == '\0') {
      t_ = 0;
      return;
    }

    int year, month, day, hour, minute, second;
    sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute,
           &second);
    tm tm_;
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    t_ = mktime(&tm_);
  }

  DateTime(const std::string& str) : DateTime(str.c_str()) {}

  std::string to_string() const {
    char str[64];
    strftime(str, 64, "%Y-%m-%d %H:%M:%S", std::localtime(&this->t_));
    return std::string(str);
  }

  friend std::ostream& operator<<(std::ostream& out, const DateTime& b) {
    out << b.to_string();
    return out;
  }

  friend std::stringstream& operator<<(std::stringstream& out,
                                       const DateTime& b) {
    out << b.to_string();
    return out;
  }

  bool operator==(const DateTime& b) const { return t_ == b.t_; }
  bool operator!=(const DateTime& b) const { return t_ != b.t_; }
  bool operator<(const DateTime& b) const { return t_ < b.t_; }
  bool operator>(const DateTime& b) const { return t_ > b.t_; }
  bool operator<=(const DateTime& b) const { return t_ <= b.t_; }
  bool operator>=(const DateTime& b) const { return t_ >= b.t_; }

 private:
  time_t t_;
};

}  // namespace GUNDAM
#endif