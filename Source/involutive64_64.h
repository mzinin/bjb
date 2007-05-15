#include <vector>
#include <algorithm>
#include "janettree64_64.h"

class GBasis64_64{
protected:
  vector<Poly64_64*> gBasis;
  vector<Triple64_64*> qSet, tSet;
  JanetTree64_64 jTree;
  Variables vars;
  MonomInterface64_64* mInterface_local;
  PolyInterface64_64* pInterface_local;
  int Dim;

  void InvolutiveBasis();
  void ReduceSet(vector<Poly64_64*> &set/*, int i*/);

public:
  GBasis64_64();
  GBasis64_64(vector<Poly64_64*> set);
  ~GBasis64_64() {}

  Poly64_64* operator[](int num);
  int length();

  Poly64_64* NormalForm(Triple64_64 &p);
  MonomInterface64_64* monomInterface32() const {return mInterface_local;}
  PolyInterface64_64*  polyInterface32()  const {return pInterface_local;}

  friend std::ostream& operator<<(std::ostream& out, GBasis64_64& GBasis);
};
