#include <cstdarg>
#include <cmath>
#include "iutil.h"
#include "imymonom128.h"

IMyMonomInterface128::IMyMonomInterface128(IVariables* independ){
  mDimIndepend = independ->dim();
  mIndepend = independ;
}

IMyMonomInterface128* IMyMonomInterface128::create(IVariables* independ) {
  IMyMonomInterface128* r = new IMyMonomInterface128(independ);
  return r;
}

IMyMonom128* IMyMonomInterface128::create() {
  return new IMyMonom128(this);
}

IMyMonom128* IMyMonomInterface128::copy(const IMyMonom128 &a) {
  IMyMonom128* r;
  if (this == a.monomInterface())
    r = new IMyMonom128(a);

  return r;
}

IMyMonom128::IMyMonom128(IMyMonomInterface128* r):
  mRealization(r),
  mDimIndepend(r->dimIndepend()),mWord((mDimIndepend-1)/32),exp(),Next(NULL){
      IASSERT(mRealization);
      total_degree = 0;
  }

IMyMonom128::IMyMonom128(const IMyMonom128& a):
  mRealization(a.mRealization),
  mDimIndepend(a.mDimIndepend),mWord((mDimIndepend-1)/32),exp(),Next(NULL){
    total_degree = a.total_degree;
    exp = a.exp;
  }

IMyMonom128* IMyMonom128::copy(){
  IMyMonom128* r = new IMyMonom128(mRealization);
  r->total_degree = total_degree;
  r->exp = exp;
  return r;
}

void IMyMonom128::set(const IMyMonom128& a) {
  if (monomInterface() == a.monomInterface()) {
    total_degree = a.total_degree;
    exp = a.exp;
  }
}

void IMyMonom128::swap(IMyMonom128& a) {
  unsigned t;
  t = total_degree;
  total_degree = a.total_degree;
  a.total_degree = t;

  bitset<128> d = exp;
  exp = a.exp;
  a.exp = d;
}

void IMyMonom128::prolong(int var, unsigned deg) {
  if (deg>0) prolong(var);
}

void IMyMonom128::div(int var) {
  if ( exp.test(var) ){
    exp.reset(var);
    total_degree--;
  }
  else
   IERROR("Monom can't be divided by variable");
}

bool IMyMonom128::equality(const IMyMonom128& a, int var, unsigned degree) const {
  bitset<128> d=1;
  d=d<<var;
  d|=a.exp;
  return exp==d;
}

int IMyMonom128::compare(const IMyMonom128& a, const IMyMonom128& b) const {
  IMyMonom128 tmp(a);
  tmp.mult(b);
  return compare(tmp);
}

std::istream& operator>>(std::istream& in, IMyMonom128& a) {
  std::streampos posbeg = in.tellg();
  int var = a.independ()->read(in);
  if (var < 0) {
    in.clear();
    in.setstate(std::ios::failbit);
  }
  else {
    a.setZero();
    int deg;
    do {
      deg = 1;
      std::streampos posbeg = in.tellg();
      if ((in >> std::ws).peek() == '^') {
        in.get();
        in >> std::ws >> deg;
        if (in.fail() || deg < 0) {
          in.setstate(std::ios::failbit);
          IMESSAGE("expected 'degree >= 0'");
        }
      }
      a.prolong(var,deg);

      posbeg = in.tellg();
      if (in.peek() != '*')
        var = -1;
      else {
        in.get();
        var = a.independ()->read(in);
        if (var < 0) {
          in.clear();
          in.seekg(posbeg);
        }
      }
    } while(var >= 0);
    if (in.eof() &&
        deg >= 0)
      in.clear();
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const IMyMonom128& a) {
  if (a.degree() == 0)
    out << '1';
  else {
    int i = 0;
    IVariables::ConstIterator j(a.independ()->begin());
    while(a.deg(i) == 0) {
      ++i;
      ++j;
    }
    out << *j;
    if (a.deg(i) > 1)
      out << '^' << a[i];
    ++i;
    ++j;
    while(j!=a.independ()->end()){
      if (a.deg(i)) {
        out << '*' << *j;
        if (a.deg(i) > 1)
          out << '^' << a[i];
      }
      ++i;
      ++j;
    }
  }
  return out;
}
