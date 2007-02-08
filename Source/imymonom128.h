#include <iostream>
#include <bitset>
#include "ivariables.h"

using namespace std;

class IMyMonom128;
class IMyMonomInterface128 {
  friend class IMyMonom;

protected:
  int           mDimIndepend;
  IVariables*   mIndepend;

public:
  IMyMonomInterface128(IVariables* independ);
  static IMyMonomInterface128* create(IVariables* independ);
  ~IMyMonomInterface128() {}

  int dimIndepend() const { return mDimIndepend; }
  const IVariables* independ() const { return mIndepend; }

  IMyMonom128* create();
  IMyMonom128* copy(const IMyMonom128& a);
};

class IMyMonom128 {
  friend class IMyMonomInterface128;

protected:
  IMyMonom128(IMyMonomInterface128* r);
  IMyMonomInterface128* const mRealization;
  unsigned total_degree;
  int mDimIndepend,mWord;
  bitset<128> exp;

public:
  IMyMonom128 *Next;
  IMyMonom128* copy();

  IMyMonom128(const IMyMonom128& a);
  ~IMyMonom128() {Next =NULL;};

  IMyMonomInterface128* monomInterface() const { return mRealization; }
  int dimIndepend() const { return mDimIndepend; }
  const IVariables* independ() const { return mRealization->independ(); }

  unsigned deg(int var) const;
  unsigned operator[](int var) const { return deg(var); }
  unsigned degree() const;

  void set(const IMyMonom128& a);
  void operator=(const IMyMonom128& a) { set(a); }

  void setZero();
  void swap(IMyMonom128& a);
  void prolong(int var);
  void prolong(int var, unsigned deg);
  void div(int var);

  void mult(const IMyMonom128& a);
  void mult1(const IMyMonom128& a);
  void mult(const IMyMonom128& a, const IMyMonom128& b);

  bool divisibility(const IMyMonom128& a) const;
  bool divisibilityTrue(const IMyMonom128& a) const;
  void divide(const IMyMonom128& a, const IMyMonom128& b);
  void divide1(const IMyMonom128& a, const IMyMonom128& b);

  unsigned gcd(const IMyMonom128& a) const;
  unsigned lcm(const IMyMonom128& a) const;
  void gcd(const IMyMonom128& a, const IMyMonom128& b);
  void lcm(const IMyMonom128& a, const IMyMonom128& b);

  bool equality(const IMyMonom128& a, int var, unsigned degree=1) const;
  int compare(const IMyMonom128& a) const;
  int compare(const IMyMonom128& a, const IMyMonom128& b) const;

  friend std::istream& operator>>(std::istream& in, IMyMonom128& a);
  friend std::ostream& operator<<(std::ostream& out, const IMyMonom128& a);
  friend bool operator==(const IMyMonom128 &a, const IMyMonom128 &b);
  friend bool operator!=(const IMyMonom128 &a, const IMyMonom128 &b) { return !(a==b); };

  //void assertValid(const char* fileName, int fileLine) const;
};

inline unsigned IMyMonom128::deg(int var) const {
  return exp.test(var);
}

inline unsigned IMyMonom128::degree() const {
  return total_degree;
}

inline void IMyMonom128::setZero() {
  total_degree=0;
  //*exp_int=0;
  exp = 0;
}

inline void IMyMonom128::prolong(int var) {
  if ( !exp.test(var) ) {
    exp.set(var);
    total_degree++;
  };
}

inline void IMyMonom128::mult(const IMyMonom128& a) {
  exp|=a.exp;
  total_degree=exp.count();
}

inline void IMyMonom128::mult1(const IMyMonom128& a) {
  exp|=a.exp;
}

inline void IMyMonom128::mult(const IMyMonom128& a, const IMyMonom128& b) {
  exp = a.exp | b.exp;
  total_degree = exp.count();
}

inline bool IMyMonom128::divisibility(const IMyMonom128& a) const{
  bitset<128> d;
  d = exp ^ a.exp;
  d&=a.exp;
  if(d!=0)
    return false;
  else
    return true;
}

inline bool IMyMonom128::divisibilityTrue(const IMyMonom128& a) const {
  bitset<128> d;
  d = exp ^ a.exp;
  if (d==0)
    return false;
  else{
    d&=a.exp;
    if(d!=0)
      return false;
    else
      return true;
  }
}

inline void IMyMonom128::divide(const IMyMonom128& a, const IMyMonom128& b) {
  total_degree = a.total_degree - b.total_degree;
  exp = a.exp ^ b.exp;
}

inline void IMyMonom128::divide1(const IMyMonom128& a, const IMyMonom128& b) {
  exp = a.exp ^ b.exp;
}

inline unsigned IMyMonom128::gcd(const IMyMonom128& a) const {
  bitset<128> d;
  d = exp & a.exp;
  return d.count();
}

inline unsigned IMyMonom128::lcm(const IMyMonom128& a) const {
  bitset<128> d;
  d = exp | a.exp;
  return d.count();
}

inline void IMyMonom128::gcd(const IMyMonom128& a, const IMyMonom128& b) {
  exp = a.exp & b.exp;
  total_degree = exp.count();
}

inline void IMyMonom128::lcm(const IMyMonom128& a, const IMyMonom128& b) {
  exp = a.exp | b.exp;
  total_degree = exp.count();
}

inline int IMyMonom128::compare(const IMyMonom128& a) const {
  if (total_degree<a.total_degree)
    return -1;
  else if (total_degree>a.total_degree)
    return 1;
  else {
    unsigned long *exp_int = (unsigned long*)&exp + 1;
    unsigned long *exp_int_a = (unsigned long*)&a.exp + 1;
    if (*exp_int<*exp_int_a)
      return 1;
    else if (*exp_int>*exp_int_a)
      return -1;
    else {
      exp_int--; exp_int_a--;
      if (*exp_int<*exp_int_a)
        return 1;
      else if (*exp_int>*exp_int_a)
        return -1;
      else
        return 0;
    }
  }
}

inline bool operator==(const IMyMonom128 &a, const IMyMonom128 &b){
  return a.exp==b.exp;
}
