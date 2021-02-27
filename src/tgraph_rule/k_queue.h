#ifndef KQUEUE_H
#define KQUEUE_H

#include "config.h"
#include "global.h"

namespace toy {
template <class T>
class KQueue {
 public:
  using iterator = typename std::list<std::pair<T, GRADE_T>>::iterator;
  using reverse_iterator =
      typename std::list<std::pair<T, GRADE_T>>::reverse_iterator;
  using const_iterator =
      typename std::list<std::pair<T, GRADE_T>>::const_iterator;
  using const_reverse_iterator =
      typename std::list<std::pair<T, GRADE_T>>::const_reverse_iterator;

  KQueue() {}
  KQueue(size_t capecity) : capecity_(capecity) {}
  ~KQueue() {}

  void init(size_t capecity) {
    top_k_list_.clear();
    capecity_ = capecity;
  }

  static bool cmp_conf(std::pair<T, GRADE_T> &a, std::pair<T, GRADE_T> &b) {
    return a.second < b.second;
  }

  inline iterator begin() { return top_k_list_.begin(); }
  inline iterator end() { return top_k_list_.end(); }
  inline reverse_iterator rbegin() { return top_k_list_.rbegin(); }
  inline reverse_iterator rend() { return top_k_list_.rend(); }

  void push(std::pair<T, GRADE_T> val) {
    if (capecity_ != 0 && top_k_list_.size() == capecity_) {
      pop();
    }
    top_k_list_.emplace_back(val);
    top_k_list_.sort(cmp_conf);
  }

  void emplace(std::pair<T, GRADE_T> &val) {
    if (capecity_ != 0 && top_k_list_.size() == capecity_) {
      pop();
    }
    top_k_list_.emplace_back(val);
    top_k_list_.sort(cmp_conf);
  }

  std::pair<T, GRADE_T> &top() { return *top_k_list_.begin(); }

  void pop() { top_k_list_.pop_front(); }

  inline size_t size() const { return top_k_list_.size(); }
  inline bool empty() const { return top_k_list_.empty(); }
  inline bool full() const { return top_k_list_.size() == capecity_; }
  inline const_iterator cbegin() const { return top_k_list_.cbegin(); }
  inline const_iterator cend() const { return top_k_list_.cend(); }
  inline const_reverse_iterator crbegin() const {
    return top_k_list_.crbegin();
  }
  inline const_reverse_iterator crend() const { return top_k_list_.crend(); }

 private:
  std::list<std::pair<T, GRADE_T>> top_k_list_;
  size_t capecity_ = 0;
};

}  // namespace toy

#endif  // KQUEUE_H
