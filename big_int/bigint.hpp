
#include <string.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

class BigInt {
 public:
  BigInt();
  BigInt(const BigInt&);
  BigInt(int64_t);
  BigInt(const std::string&);
  ~BigInt() = default;

  BigInt& operator=(const BigInt&);

  BigInt& operator+=(const BigInt&);
  BigInt& operator-=(const BigInt&);
  BigInt& operator*=(const BigInt&);
  BigInt& operator/=(const BigInt&);
  BigInt& operator%=(const BigInt&);

  friend bool operator<(const BigInt&, const BigInt&);
  friend bool operator==(const BigInt&, const BigInt&);

  BigInt operator++(int);
  BigInt& operator++();
  BigInt operator--(int);
  BigInt& operator--();
  BigInt& operator-();

  friend std::ostream& operator<<(std::ostream&, const BigInt&);

 private:
  std::vector<int> digits_;
  const int base_ = 10;
  bool is_negative_;
  void FixBigInt();
};

BigInt operator+(const BigInt&, const BigInt&);
BigInt operator-(const BigInt&, const BigInt&);
BigInt operator/(const BigInt&, const BigInt&);
BigInt operator*(const BigInt&, const BigInt&);
BigInt operator%(const BigInt&, const BigInt&);

bool operator!=(const BigInt&, const BigInt&);
bool operator>(const BigInt&, const BigInt&);
bool operator<=(const BigInt&, const BigInt&);
bool operator>=(const BigInt&, const BigInt&);

std::istream& operator>>(std::istream&, BigInt&);