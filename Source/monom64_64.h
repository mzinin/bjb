#include <iostream>
#include <bitset>
#include "variables.h"

using namespace std;

class Monom64_64;
class MonomInterface64_64 {
  friend class Monom64_64;

protected:
  int          mDimIndepend;
  Variables*   mIndepend;
  bitset<128>  mask0, mask1;

public:
  MonomInterface64_64(Variables* independ);
  ~MonomInterface64_64() {}

  int dimIndepend() const { return mDimIndepend; }
  const Variables* independ() const { return mIndepend; }

  Monom64_64 create();
  Monom64_64 copy(const Monom64_64& a);
};

class Monom64_64 {
  friend class MonomInterface64_64;

protected:
  MonomInterface64_64* const mRealization;
  unsigned total_degree;
  int mDimIndepend;
  bitset<128> exp;

public:
  Monom64_64 *Next;

  Monom64_64(MonomInterface64_64* r);
  Monom64_64(const Monom64_64& a);
  ~Monom64_64() {Next = NULL;};
//вопрос - зачем эта функция и не оптимизировать ли ее? см. lm() в полиноме
  Monom64_64 copy();

  MonomInterface64_64* monomInterface() const { return mRealization; }
  int dimIndepend() const { return mDimIndepend; }
  const Variables* independ() const { return mRealization->independ(); }

  void set(const Monom64_64& a);
  void operator=(const Monom64_64& a) { set(a); }

  unsigned deg(int var) const;
  unsigned operator[](int var) const { return deg(var); }
  unsigned degree() const;

  void setZero();
  void swap(Monom64_64& a);
  void prolong(int var, int deg=1);
  void prolong1(int var, int deg=1);
  void div(int var);

  void mult(const Monom64_64& a);
  void mult(const Monom64_64& a, const Monom64_64& b);
  void mult1(const Monom64_64& a);
  void mult1(const Monom64_64& a, const Monom64_64& b);

  bool divisibility(const Monom64_64& a) const;
  bool divisibilityTrue(const Monom64_64& a) const;
  void divide(const Monom64_64& a, const Monom64_64& b);

  unsigned gcd(const Monom64_64& a) const;
  unsigned lcm(const Monom64_64& a) const;
  void gcd(const Monom64_64& a, const Monom64_64& b);
  void lcm(const Monom64_64& a, const Monom64_64& b);

  int compare(const Monom64_64& a) const;
  int compare(const Monom64_64& a, const Monom64_64& b) const;

  friend std::istream& operator>>(std::istream& in, Monom64_64& a);
  friend std::ostream& operator<<(std::ostream& out, const Monom64_64& a);
  friend bool operator==(const Monom64_64 &a, const Monom64_64 &b);
  friend bool operator!=(const Monom64_64 &a, const Monom64_64 &b) { return !(a==b); };

  bool isHG() const;
  int num_of_only_var() const;
  bool equality(const Monom64_64& a, int var) const;
  //void assertValid(const char* fileName, int fileLine) const;
};

inline unsigned Monom64_64::deg(int var) const {
  return exp.test(2*var) + 2*exp.test(2*var+1);
}

inline unsigned Monom64_64::degree() const {
  return total_degree;
}

inline void Monom64_64::setZero() {
  total_degree = 0;
  exp = 0;
}

inline void Monom64_64::prolong(int var, int deg) {
  unsigned long t = deg, *exp_long;
  if (var<32){
    t = t << 2*var;
    exp_long = (unsigned long*)&exp;
  }
  else{
    t = t << 2*(var-32);
    exp_long = (unsigned long*)&exp + 1;
  }
  *exp_long += t;
  total_degree += deg;
}

inline void Monom64_64::prolong1(int var, int deg) {
  bitset<128> a0,a1;
  a0 = exp & mRealization->mask0;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  exp = a0 | a1;
  exp.set(2*var);
  total_degree = exp.count();
}

inline void Monom64_64::mult(const Monom64_64& a) {
  unsigned long *exp_long = (unsigned long*)&exp,
                *a_exp_long = (unsigned long*)&a.exp;
  *exp_long += *a_exp_long;
  exp_long++;
  a_exp_long++;
  *exp_long += *a_exp_long;
  total_degree+=a.total_degree;
}

inline void Monom64_64::mult(const Monom64_64& a, const Monom64_64& b) {
  unsigned long *exp_long = (unsigned long*)&exp,
                *a_exp_long = (unsigned long*)&a.exp,
                *b_exp_long = (unsigned long*)&b.exp;
  *exp_long = *a_exp_long + *b_exp_long;
  exp_long++;
  a_exp_long++;
  b_exp_long++;
  *exp_long = *a_exp_long + *b_exp_long;
  total_degree = a.total_degree + b.total_degree;
}

