#include <cstdarg>
#include <cstring>
#include "imypoly128.h"

IMyPolyInterface128::IMyPolyInterface128():
    mMonomInterface(NULL){
}

IMyPolyInterface128* IMyPolyInterface128::create(IMyMonomInterface128* monomInterface) {
  if (monomInterface == NULL)
    IERROR("error in IPolyInterface: monomInterface == NULL");
  IMyPolyInterface128* r = new IMyPolyInterface128;
  if (r)
    r->mMonomInterface = monomInterface;
  return r;
}

IMyPoly128* IMyPolyInterface128::create() {
  return new IMyPoly128(this);
}

IMyPoly128* IMyPolyInterface128::copy(const IMyPoly128& a) {
  //IASSERT(this == a.polyInterface());
  return new IMyPoly128(a);
}

IMyPoly128* IMyPolyInterface128::copy(const IMyPoly128& a, int var) {
  //IASSERT(this == a.polyInterface());
  return new IMyPoly128(a, var);
}

IMyPoly128::IMyPoly128(IMyPolyInterface128* r):
   mRealization(r),
   mHead() {
   len=0;
  //IASSERT(mRealization);
  }

IMyPoly128::IMyPoly128(const IMyPoly128& a):
    mRealization(a.mRealization),
    mHead() {
  //IASSERT(mRealization);
  ConstIterator ia(a.mHead);
  Iterator i(mHead);
  while(ia) {
    i.insert(*monomInterface()->copy(*ia));
    ia++;
    i++;
  }
  len = a.len;
  //IASSERTVALID(*this);
}

IMyPoly128::IMyPoly128(const IMyPoly128& a, int var):
    mRealization(a.mRealization),
    mHead() {
  //IASSERT(mRealization);
  ConstIterator ia(a.mHead);
  Iterator i(mHead);
  while(ia) {
    i.insert(*monomInterface()->copy(*ia));
    ia++;
    i++;
  }
  len =a.len;
  mult(var);
  //IASSERTVALID(*this);
}

IMyPoly128::IMyPoly128(const IMyPoly128& a, const IMyMonom128& m):
    mRealization(a.mRealization),
    mHead() {
  //IASSERT(mRealization);
  ConstIterator ia(a.mHead);
  Iterator i(mHead);
  while(ia) {
    i.insert(*monomInterface()->copy(*ia));
    ia++;
    i++;
  }
  len = a.len;
  mult(m);
  //IASSERTVALID(*this);
}

void IMyPoly128::setOne() {
  Iterator i(mHead);
  i.clear();
  mHead = monomInterface()->create();
  mHead->setZero();
  mHead->Next = NULL;
  len =1;
}

void IMyPoly128::setZero() {
  Iterator i(mHead);
  i.clear();
  mHead = NULL;
  len = 0;
}

void IMyPoly128::set(const IMyPoly128& a) {
  //IASSERT(polyInterface() == a.polyInterface());
  Iterator i(mHead);
  i.clear();
  ConstIterator ia(a.mHead);
  while(ia) {
    i.insert(*monomInterface()->copy(*ia));
    ++i;
    ++ia;
  }
  len = a.len;
  //IASSERTVALID(*this);
}

void IMyPoly128::swap(IMyPoly128 &a) {
  //IASSERT(polyInterface() == a.polyInterface());
  IMyMonom128 *tmp;
  tmp = mHead;
  mHead = a.mHead;
  a.mHead = tmp;

  int itmp;
  itmp = len;
  len = a.len;
  a.len = itmp;
}

int IMyPoly128::degree() const {
  if (mHead)
    return mHead->degree();
  else
    return -100;
}

int IMyPoly128::degree_of_monom(int i) const {
   ConstIterator cit(mHead);
   for (int j=0; j<i; j++)
     cit++;
   return cit->degree();
}

int IMyPoly128::deg(int var){
  //IASSERT(0 <= var && var < monomInterface()->dimIndepend());
  ConstIterator i(mHead);
  int output = 0;
  while (i && !output){
    if (output < i->deg(var)) output = i->deg(var);
    ++i;
  }
  return output;
}

void IMyPoly128::rid_of_lm(){
  Iterator i(mHead);
  i.del();
  len--;
}

void IMyPoly128::add(const IMyMonom128& m){
  Iterator i(mHead);
  bool placed=false;
  while (i && !placed)
    switch (i->compare(m)) {
      case 1:
        i++;
	break;
      case -1:
        i.insert(*monomInterface()->copy(m));
	placed = true;
	len++;
	break;
      case 0:
        i.del();
	placed = true;
	len--;
	break;
    }

  if (!placed){
    i.insert(*monomInterface()->copy(m));
    len++;
  }
}

