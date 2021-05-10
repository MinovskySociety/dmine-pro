#ifndef _GUNDAM_DATA_TYPE_LABEL_HPP
#define _GUNDAM_DATA_TYPE_LABEL_HPP

#include <string>
#include <type_traits>
#include <assert.h>

template<typename... LabelUnderlieType>
class Label;

template<typename         Arg1,
         typename         Arg2,
         typename... OtherArgs>
class Label<Arg1,Arg2,OtherArgs...>{
private:
    /// only allows zero or one parameter
    /// trick compiler, same to static_assert(false)
    /// since the template parameter must be used
    static_assert(!std::is_object<Arg1>::value,
                  "Illegal parameter number");
    static_assert( std::is_object<Arg1>::value,
                  "Illegal parameter number");
};

template<>
class Label<>{
    /// trivial label, only have one value
public:
    using UnderlieType = void;

    Label(){
        return;
    }

    Label(Label &&) = default;
    Label(const Label &) = default;

    Label& operator=(const Label&) = default;	  
    Label& operator=(Label&&) = default;	

    inline bool operator ==(const Label<>& label) const{
        return true;
    }
    inline bool CanMatchTo(const Label<>& label) const{
        return true;
    }
    inline std::string to_string() const{
        return "trivial label";
    }
};

template<typename LabelUnderlieType>
class Label<LabelUnderlieType>{
private:
    static_assert(std::is_object<LabelUnderlieType>::value,
                  "Illegal underlie label type");
protected:
    LabelUnderlieType kLable_;

    template<bool judge = std::is_same<LabelUnderlieType,
                                       std::string>::value,
             typename std::enable_if<judge, bool>::type = false>
    inline std::string _to_string() const{
      static_assert(judge == std::is_same<LabelUnderlieType,
                                          std::string>::value,
                   "Illegal usage of this method");
      return this->kLable_;
    }

    template<bool judge = std::is_same<LabelUnderlieType,
                                       std::string>::value,
             typename std::enable_if<!judge, bool>::type = false>
    inline std::string _to_string() const{
      static_assert(judge == std::is_same<LabelUnderlieType,
                                          std::string>::value,
                   "Illegal usage of this method");
        return std::to_string(this->kLable_);
    }

public:
    using UnderlieType = LabelUnderlieType;

    Label():kLable_(){
        return;
    }

    Label(Label &&) = default;
    Label(const Label &) = default;

    Label& operator=(const Label&) = default;	  
    Label& operator=(Label&&) = default;	

    Label(const LabelUnderlieType& kLable_)
                          :kLable_(kLable_){
        return;
    }

    inline bool operator ==(const Label<LabelUnderlieType>& label) const{
        return this->kLable_ == label.kLable_;
    }
    inline bool operator !=(const Label<LabelUnderlieType>& label) const{
        return this->kLable_ != label.kLable_;
    }
    inline bool operator > (const Label<LabelUnderlieType>& label) const{
        return this->kLable_ >  label.kLable_;
    }
    inline bool operator < (const Label<LabelUnderlieType>& label) const{
        return this->kLable_ <  label.kLable_;
    }
    inline bool operator >=(const Label<LabelUnderlieType>& label) const{
        return this->kLable_ >= label.kLable_;
    }
    inline bool operator <=(const Label<LabelUnderlieType>& label) const{
        return this->kLable_ <= label.kLable_;
    }
    inline bool CanMatchTo(const Label<LabelUnderlieType>& label) const{
        return this->kLable_ == label.kLable_;
    }
    inline std::string to_string() const{
      /// to hide the template method
      return this->_to_string();
    }
};
#endif // _LABEL_HPP
