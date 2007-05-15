#include <cstdarg>
#include <cmath>
#include "util.h"
#include "monom32_64.h"

MonomInterface32_64::MonomInterface32_64(Variables* independ){
  mDimIndepend = independ->dim();
  mIndepend = independ;
  mask0 = 0;
  mask1 = 0;
  for (int i=0; i<32; i++){
    mask0.set(2*i);
    mask1.set(2*i+1);
  }
}

Monom32_64 MonomInterface32_64::create() {
  return Monom32_64(this);
}

Monom32_64 MonomInterface32_64::copy(const Monom32_64 &a) {
  return Monom32_64(a);
}

Monom32_64::Monom32_64(MonomInterface32_64* r):
  mRealization(r),mDimIndepend(r->dimIndepend()),exp(),Next(NULL){
      IASSERT(mRealization);
      total_degree = 0;
  }

Monom32_64::Monom32_64(const Monom32_64& a):
  mRealization(a.mRealization),mDimIndepend(a.mDimIndepend),exp(),Next(NULL){
    total_degree = a.total_degree;
    exp = a.exp;
  }

Monom32_64 Monom32_64::copy(){
  Monom32_64 r = Monom32_64(mRealization);
  r.total_degree = total_degree;
  r.exp = exp;
  return r;
}

void Monom32_64::set(const Monom32_64& a) {
  total_degree = a.total_degree;
  exp = a.exp;
}

void Monom32_64::swap(Monom32_64& a) {
  unsigned t;
  t = total_degree;
  total_degree = a.total_degree;
  a.total_degree = t;

  bitset<64> d = exp;
  exp = a.exp;
  a.exp = d;
}

void Monom32_64::div(int var) {
  if ( exp.test(2*var) || exp.test(2*var+1) ){
    unsigned long t = 1;
    t = t << 2*var;
    exp = *(unsigned long*)&exp - t;
    total_degree--;
  }
  else
   IERROR("Monom can't be divided by variable");
}

int Monom32_64::compare(const Monom32_64& a, const Monom32_64& b) const {
  Monom32_64 tmp(a);
  tmp.mult(b);
  return compare(tmp);
}

std::istream& operator>>(std::istream& in, Monom32_64& a) {
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

std::ostream& operator<<(std::ostream& out, const Monom32_64& a) {
  if (a.degree() == 0)
    out << '1';
  else {
    int i = 0;
    Variables::ConstIterator j(a.independ()->begin());
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