void IMyPoly128::add(IMyPoly128& a) {
  //IASSERT(polyInterface() == a.polyInterface());
  Iterator i1(mHead);
  ConstIterator i2(a.mHead);
  while(i1 && i2)
    switch(i1->compare(*i2)) {
    case 1:
      i1++;
      break;
    case -1:
      i1.insert(*monomInterface()->copy(*i2));
      i2++;
      i1++;
      len++;
      break;
    case 0:
      i1.del();
      i2++;
      len--;
      break;
    }

  while(i2){
    i1.insert(*monomInterface()->copy(*i2));
    i1++;
    i2++;
    len++;
  }
  //IASSERTVALID(*this);
}

void IMyPoly128::mult(int var) {
  if (isZero()) return; //
  Iterator i(mHead), itmp;
  i->prolong(var); //
  i++;
  bool moved;

  while (i)
    if (!i->deg(var)){
      i->prolong(var);
      moved = false;
      itmp = begin(); //
      while (itmp!=i && !moved) {
        moved = false;
	switch(itmp->compare(*i)){
        case 1:  //
          itmp++;
	  break;
        case -1: //
          itmp.insert(i.get());
	  moved = true;
	  break;
        case 0:  //
          i.del();
          len-=2;
	  if (itmp->Next!=*i.mIt)
            itmp.del();
          else{
            itmp.del();
            i = itmp;
          }
	  moved = true;
	  break;
        }
      }
      if (!moved) i++;
    }
    else
      i++;

}

void IMyPoly128::mult(int var, unsigned deg) {
  if (var>0) mult(var);
  //IASSERTVALID(*this);
}

void IMyPoly128::mult(const IMyMonom128& m) {
  if (isZero()) return; //
  Iterator i(mHead), itmp, i_minus;
  i->mult(m); //
  i++;
  bool moved;
  i_minus = begin();

  while (i) {
    i->mult(m);
    if (i!=i_minus)
      switch (i->compare(*i_minus)){
      case -1:
        i++;
	i_minus++;
	break;
      case 0:
        i.del();
	i_minus.del();
	len-=2;
	i = i_minus;
	if (*i.mIt!=mHead){
	  i_minus=begin();
	  while (i_minus->Next!=*i.mIt)
	    i_minus++;
	}
        break;

      case 1:
      moved = false;
      itmp = begin(); //
      while (itmp!=i && !moved) {
        moved = false;
	switch(itmp->compare(*i)) {
        case 1:  //
          itmp++;
	  break;
        case -1: //
	  if (itmp!=i_minus)
            itmp.insert(i.get());
	  else{
	    i_minus.insert(i.get());
	    i_minus++;
	  }
	  moved = true;
	  break;
        case 0:  //
          i.del();
	  len-=2;
	  if (itmp->Next!=*i_minus.mIt)
            itmp.del();
          else{
            itmp.del();
            i_minus = itmp;
          }
	  moved = true;
	  break;
        }
      }
      if (!moved) i++;
      break;

    }
    else
     i++;
  }
}

void IMyPoly128::mult(const IMyPoly128 &a) {
  //IASSERT(polyInterface() == a.polyInterface());
  IMyPoly128 *p = new IMyPoly128(polyInterface());
  ConstIt ia(a.mHead);
  while(ia) {
    IMyPoly128 *tmp = new IMyPoly128(*this, *ia);
    p->add(*tmp);
    delete tmp;
    ia++;
    //IASSERTVALID(*p);
  }
  swap(*p);
  delete p;
  //IASSERTVALID(*this);
}

void IMyPoly128::pow(unsigned deg) {
//
}

void IMyPoly128::reduction(const IMyPoly128 &a) {
  //IASSERT(polyInterface() == a.polyInterface());
  IMyMonom128 *m2(monomInterface()->create());
  IMyPoly128 *p;

  ConstIterator j(mHead);
  while (j)
    if (j->divisibility(a.lm())){
      m2->divide(*j, a.lm());
      p = new IMyPoly128(a);
      p->mult(*m2);
      add(*p); delete p;
      j.mConstIt=mHead;
    }
    else
      break;

  if (isZero())
    return;
  ConstIterator i(j);
  i++;

  while (i)
    if (i->divisibility(a.lm())){
      m2->divide1(*i, a.lm());
      p = new IMyPoly128(a);
      p->mult(*m2);
      add(*p); delete p;
      i=j;
      i++;
    }
    else{
      i++;
      j++;
    }
}

