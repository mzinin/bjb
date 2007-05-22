#include <iostream>
#include <bitset>
#include "variables.h"

using namespace std;

class Monom32_64;
class MonomInterface32_64 {
  friend class Monom32_64;

protected:
  int          mDimIndepend;
  Variables*   mIndepend;
  bitset<64>   mask0, mask1;

public:
  MonomInterface32_64(Variables* independ);
  ~MonomInterface32_64() {}

  int dimIndepend() const { return mDimIndepend; }
  const Variables* independ() const { return mIndepend; }

  Monom32_64 create();
  Monom32_64 copy(const Monom32_64& a);
};

class Monom32_64 {
  friend class MonomInterface32_64;

protected:
  MonomInterface32_64* const mRealization;
  unsigned total_degree;
  int mDimIndepend;
  bitset<64> exp;

public:
  Monom32_64 *Next;

  Monom32_64(MonomInterface32_64* r);
  Monom32_64(const Monom32_64& a);
  ~Monom32_64() {Next = NULL;};
//вопрос - зачем эта функция и не оптимизировать ли ее? см. lm() в полиноме
  Monom32_64 copy();

  MonomInterface32_64* monomInterface() const { return mRealization; }
  int dimIndepend() const { return mDimIndepend; }
  const Variables* independ() const { return mRealization->independ(); }

  void set(const Monom32_64& a);
  void operator=(const Monom32_64& a) { set(a); }

  unsigned deg(int var) const;
  unsigned operator[](int var) const { return deg(var); }
  unsigned degree() const;

  void setZero();
  void swap(Monom32_64& a);
  void prolong(int var, int deg=1);
  void div(int var);

  void mult(const Monom32_64& a);
  void mult(const Monom32_64& a, const Monom32_64& b);

  bool divisibility(const Monom32_64& a) const;
  bool divisibilityTrue(const Monom32_64& a) const;
  void divide(const Monom32_64& a, const Monom32_64& b);

  unsigned gcd(const Monom32_64& a) const;
  unsigned lcm(const Monom32_64& a) const;
  void gcd(const Monom32_64& a, const Monom32_64& b);
  void lcm(const Monom32_64& a, const Monom32_64& b);

  int compare(const Monom32_64& a) const;
  int compare(const Monom32_64& a, const Monom32_64& b) const;

  friend std::istream& operator>>(std::istream& in, Monom32_64& a);
  friend std::ostream& operator<<(std::ostream& out, const Monom32_64& a);
  friend bool operator==(const Monom32_64 &a, const Monom32_64 &b);
  friend bool operator!=(const Monom32_64 &a, const Monom32_64 &b) { return !(a==b); };

  //void assertValid(const char* fileName, int fileLine) const;
};

inline unsigned Monom32_64::deg(int var) const {
  return exp.test(2*var) + 2*exp.test(2*var+1);
}

inline unsigned Monom32_64::degree() const {
  return total_degree;
}

inline void Monom32_64::setZero() {
  total_degree = 0;
  exp = 0;
}

inline void Monom32_64::prolong(int var, int deg) {
  unsigned long t = deg;
  t = t << 2*var;
  exp = t + *(unsigned long*)&exp;
  total_degree += deg;
}

inline void Monom32_64::mult(const Monom32_64& a) {
  exp = *(unsigned long*)&exp + *(unsigned long*)&a.exp;
  total_degree+=a.total_degree;
}

inline void Monom32_64::mult(const Monom32_64& a, const Monom32_64& b) {
  exp = *(unsigned long*)&a.exp + *(unsigned long*)&b.exp;
  total_degree = a.total_degree + b.total_degree;
}

inline bool Monom32_64::divisibility(const Monom32_64& a) const{
  if (total_degree < a.total_degree)
    return false;

  bitset<64> a1,b1,l1;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;

  l1 = a1 ^ b1;
  l1&=b1;
  if (l1!=0)
    return false;
  else{
    bitset<64> a0,b0,l2;
    a0 = exp & mRealization->mask0;
    b0 = a.exp & mRealization->mask0;
    l1 = a0 ^ b0;
    l2 = ~a1 | b1;
    l1&= l2 & b0;
    if (l1!=0)
      return false;
    else
      return true;
  }
}

