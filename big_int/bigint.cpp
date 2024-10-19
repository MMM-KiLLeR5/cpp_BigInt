#include "bigint.hpp"

BigInt::BigInt() : is_negative_(false), digits_(1, 0) {}

void BigInt::FixBigInt() {
  while (digits_.size() > 1 && digits_.back() == 0) {
    digits_.pop_back();
  }
  is_negative_ = (digits_.size() == 1 && is_negative_ && digits_[0] == 0)
                 ? false
                 : is_negative_;
}

BigInt::BigInt(const std::string& kString) {
  is_negative_ = false;
  size_t break_point = 0;
  if (kString[0] == '-') {
    is_negative_ = true;
    break_point = 1;
  }
  for (int i = static_cast<int>(kString.size()) - 1;
       i >= static_cast<int>(break_point); --i) {
    digits_.push_back(kString[i] - '0');
    if (i == static_cast<int>(break_point)) {
      break;
    }
  }
  FixBigInt();
}

BigInt::BigInt(int64_t Number) {
  is_negative_ = Number < 0;
  while (Number != 0) {
    digits_.push_back(std::abs(Number % base_));
    Number /= base_;
  }
  if (digits_.empty()) {
    digits_.push_back(0);
  }
  FixBigInt();
}

BigInt::BigInt(const BigInt& kBigIntNum) {
  is_negative_ = kBigIntNum.is_negative_;
  digits_ = kBigIntNum.digits_;
}

BigInt& BigInt::operator=(const BigInt& kBigIntNum) {
  is_negative_ = kBigIntNum.is_negative_;
  digits_.clear();
  for (size_t i = 0; i < kBigIntNum.digits_.size(); ++i) {
    digits_.push_back(kBigIntNum.digits_[i]);
  }
  return *this;
}

BigInt& BigInt::operator+=(const BigInt& kBigIntNum) {
  if ((!is_negative_ && !kBigIntNum.is_negative_) ||
      (is_negative_ && kBigIntNum.is_negative_)) {
    int temp = 0;
    for (size_t i = 0; i < std::max(digits_.size(), kBigIntNum.digits_.size());
         ++i) {
      if (i < digits_.size() && i < kBigIntNum.digits_.size()) {
        digits_[i] += (kBigIntNum.digits_[i] + temp);
        temp = digits_[i] / base_;
        digits_[i] %= base_;
      } else if (i < digits_.size()) {
        digits_[i] += temp;
        temp = digits_[i] / base_;
        digits_[i] %= base_;
      } else {
        int temp_num;
        temp_num = kBigIntNum.digits_[i] + temp;
        temp = temp_num / base_;
        temp_num %= base_;
        digits_.push_back(temp_num);
      }
    }
    if (temp > 0) {
      digits_.push_back(temp);
    }
  } else if (is_negative_) {
    BigInt Temp_BigInt = kBigIntNum;
    Temp_BigInt -= (-*this);
    *this = Temp_BigInt;
    FixBigInt();
  } else {
    *this = (BigInt(kBigIntNum) -= (-*this));
  }
  return *this;
}

BigInt& BigInt::operator-=(const BigInt& kBigIntNum) {
  if ((is_negative_ && !kBigIntNum.is_negative_) ||
      (!is_negative_ && kBigIntNum.is_negative_)) {
    *this += -BigInt(kBigIntNum);
    FixBigInt();
  } else {
    if ((*this < kBigIntNum && !is_negative_) ||
        (*this > kBigIntNum && is_negative_)) {
      *this = -(BigInt(kBigIntNum) -= *this);
      FixBigInt();
    } else {
      int borrow = 0;
      for (size_t i = 0;
           i < std::max(digits_.size(), kBigIntNum.digits_.size()) ||
               borrow != 0;
           ++i) {
        int subtrahend = borrow + (i < digits_.size() ? digits_[i] : 0);
        if (i < kBigIntNum.digits_.size()) {
          subtrahend -= kBigIntNum.digits_[i];
        }

        if (subtrahend < 0) {
          borrow = -1;
          subtrahend += base_;
        } else {
          borrow = 0;
        }

        if (i < digits_.size()) {
          digits_[i] = subtrahend;
        } else {
          digits_.push_back(subtrahend);
        }
      }
      FixBigInt();
    }
  }
  FixBigInt();
  return *this;
}

BigInt& BigInt::operator*=(const BigInt& kBigIntNum) {
  is_negative_ = is_negative_ != kBigIntNum.is_negative_;
  if (*this == 0 || kBigIntNum == 0) {
    *this = 0;
    return *this;
  }
  BigInt result;
  result.digits_.resize(digits_.size() + kBigIntNum.digits_.size());
  for (size_t i = 0; i < digits_.size(); ++i) {
    BigInt temp;
    temp.digits_.resize(i);
    int carry = 0;
    for (size_t j = 0; j < kBigIntNum.digits_.size(); ++j) {
      int temp_num = digits_[i] * kBigIntNum.digits_[j];
      temp_num += carry;
      temp.digits_.push_back(temp_num % base_);
      carry = temp_num / base_;
    }
    if (carry > 0) {
      temp.digits_.push_back(carry);
    }
    result += temp;
  }
  result.is_negative_ = is_negative_;
  *this = result;
  FixBigInt();
  return *this;
}

