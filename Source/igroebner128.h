#include <vector>
#include <algorithm>
#include "janettree128.h"

class IGBasis128{
protected:
  vector<IMyPoly128*> gBasis;
  vector<Triple128*> qSet, tSet;
  JanetTree128 jTree;
  IVariables vars;
  IMyMonomInterface128* mInterface_local;
  IMyPolyInterface128* pInterface_local;

public:
  IGBasis128();
  IGBasis128(vector<IMyPoly128*> set);
  ~IGBasis128() {}

  IMyPoly128* operator[](int num);
  int length();
  void ReduceSet(vector<IMyPoly128*> &set, int i);

  IMyPoly128* NormalForm(Triple128 &p);
  void InvolutiveBasisI();
  IMyMonomInterface128* monomInterface32() const {return mInterface_local;}
  IMyPolyInterface128*  polyInterface32()  const {return pInterface_local;}

  friend std::ostream& operator<<(std::ostream& out, IGBasis128& GBasis);
};
