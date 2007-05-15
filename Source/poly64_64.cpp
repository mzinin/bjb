#include <cstdarg>
#include <cstring>
#include "poly64_64.h"

PolyInterface64_64::PolyInterface64_64(MonomInterface64_64 *mInterface):
    mMonomInterface(mInterface){
}

Poly64_64* PolyInterface64_64::create() {
  return new Poly64_64(this);
}

Poly64_64::Poly64_64(PolyInterface64_64* r):
   mRealization(r), mHead() {
   len=0;
  //IASSERT(mRealization);
  }

Poly64_64::Poly64_64(const Poly64_64& a):
    mRealization(a.mRealization), mHead() {
  //IASSERT(mRealization);
  ConstIterator ia(a.mHead);
  Iterator i(mHead);
  while(ia) {
    i.insert(*ia);
    ia++;
    i++;
  }
  len = a.len;
  //IASSERTVALID(*this);
}

void Poly64_64::setOne() {
  Iterator i(mHead);
  i.clear();
  mHead = new Monom64_64(monomInterface());
  mHead->setZero();
  mHead->Next = NULL;
  len =1;
}

void Poly64_64::setZero() {
  Iterator i(mHead);
  i.clear();
  mHead = NULL;
  len = 0;
}

void Poly64_64::set(const Poly64_64& a) {
  Iterator i(mHead);
  i.clear();
  ConstIterator ia(a.mHead);
  while(ia) {
    i.insert(*ia);
    ++i;
    ++ia;
  }
  len = a.len;
  //IASSERTVALID(*this);
}

void Poly64_64::swap(Poly64_64 &a) {
  Monom64_64 *tmp;
  tmp = mHead;
  mHead = a.mHead;
  a.mHead = tmp;

  int itmp;
  itmp = len;
  len = a.len;
  a.len = itmp;
}

bool Poly64_64::isBinomial() const {
  if (len!=2) return false;
  if (degree_of_monom(0)!=2 || degree_of_monom(1)!=1) return false;
  Poly64_64 tmp = *this;
  Monom64_64 m0 = tmp.lm();
  tmp.rid_of_lm();
  Monom64_64 m1 = tmp.lm();
  if (!m0.divisibility(m1))
    return false;
  else
     m0.divide(m0,m1);
  if (!m0.divisibility(m1))
    return false;
  else
    return true;
}

int Poly64_64::degree() const {
  if (mHead)
    return mHead->degree();
  else
    return -100;
}

int Poly64_64::degree_of_monom(int i) const {
   ConstIterator cit(mHead);
   for (int j=0; j<i; j++)
     cit++;
   return cit->degree();
}

int Poly64_64::deg(int var){
  //IASSERT(0 <= var && var < monomInterface()->dimIndepend());
  ConstIterator i(mHead);
  int output = 0;
  while (i){
    if (output < i->deg(var)) output = i->deg(var);
    ++i;
  }
  return output;
}

void Poly64_64::rid_of_lm(){
  Iterator i(mHead);
  i.del();
  len--;
}

void Poly64_64::rid_of_lastm() {
  Iterator i(mHead);
  while (i->Next) i++;
  i.del();
  len--;
}

void Poly64_64::add(const Monom64_64& m){
  Iterator i(mHead);
  bool placed=false;
  while (i && !placed)
    switch (i->compare(m)) {
      case 1:
        i++;
	break;
      case -1:
        i.insert(m);
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
    i.insert(m);
    len++;
  }
}

