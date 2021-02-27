#ifndef _TOP_K_CONTAINER_H
#define _TOP_K_CONTAINER_H

#include <algorithm>
#include <vector>

namespace gmine_new {

template <class T, class Compare = std::greater<T>>
class TopKContainer {
 public:
  using const_iterator = typename std::vector<T>::const_iterator;

  TopKContainer() = delete;

  TopKContainer(size_t k) : k_(k) { top_k_heap_.reserve(k); }

  TopKContainer(const TopKContainer &) = delete;

  TopKContainer(TopKContainer &&) = delete;

  TopKContainer &operator=(const TopKContainer &) = delete;

  TopKContainer &operator=(TopKContainer &&) = delete;

  const_iterator begin() const noexcept { return top_k_heap_.begin(); }

  const_iterator cbegin() const noexcept { return top_k_heap_.cbegin(); }

  const_iterator end() const noexcept { return top_k_heap_.end(); }

  const_iterator cend() const noexcept { return top_k_heap_.end(); }

  void clear() noexcept { top_k_heap_.clear(); }

  template <class... Args>
  void emplace(Args &&... args) {
    if (top_k_heap_.size() < k_) {
      top_k_heap_.emplace_back(std::forward<Args>(args)...);
      std::push_heap(top_k_heap_.begin(), top_k_heap_.end(), Compare());
    } else {
      T t(std::forward<Args>(args)...);
      if (Compare()(t, top_k_heap_.front())) {
        std::pop_heap(top_k_heap_.begin(), top_k_heap_.end(), Compare());
        top_k_heap_.back() = std::move(t);
        std::push_heap(top_k_heap_.begin(), top_k_heap_.end(), Compare());
      }
    }
  }

 private:
  size_t k_;
  std::vector<T> top_k_heap_;
};
}  // namespace gmine_new
#endif