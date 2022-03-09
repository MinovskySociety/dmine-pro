#ifndef _GUNDAM_MATCH_MATCHCONTAINER_H
#define _GUNDAM_MATCH_MATCHCONTAINER_H
#pragma once
#include <iostream>
#include <set>
#include <vector>

namespace GUNDAM {
template <typename QueryType, typename TargetType>
class SingleMatch {
 private:
  using SingleMatchType = std::map<QueryType, TargetType>;
  using SingleMatchIterator = typename SingleMatch::iterator;
  using SingleMatchConstIterator = typename SingleMatch::const_iterator;
  SingleMatchType single_match_;

 public:
  SingleMatch() { this->single_match_.clear(); }
  SingleMatch(int match_size) {
    // when SingleMatchType is vector,init size
  }
  ~SingleMatch() { this->single_match_.clear(); }
  SingleMatch(const SingleMatch &b) { this->single_match_ = b.single_match_; }
  SingleMatchIterator begin() { return this->single_match_.begin(); }
  SingleMatchIterator end() { return this->single_match_.end(); }
  SingleMatchConstIterator begin() const {
    return this->single_match_.cbegin();
  }
  SingleMatchConstIterator end() const { return this->single_match_.cend(); }
  void Add(QueryType query_vertex, TargetType target_vertex) {
    this->single_match_.insert(std::make_pair(query_vertex, target_vertex));
  }
};

template <class MatchMap>
class MatchContainer {
 private:
  using MatchContainerType = std::vector<MatchMap>;
  using MatchContainerIterator = typename MatchContainerType::iterator;
  using MatchContainerConstIterator =
      typename MatchContainerType::const_iterator;
  MatchContainerType match_container_;

 public:
  MatchContainer() { this->match_container_.clear(); }
  ~MatchContainer() { this->match_container_.clear(); }
  MatchContainer(const MatchContainer &b) {
    this->match_container_ = b.match_container_;
  }
  MatchContainerIterator begin() { return this->match_container_.begin(); }
  MatchContainerIterator end() { return this->match_container_.end(); }
  MatchContainerConstIterator begin() const {
    return this->match_container_.cbegin();
  }
  MatchContainerConstIterator end() const {
    return this->match_container_.cend();
  }
  void Add(MatchMap &match_map) { this->match_container_.push_back(match_map); }
};
}  // namespace GUNDAM

#endif