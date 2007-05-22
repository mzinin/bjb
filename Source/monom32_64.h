#include <iostream>
#include <bitset>
#include "variables.h"

using namespace std;

class Monom32_64;
class MonomInterface32_64 {
  friend class Monom32_64;

protected:
  int          mDimIndepend,mDword;
  Variables*   mIndepend;
  bitset<64>   mask0, mask1;

public:
  MonomInterface32_64(Variables* independ);
  ~MonomInterface32_64() {}

  int dimIndepend() const { return mDimIndepend; }
  int dword() const { return mDword; }
  const Variables* independ() const { return mIndepend; }

  Monom32_64 create();
  Monom32_64 copy(const Monom32_64& a);
};

class Monom32_64 {
  friend class MonomInterface32_64;

protected:
  MonomInterface32_64* const mRealization;
  unsigned total_degree;
  int mDimIndepend, mDword;
  unsigned long *exp;

public:
  Monom32_64 *Next;

  Monom32_64(MonomInterface32_64* r);
  Monom32_64(const Monom32_64& a);
  ~Monom32_64() {delete [] exp; Next = NULL;};
//вопрос - зачем эта функция и не оптимизировать ли ее? см. lm() в полиноме
  Monom32_64 copy();

  MonomInterface32_64* monomInterface() const { return mRealization; }
  int dimIndepend() const { return mDimIndepend; }
  int dword() const { return mDword; }
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
  unsigned char *d = (unsigned char*)exp;
  d+=var;
  return (*d);
}

inline unsigned Monom32_64::degree() const {
  return total_degree;
}

inline void Monom32_64::setZero() {
  total_degree = 0;
  for (int i=0; i<mDword; i++)
    exp[i] = 0;
}

inline void Monom32_64::prolong(int var, int deg) {
  unsigned char *d = (unsigned char*)exp;
  d += var;
  (*d) += deg;
  total_degree += deg;
}

inline void Monom32_64::mult(const Monom32_64& a) {
  for (int i=0; i<mDword; i++)
    exp[i] += a.exp[i];
  total_degree+=a.total_degree;
}

inline void Monom32_64::mult(const Monom32_64& a, const Monom32_64& b) {
  for (int i=0; i<mDword; i++)
    exp[i] = a.exp[i] + b.exp[i];
  total_degree = a.total_degree + b.total_degree;
}

inline bool Monom32_64::divisibility(const Monom32_64& a) const{
  if (total_degree < a.total_degree)
    return false;
  else{
    unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp;
    for (int i=0; i<mDimIndepend; i++)
      if (*md < *mad)
        return false;
      else{
        md++;
        mad++;
      }
    return true;
  }
}

inline bool Monom32_64::divisibilityTrue(const Monom32_64& a) const {
  if (total_degree <= a.total_degree)
    return false;
  else{
    unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp;
    for (int i=0; i<mDimIndepend; i++)
      if (*md < *mad)
        return false;
      else{
        md++;
        mad++;
      }
    return true;
  }
}

inline void Monom32_64::divide(const Monom32_64& a, const Monom32_64& b) {
  for (int i=0; i<mDword; i++)
    exp[i] = a.exp[i] - b.exp[i];
  total_degree = a.total_degree - b.total_degree;
}

inline unsigned Monom32_64::gcd(const Monom32_64& a) const {
  unsigned gcd = 0;
  unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp;
  for (int i=0; i<mDimIndepend; i++){
    gcd += min(*md, *mad);
    md++;
    mad++;
  }
  return gcd;
}

inline unsigned Monom32_64::lcm(const Monom32_64& a) const {
  unsigned lcm = 0;
  unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp;
  for (int i=0; i<mDimIndepend; i++){
    lcm += max(*md, *mad);
    md++;
    mad++;
  }
  return lcm;
}

inline void Monom32_64::gcd(const Monom32_64& a, const Monom32_64& b) {
  unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp, *mbd = (unsigned char*)b.exp;
  total_degree = 0;
  for (int i=0; i<mDimIndepend; i++){
    (*md) = min(*mad, *mbd);
    total_degree += *md;
    md++;
    mad++;
    mbd++;
  }
}

inline void Monom32_64::lcm(const Monom32_64& a, const Monom32_64& b) {
  unsigned char *md = (unsigned char*)exp, *mad = (unsigned char*)a.exp, *mbd = (unsigned char*)b.exp;
  total_degree = 0;
  for (int i=0; i<mDimIndepend; i++){
    (*md) = max(*mad, *mbd);
    total_degree += *md;
    md++;
    mad++;
    mbd++;
  }
}

inline int Monom32_64::compare(const Monom32_64& a) const {
  if (total_degree<a.total_degree)
    return -1;
  else if (total_degree>a.total_degree)
    return 1;
  else {
    for (int i=mDword-1; i>=0; i--)
      if (exp[i] < a.exp[i])
        return 1;
      else if (exp[i] > a.exp[i])
        return -1;

    return 0;
  }
}

inline bool operator==(const Monom32_64 &a, const Monom32_64 &b){
  for (int i=0; i<a.mDword; i++)
    if (a.exp[i]!=b.exp[i])
      return false;
  return true;
}