inline bool Monom32_64::divisibilityTrue(const Monom32_64& a) const {
  if (total_degree <= a.total_degree)
    return false;

  bitset<64> a1,b1,l1;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;

  l1 = a1 ^ b1;
  l1&=b1;
  if (l1!=0)
    return false;
  else{
    bitset<64> a0,b0,l2;
    a0 = exp & mRealization->mask0;
    b0 = a.exp & mRealization->mask0;
    l1 = a0 ^ b0;
    l2 = ~a1 | b1;
    l1&= l2 & b0;
    if (l1!=0)
      return false;
    else
      return true;
  }
}

inline void Monom32_64::divide(const Monom32_64& a, const Monom32_64& b) {
  total_degree = a.total_degree - b.total_degree;
  exp = *(unsigned long*)&a.exp - *(unsigned long*)&b.exp;
}


inline unsigned Monom32_64::gcd(const Monom32_64& a) const {
  bitset<64> a0,a1,b0,b1,e0;
  unsigned deg;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;
  e0 = a1 & b1;
  deg = 2*e0.count();

  e0 = a1 ^ b1;
  a0 = exp & mRealization->mask0;
  b0 = a.exp & mRealization->mask0;
  a0 = (a1 & e0) | a0;
  b0 = (b1 & e0) | b0;
  e0 = a0 & b0;
  deg+= e0.count();

  return deg;
}

inline unsigned Monom32_64::lcm(const Monom32_64& a) const {
  bitset<64> a0,a1,b0,b1,e0;
  unsigned deg;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;
  e0 = a1 | b1;
  deg = 2*e0.count();

  e0 = (~(a1 ^ b1)) & mRealization->mask0;
  a0 = exp & mRealization->mask0;
  b0 = a.exp & mRealization->mask0;
  a0 = (a1 | e0) & a0;
  b0 = (b1 | e0) & b0;
  e0 = a0 | b0;
  deg+= e0.count();

  return deg;
}

inline void Monom32_64::gcd(const Monom32_64& a, const Monom32_64& b) {
  bitset<64> a0,a1,b0,b1,e0;
  a1 = a.exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = b.exp & mRealization->mask1; b1 = b1 >> 1;
  e0 = a1 & b1;
  total_degree = 2*e0.count();
  exp = e0 << 1;

  e0 = a1 ^ b1;
  a0 = a.exp & mRealization->mask0;
  b0 = b.exp & mRealization->mask0;
  a0 = (a1 & e0) | a0;
  b0 = (b1 & e0) | b0;
  e0 = a0 & b0;
  total_degree+= e0.count();
  exp|= e0;
}

inline void Monom32_64::lcm(const Monom32_64& a, const Monom32_64& b) {
  bitset<64> a0,a1,b0,b1,e0;
  a1 = a.exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = b.exp & mRealization->mask1; b1 = b1 >> 1;
  e0 = a1 | b1;
  total_degree = 2*e0.count();
  exp = e0 << 1;

  e0 = (~(a1 ^ b1)) & mRealization->mask0;
  a0 = a.exp & mRealization->mask0;
  b0 = b.exp & mRealization->mask0;
  a0 = (a1 | e0) & a0;
  b0 = (b1 | e0) & b0;
  e0 = a0 | b0;
  total_degree+= e0.count();
  exp|= e0;
}


inline int Monom32_64::compare(const Monom32_64& a) const {
  if (total_degree<a.total_degree)
    return -1;
  else if (total_degree>a.total_degree)
    return 1;
  else {
    if (*(unsigned long*)&exp < *(unsigned long*)&a.exp)
      return 1;
    else if (*(unsigned long*)&exp > *(unsigned long*)&a.exp)
      return -1;
    else
      return 0;
  }
}

inline bool operator==(const Monom32_64 &a, const Monom32_64 &b){
  return a.exp==b.exp;
}
