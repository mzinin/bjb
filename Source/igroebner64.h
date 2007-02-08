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

public:
  IGBasis64();
  IGBasis64(vector<IMyPoly64*> set);
  ~IGBasis64() {}

  IMyPoly64* operator[](int num);
  int length();
  void ReduceSet(vector<IMyPoly64*> &set, int i);

  IMyPoly64* NormalForm(Triple64 &p);
  void InvolutiveBasisI();
  IMyMonomInterface64* monomInterface32() const {return mInterface_local;}
  IMyPolyInterface64*  polyInterface32()  const {return pInterface_local;}

  friend std::ostream& operator<<(std::ostream& out, IGBasis64& GBasis);
};