void IMyPoly128::reduction1(const IMyPoly128 &a) {
  IMyMonom128 *m2(monomInterface()->create());
  IMyPoly128 *p;

  ConstIterator j(mHead);
  while (j)
    if (j->divisibility(a.lm())){
      m2->divide(*j, a.lm());
      p = new IMyPoly128(a);
      p->mult(*m2);
      add(*p); delete p;
      j.mConstIt=mHead;
    }
    else
      break;
}

IMyPoly128::ConstIterator IMyPoly128::begin() const {
  //return ConstIterator(mHead);
  return mHead;
}

IMyPoly128::Iterator IMyPoly128::begin() {
  //return Iterator(mHead);
  return mHead;
}

std::ostream& operator<<(std::ostream& out, const IMyPoly128& a) {
  if (a.isZero())
    out << '0';
  else {
    IMyPoly128::ConstIterator i(a.begin());
    if ((*i).degree())
      out << *i;
    else
      out << '1';
    ++i;
    while(i) {
      out << " + ";
      if ((*i).degree())
        out << *i;
      else
        out << '1';
      ++i;
    }
  }
  return out;
}

void IMyPoly128::additive(std::istream& in) {
  multiplicative(in);
  int op = (in >> std::ws).peek();
  IMyPoly128 tmp(polyInterface());
  while(op == '+' || op == '-') {
    in.get();
    tmp.multiplicative(in);
    add(tmp);
    op = in.peek();
  }
}

void IMyPoly128::multiplicative(std::istream& in) {
  unary(in);
  int op = (in >> std::ws).peek();
  IMyPoly128 tmp(polyInterface());
  while(op == '*') {
    in.get();
    tmp.unary(in);
    mult(tmp);
    op = in.peek();
  }
}

void IMyPoly128::unary(std::istream& in) {
  int ch = (in >> std::ws).peek();
  if (ch != '+' && ch != '-')
    power(in);
  else {
    int k = 1;
    do {
      ch = in.get();
      if (ch == '-')
        k *= -1;
      ch = (in >> std::ws).peek();
    } while (ch == '+' || ch == '-');
    power(in);
  }
}

void IMyPoly128::power(std::istream& in) {
  bracket(in);
  int op = (in >> std::ws).peek();
  if (op == '^') {
    in.get();
    int deg=0;
    in >> std::ws >> deg;
    if (in.fail() || deg <= 0)
      IMESSAGE("expected 'degree > 0'");
    pow(deg);
  }
}

void IMyPoly128::bracket(std::istream& in) {
  int op = (in >> std::ws).peek();
  if (op == '(') {
    in.get();
    additive(in);
    if (in.get() != ')') {
      IMESSAGE("expected ')'");
    }
  }
  else {
    setOne();
    IMyMonom128 *m(monomInterface()->create());
    in >> (*m);
    if (!in.fail())
      mult(*m);
    else {
      in.clear();
      if (in.get() != '1') { IMESSAGE("expected 'monomial'"); };
    }
    delete m;
    //IASSERTVALID(*this);
  }
}

std::istream& operator>>(std::istream& in, IMyPoly128& a) {
  a.additive(in);
  return in;
}

bool operator==(const IMyPoly128 &a, const IMyPoly128 &b){
  if (a.polyInterface() != b.polyInterface())
    return false;
  if (a.len!=b.len)
    return false;

  IMyPoly128::ConstIterator ia(a.mHead), ib(b.mHead);
  while (ia && ib){
    if ( *ia != *ib )
      return false;
    ++ia;
    ++ib;
  }
  if (ia||ib)
    return false;
  return true;
}

void IMyPoly128::RawCopy(IMyPoly128 &a){
  setZero();
  IMyPoly128 *tmp = polyInterface()->create();
  int dim = a.polyInterface()->monomInterface()->dimIndepend(),var,len=a.length(),j,k;

  for (j=len-1;j>=0;j--){
    ConstIterator i(a.mHead);
    tmp->setOne();
    for (k=0;k<j;k++) i++;

    for (var=0;var<dim;var++)
      if ((*i).deg(var))
        tmp->mult(var);

    add(*tmp);
  }
}

void IMyPoly128::assertValid(const char* fileName, int fileLine) const {
  if (mHead) {
    ConstIterator i(mHead);
    IASSERT2(fileName, fileLine, i);
    ConstIterator prev(i);
    ++i;
    while(i) {
      IASSERT2(fileName, fileLine, i);
      IASSERT2(fileName, fileLine, (*prev).compare(*i) > 0);
      prev = i;
      ++i;
    }
  }
}
