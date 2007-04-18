#include <vector>
#include <algorithm>
#include "janettree64.h"

class IGBasis64{
protected:
  vector<IMyPoly64*> gBasis;
  vector<Triple64*> qSet, tSet;
  JanetTree64 jTree;
  IVariables vars;
  IMyMonomInterface64* mInterface_local;
  IMyPolyInterface64* pInterface_local;
  int Dim;
  bitset<64> binomials_in_tset;

  void InvolutiveBasis();
  void ReduceSet(vector<IMyPoly64*> &set, int i);

public:
  IGBasis64();
  IGBasis64(vector<IMyPoly64*> set);
  ~IGBasis64() {}

  IMyPoly64* operator[](int num);
  int length();

  IMyPoly64* NormalForm(Triple64 &p);
  IMyMonomInterface64* monomInterface64() const {return mInterface_local;}
  IMyPolyInterface64*  polyInterface64()  const {return pInterface_local;}

  friend std::ostream& operator<<(std::ostream& out, IGBasis64& GBasis);
};
