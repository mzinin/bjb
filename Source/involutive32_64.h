#include <vector>
#include <algorithm>
#include "janettree32_64.h"

class GBasis32_64{
protected:
  vector<Poly32_64*> gBasis;
  vector<Triple32_64*> qSet, tSet;
  JanetTree32_64 jTree;
  Variables vars;
  MonomInterface32_64* mInterface_local;
  PolyInterface32_64* pInterface_local;
  int Dim;

  void InvolutiveBasis();
  void ReduceSet(vector<Poly32_64*> &set/*, int i*/);

public:
  GBasis32_64();
  GBasis32_64(vector<Poly32_64*> set);
  ~GBasis32_64() {}

  Poly32_64* operator[](int num);
  int length();

  Poly32_64* NormalForm(Triple32_64 &p);
  MonomInterface32_64* monomInterface32() const {return mInterface_local;}
  PolyInterface32_64*  polyInterface32()  const {return pInterface_local;}

  friend std::ostream& operator<<(std::ostream& out, GBasis32_64& GBasis);
};