BigInt& BigInt::operator/=(const BigInt& kBigIntNum) {
  is_negative_ = is_negative_ ^ kBigIntNum.is_negative_;
  if (kBigIntNum == 0) {
    throw("UB");
  }
  BigInt result;
  result.digits_.clear();
  BigInt temp;
  temp.digits_.clear();
  BigInt copy(kBigIntNum);
  copy.is_negative_ = false;
  for (int i = digits_.size() - 1; i >= 0; --i) {
    temp.digits_.insert(temp.digits_.begin(), digits_[i]);
    int quotient = 0;
    while (temp >= copy) {
      temp -= copy;
      ++quotient;
    }
    if (temp == 0) {
      temp.digits_.clear();
    }
    if (quotient != 0 || !result.digits_.empty()) {
      result.digits_.push_back(quotient);
    }
  }
  result.is_negative_ = is_negative_;
  reverse(result.digits_.begin(), result.digits_.end());
  if (result.digits_.empty()) {
    result.digits_.push_back(0);
  }
  *this = result;
  FixBigInt();
  return *this;
}

BigInt& BigInt::operator%=(const BigInt& kBigIntNum) {
  BigInt other = *this;
  *this /= kBigIntNum;
  *this *= kBigIntNum;
  *this -= other;
  return -(*this);
}

BigInt operator+(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  BigInt result = kBigIntA;
  result += kBigIntB;
  return result;
}

BigInt operator-(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  BigInt result = kBigIntA;
  result -= kBigIntB;
  return result;
}

BigInt operator*(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  BigInt result = kBigIntA;
  result *= kBigIntB;
  return result;
}

BigInt operator/(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  BigInt result = kBigIntA;
  result /= kBigIntB;
  return result;
}

BigInt operator%(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  BigInt result = kBigIntA;
  result %= kBigIntB;
  return result;
}

BigInt& BigInt::operator++() { return (*this += 1); }

BigInt BigInt::operator++(int) {
  BigInt temp(*this);
  ++(*this);
  return temp;
}

BigInt& BigInt::operator--() { return (*this -= 1); }

BigInt BigInt::operator--(int) {
  BigInt temp(*this);
  --(*this);
  return temp;
}

bool operator==(const BigInt& kBigIntA, const BigInt& kBigintB) {
  if ((kBigIntA.is_negative_ != kBigintB.is_negative_) ||
      (kBigIntA.digits_.size() != kBigintB.digits_.size()) ||
      (kBigIntA.digits_ != kBigintB.digits_)) {
    return false;
  }
  return true;
}

bool operator!=(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  return !(kBigIntA == kBigIntB);
}

bool operator<(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  bool TwoNegative = false;
  if (kBigIntA.is_negative_) {
    if (kBigIntB.is_negative_) {
      TwoNegative = true;
    } else {
      return true;
    }
  }
  if (kBigIntB.is_negative_ && !TwoNegative) {
    return false;
  }
  if (kBigIntA.digits_.size() != kBigIntB.digits_.size()) {
    return (kBigIntA.digits_.size() < kBigIntB.digits_.size()) != TwoNegative;
  } else {
    for (int i = static_cast<int>(kBigIntB.digits_.size()) - 1; i >= 0; --i) {
      if (kBigIntA.digits_[i] != kBigIntB.digits_[i]) {
        return (kBigIntA.digits_[i] < kBigIntB.digits_[i]) != TwoNegative;
      }
    }
    return TwoNegative;
  }
}

BigInt& BigInt::operator-() {
  is_negative_ = !is_negative_;
  FixBigInt();
  return *this;
}

bool operator>(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  return (kBigIntA != kBigIntB) && !(kBigIntA < kBigIntB);
}

bool operator<=(const BigInt& kBigIntA, const BigInt& kBigIntB) {
  return !(kBigIntA > kBigIntB);
}

bool operator>=(const BigInt& kBigIntA, const BigInt& kBigintB) {
  return !(kBigIntA < kBigintB);
}

std::ostream& operator<<(std::ostream& out, const BigInt& kBigInt) {
  if (kBigInt.is_negative_) {
    out << "-";
  }
  for (int i = static_cast<int>(kBigInt.digits_.size() - 1); i >= 0; --i) {
    out << kBigInt.digits_[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, BigInt& BigIntNum) {
  std::string str;
  in >> str;
  BigIntNum = str;
  return in;
}