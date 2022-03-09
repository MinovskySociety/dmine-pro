#ifndef _GUNDAM_COMPONENT_POINTER_H
#define _GUNDAM_COMPONENT_POINTER_H

#include <type_traits>

namespace GUNDAM {

template <bool is_const, class ContentType, 
                    class ConstContentType>
class GPointer {
 public:
  GPointer() = default;

  GPointer(const GPointer &b) = default;

  GPointer(GPointer &&b) = default;

  GPointer(const typename std::conditional<is_const, ConstContentType,
                                           ContentType>::type &v)
      : v_(v) {}

  GPointer(typename std::conditional<is_const, ConstContentType,
                                     ContentType>::type &&v)
      : v_(std::move(v)) {}

  GPointer(typename std::conditional<is_const, const ContentType *,
                                     ContentType *>::type p) {
    if (p) {
      v_ = *p;
    }
  }

  GPointer &operator=(const GPointer &b) = default;

  GPointer &operator=(GPointer &&b) = default;

  GPointer &operator=(typename std::conditional<is_const, const ContentType *,
                                                ContentType *>::type *p) {
    if (p) {
      v_ = *p;
    } else {
      v_.Reset();
    }
    return *this;
  }

  ~GPointer() = default;

  typename std::conditional<is_const, const ConstContentType &, 
                                                 ContentType &>::type
  operator*() {
    assert(!IsNull());
    return v_;
  }

  typename std::conditional<is_const, const ConstContentType &,
                                           const ContentType &>::type &
  operator*() const {
    assert(!IsNull());
    return v_;
  }

  typename std::conditional<is_const, const ConstContentType *,  
                                                 ContentType *>::type
  operator->() {
    assert(!IsNull());
    return &v_;
  }

  typename std::conditional<is_const, const ConstContentType *,
                                      const ContentType *>::type
  operator->() const {
    assert(!IsNull());
    return &v_;
  }

  bool operator==(const GPointer &b) const { 
    return v_ == b.v_; 
  }

  bool operator!=(const GPointer &b) const { 
    return !(*this == b); 
  }

  bool operator<(const GPointer &b) const { return v_ < b.v_; }

  bool IsNull() const { return !v_.HasValue(); }

  operator bool() const { return v_.HasValue(); };

 private:
  typename std::conditional<is_const, ConstContentType, ContentType>::type v_;
};

// template <bool is_const, class ContentType, class ConstContentType>
// class GPointer {
// public:
//  GPointer() : is_null_(true){}
//
//  template <bool judge = is_const,
//            class = typename std::enable_if<!judge>::type>
//  GPointer(const GPointer<ContentType, false> &b) : is_null_{b.is_null_} {
//    if (!is_null_) {
//      v_ = b.v_;
//    }
//  }
//
//  GPointer(const GPointer<ConstContentType, true> &b) : is_null_{b.is_null_} {
//    if (!is_null_) {
//      v_ = b.v_;
//    }
//  }
//
//  template <bool judge = is_const,
//            class = typename std::enable_if<!judge>::type>
//  GPointer(GPointer<ContentType, false> &&b) : is_null_{std::move(b.is_null_)}
//  {
//    if (!is_null_) {
//      v_ = std::move(b.v_);
//    }
//  }
//
//  GPointer(GPointer<ConstContentType, true> &&b) :
//  is_null_{std::move(b.is_null_)} {
//    if (!is_null_) {
//      v_ = std::move(b.v_);
//    }
//  }
//
//  GPointer(const ContentType &v) : v_(v), is_null_(false) {}
//
//  GPointer(typename std::conditional<is_const, const ContentType *,
//                                     ContentType *>::type p) {
//    if (p) {
//      is_null_ = false;
//      v_ = *p;
//    } else {
//      is_null_ = true;
//    }
//  }
//
//  GPointer &operator=(const GPointer &b) {
//    is_null_ = b.is_null_;
//    if (!is_null_) {
//      v_ = b.v_;
//    }
//    return *this;
//  }
//
//  GPointer &operator=(GPointer &&b) {
//    is_null_ = std::move(b.is_null_);
//    if (!is_null_) {
//      v_ = std::move(b.v_);
//    }
//    return *this;
//  }
//
//  GPointer &operator=(typename std::conditional<is_const, const ContentType *,
//                                                ContentType *>::type *p) {
//    if (p) {
//      is_null_ = false;
//      v_ = *p;
//    } else {
//      is_null_ = true;
//    }
//    return *this;
//  }
//
//  ~GPointer(){};
//
//
//  typename std::conditional<is_const, const ContentType &, ContentType
//  &>::type operator*() {
//    assert(!IsNull());
//    return v_;
//  }
//
//  const ContentType &operator*() const {
//    assert(!IsNull());
//    return v_;
//  }
//
//  typename std::conditional<is_const, const ContentType *, ContentType
//  *>::type operator->() {
//    assert(!IsNull());
//    return &v_;
//  }
//
//  const ContentType *operator->() const {
//    assert(!IsNull());
//    return &v_;
//  }
//
//  bool operator==(const GPointer &b) const {
//    if (IsNull()) {
//      return b.IsNull();
//    } else if (b.IsNull()) {
//      return false;
//    } else {
//      return v_ == b.v_;
//    }
//  }
//
//  bool operator<(const GPointer &b) const {
//    if (IsNull() || b.IsNull()) {
//      return false;
//    } else {
//      return v_ < b.v_;
//    }
//  }
//
//  bool IsNull() const { return is_null_; }
//
//  operator bool() const { return !IsNull(); };
//
// private:
//  ContentType v_;
//  bool is_null_;
//};
}  // namespace GUNDAM

#endif