void Poly64_64::add(Poly64_64& a) {
  //IASSERT(polyInterface() == a.polyInterface());
  Iterator i1(mHead);
  ConstIterator i2(a.mHead);
  while(i1 && i2){
    switch(i1->compare(*i2)) {
    case 1:
      i1++;
      break;
    case -1:
      i1.insert(*i2);
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
  }

  while(i2){
    i1.insert(*i2);
    i1++;
    i2++;
    len++;
  }
  //IASSERTVALID(*this);
}

void Poly64_64::mult(int var, unsigned deg) {
  if (isZero()) return; //ноль ни на что не умножаем
  Iterator i(mHead);

  while (i){
    i->prolong(var,deg);
    i++;
  }
}

void Poly64_64::mult(const Monom64_64& m) {
  if (isZero()) return; //ноль ни на что не умножаем
  Iterator i(mHead);

  while (i) {
    i->mult(m);
    i++;
  }
}

void Poly64_64::mult(const Poly64_64 &a) {
  Poly64_64 p(mRealization);
  ConstIterator ia(a.mHead);
  while(ia) {
    Poly64_64 tmp(*this);
    tmp.mult(*ia);
    p.add(tmp);
    ia++;
    //IASSERTVALID(*p);
  }
  swap(p);
  //IASSERTVALID(*this);
}

void Poly64_64::mult1(int var, unsigned deg) {
  if (isZero()) return; //ноль ни на что не умножаем
  Iterator i(mHead), itmp;
  i->prolong1(var); //старший моном просто умножается
  i++;
  bool moved;

  while (i)
    if (!i->deg(var)){
      i->prolong1(var);
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

void Poly64_64::mult1(const Monom64_64& m) {
  if (isZero()) return; //ноль ни на что не умножаем
  Iterator i(mHead), itmp, i_minus;
  i->mult1(m); //
  i++;
  bool moved;
  i_minus = begin();

  while (i) {
    i->mult1(m);
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

void Poly64_64::mult1(const Poly64_64 &a) {
  Poly64_64 p(mRealization);
  ConstIterator ia(a.mHead);
  while(ia) {
    Poly64_64 tmp(*this);
    tmp.mult1(*ia);
    p.add(tmp);
    ia++;
  }
  swap(p);
}

void Poly64_64::reduction(const Poly64_64 &a) {
  Monom64_64 *m2 = new Monom64_64(monomInterface());
  Poly64_64 *p;

  ConstIterator j(mHead);
  while (j)
    if (j->divisibility(a.lm())){
      m2->divide(*j, a.lm());
      p = new Poly64_64(a);
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
      m2->divide(*i, a.lm());
      p = new Poly64_64(a);
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

void Poly64_64::reduction1(const Poly64_64 &a) {
  Monom64_64 *m2 = new Monom64_64(monomInterface());
  Poly64_64 *p;

  Iterator j(mHead);
  while (j)
    if (j->divisibility(a.lm())){
      m2->divide(*j, a.lm());
      p = new Poly64_64(a);
      if (j->isHG()){
        j.del();
        p->rid_of_lm();
      }
      p->mult1(*m2);
      add(*p); delete p;
      j.mIt=&mHead;
    }
    else
      break;

  if (isZero())
    return;
  Iterator i(j);
  i++;

  while (i)
    if (i->divisibility(a.lm())){
      m2->divide(*i, a.lm());
      p = new Poly64_64(a);
      if (i->isHG()){
        i.del();
        p->rid_of_lm();
      }
      p->mult1(*m2);
      add(*p); delete p;
      i=j;
      i++;
    }
    else{
      i++;
      j++;
    }
}

void Poly64_64::head_reduction(const Poly64_64 &a) {
  Monom64_64 *m2 = new Monom64_64(monomInterface());
  Poly64_64 *p;

  ConstIterator j(mHead);
  while (j)
    if (j->divisibility(a.lm())){
      m2->divide(*j, a.lm());
      p = new Poly64_64(a);
      p->mult(*m2);
      add(*p); delete p;
      j.mConstIt=mHead;
    }
    else
      break;
}

std::ostream& operator<<(std::ostream& out, const Poly64_64& a) {
  if (a.isZero())
    out << '0';
  else {
    Poly64_64::ConstIterator i(a.begin());
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

void Poly64_64::additive(std::istream& in) {
  multiplicative(in);
  int op = (in >> std::ws).peek();
  Poly64_64 tmp(polyInterface());
  while(op == '+' || op == '-') {
    in.get();
    tmp.multiplicative(in);
    add(tmp);
    op = in.peek();
  }
}

void Poly64_64::multiplicative(std::istream& in) {
  unary(in);
  int op = (in >> std::ws).peek();
  Poly64_64 tmp(polyInterface());
  while(op == '*') {
    in.get();
    tmp.unary(in);
    mult(tmp);
    op = in.peek();
  }
}

void Poly64_64::unary(std::istream& in) {
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

void Poly64_64::power(std::istream& in) {
  bracket(in);
  int op = (in >> std::ws).peek();
  if (op == '^') {
    in.get();
    int deg=0;
    in >> std::ws >> deg;
    if (in.fail() || deg <= 0)
      IMESSAGE("expected 'degree > 0'");
    //pow(deg);
  }
}

void Poly64_64::bracket(std::istream& in) {
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
    Monom64_64 m(monomInterface());
    in >> m;
    if (!in.fail())
      mult(m);
    else {
      in.clear();
      if (in.get() != '1') { IMESSAGE("expected 'monomial'"); };
    }
    //IASSERTVALID(*this);
  }
}

std::istream& operator>>(std::istream& in, Poly64_64& a) {
  a.additive(in);
  return in;
}

bool operator==(const Poly64_64 &a, const Poly64_64 &b){
  if (a.polyInterface() != b.polyInterface())
    return false;
  if (a.len!=b.len)
    return false;

  Poly64_64::ConstIterator ia(a.mHead), ib(b.mHead);
  while (ia && ib){
    if ( *ia != *ib )
      return false;
    ++ia;
    ++ib;
  }
  return true;
}

void Poly64_64::assertValid(const char* fileName, int fileLine) const {
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