inline void Monom64_64::mult1(const Monom64_64& a) {
  bitset<128> res = 0, a1;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  res |= a1;
  a1 = a.exp & mRealization->mask1; a1 = a1 >> 1;
  res |= a1;
  a1 = exp & mRealization->mask0;
  res |= a1;
  a1 = a.exp & mRealization->mask0;
  res |= a1;
  exp = res;
  total_degree = exp.count();
}

inline void Monom64_64::mult1(const Monom64_64& a, const Monom64_64& b) {
  bitset<128> a1;
  exp = 0;
  a1 = a.exp & mRealization->mask1; a1 = a1 >> 1;
  exp |= a1;
  a1 = b.exp & mRealization->mask1; a1 = a1 >> 1;
  exp |= a1;
  a1 = a.exp & mRealization->mask0;
  exp |= a1;
  a1 = b.exp & mRealization->mask0;
  exp |= a1;
  total_degree = exp.count();
}

inline bool Monom64_64::divisibility(const Monom64_64& a) const{
  if (total_degree < a.total_degree)
    return false;

  bitset<128> a1,b1,l1;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;

  l1 = a1 ^ b1;
  l1&=b1;
  if (l1!=0)
    return false;
  else{
    bitset<128> a0,b0,l2;
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

inline bool Monom64_64::divisibilityTrue(const Monom64_64& a) const {
  if (total_degree <= a.total_degree)
    return false;

  bitset<128> a1,b1,l1;
  a1 = exp & mRealization->mask1; a1 = a1 >> 1;
  b1 = a.exp & mRealization->mask1; b1 = b1 >> 1;

  l1 = a1 ^ b1;
  l1&=b1;
  if (l1!=0)
    return false;
  else{
    bitset<128> a0,b0,l2;
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

inline void Monom64_64::divide(const Monom64_64& a, const Monom64_64& b) {
  total_degree = a.total_degree - b.total_degree;
  unsigned long *exp_long = (unsigned long*)&exp,
                *a_exp_long = (unsigned long*)&a.exp,
                *b_exp_long = (unsigned long*)&b.exp;
  *exp_long = *a_exp_long - *b_exp_long;
  exp_long++;
  a_exp_long++;
  b_exp_long++;
  *exp_long = *a_exp_long - *b_exp_long;
}

inline unsigned Monom64_64::gcd(const Monom64_64& a) const {
  bitset<128> a0,a1,b0,b1,e0;
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

inline unsigned Monom64_64::lcm(const Monom64_64& a) const {
  bitset<128> a0,a1,b0,b1,e0;
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

inline void Monom64_64::gcd(const Monom64_64& a, const Monom64_64& b) {
  bitset<128> a0,a1,b0,b1,e0;
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

inline void Monom64_64::lcm(const Monom64_64& a, const Monom64_64& b) {
  bitset<128> a0,a1,b0,b1,e0;
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

inline int Monom64_64::compare(const Monom64_64& a) const {
  if (total_degree<a.total_degree)
    return -1;
  else if (total_degree>a.total_degree)
    return 1;
  else {
    unsigned long *exp_long = (unsigned long*)&exp + 1;
    unsigned long *a_exp_long = (unsigned long*)&a.exp + 1;
    if (*exp_long<*a_exp_long)
      return 1;
    else if (*exp_long>*a_exp_long)
      return -1;
    else {
      exp_long--; a_exp_long--;
      if (*exp_long<*a_exp_long)
        return 1;
      else if (*exp_long>*a_exp_long)
        return -1;
      else
        return 0;
    }
  }
}

inline bool operator==(const Monom64_64 &a, const Monom64_64 &b){
  return a.exp==b.exp;
}

inline bool Monom64_64::isHG() const{
  bitset<128> a1 = exp & mRealization->mask1;
  if (a1==0)
    return false;
  else
    return true;
}

inline int Monom64_64::num_of_only_var() const {
  int i = -1;
  unsigned long exp_int = *(unsigned long*)&exp;
  if (exp_int)
    while(exp_int){
      exp_int = exp_int>>1;
      i++;
    }
  else{
    exp_int = *((unsigned long*)&exp + 1);
    i += 64;
    while(exp_int){
      exp_int = exp_int>>1;
      i++;
    }
  }
  return i/2;
}

inline bool Monom64_64::equality(const Monom64_64& a, int var) const {
  bitset<128> d = a.exp;
  d.set(2*var);
  return exp==d;
}
