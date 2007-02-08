#include "janettree128.h"

void JanetTree128::Iterator::del() {
  Node* tmp = *i;
  *i = tmp->mNextDeg;
  delete tmp;
}

void JanetTree128::Iterator::build(int d, int var, Triple128 *trpl) {
  //IASSERT(d >= trpl->poly->lm()[var]);
  Node* r =  new JanetTree128::Node(trpl->poly->lm()[var]);
  Node* j = r;
  while(d > trpl->poly->lm()[var]) {
    d -= trpl->poly->lm()[var];
    var++;
    j->mNextVar = new JanetTree128::Node(trpl->poly->lm()[var]);
    j = j->mNextVar;
  }
  j->mTriple = trpl;

  //IASSERT(!*i || r->mDeg < (*i)->mDeg);
  r->mNextDeg = *i;
  *i = r;
}

void JanetTree128::Iterator::clear() {
  while (nextDeg()) {
    //IASSERT(nextVar());
    JanetTree128::Iterator((*i)->mNextVar).clear();
    del();
  }
  del();
}

JanetTree128::~JanetTree128() {
  clear();
  delete mRoot;
}

Triple128* JanetTree128::find(const IMyMonom128& m) const {
  Triple128* trpl = NULL;

  if (mRoot) {
    ConstIterator j(mRoot);
    unsigned d = m.degree();
    int var = 0;
    do {
      while(j.degree() < m[var] && j.isNextDeg())
        j.step_deg();

      if (j.degree() > m[var])
        break;
      else if (j.isNextVar()) {
        d -= m[var];
        if (d == 0)
          break;
        var++;
        j.step_var();
      }
      else {
        trpl = j.trpl();
        //IASSERT(m.divisibility(trpl->poly->lm()));
        break;
      }
    } while(true);
  }
  return trpl;
}

void JanetTree128::del(Triple128 *trpl) {
  //IASSERT(find(trpl->poly->lm()) != NULL);
  Iterator j(mRoot);
  //
  int var=0, vet=0;
  if (j.isNextDeg() && j.isNextVar()) vet++;

  do {
    while(j.degree() < trpl->poly->lm()[var]){
      j.step_deg();
      if (j.isNextDeg() && j.isNextVar()) vet++;
    }

    //IASSERT(j.degree()==trpl->poly->lm()[var]);
    if (j.nextVar()) {
      var++;
      j.step_var();
      if (j.isNextDeg() && j.isNextVar()) vet++;
    }
    else {
      //IASSERT(j.trpl()==trpl);
      break;
    }
  } while(true);

  //
  j = mRoot;
  var=0;
  bool varDirection=false;

  if (j.isNextDeg() && j.isNextVar()) vet--;
  if (vet==0)
    if (j.degree() < trpl->poly->lm()[var])
      j.step_deg();
    else
      varDirection=true;

  while (vet>0) {
    while(j.degree() < trpl->poly->lm()[var] && vet>0){
      j.step_deg();
      if (j.isNextDeg() && j.isNextVar()) vet--;
    }
    if (vet==0)
      if (j.degree() < trpl->poly->lm()[var]){
        j.step_deg();
        break;
      }
      else{
        varDirection=true;
        break;
      }

    var++;
    j.step_var();
    if (j.isNextDeg() && j.isNextVar()) vet--;
    if (vet==0){
      if (j.degree() < trpl->poly->lm()[var]){
        j.step_deg();
        break;
      }
      else{
        varDirection=true;
        break;
      }
    }
  }
  if (varDirection){
    Iterator j1 = j;
    j1.step_var();
    j1.clear();
    j.del();
  }
  else
    j.clear();
  //IASSERT(find(trpl->poly->lm()) != trpl);
}

void JanetTree128::insert(Triple128* trpl) {
  //IASSERT(trpl != NULL);
  //IASSERT(find(trpl->poly->lm()) == NULL);

  unsigned d = trpl->poly->lm().degree();
  JanetTree128::Iterator j(mRoot);
  if (mRoot == NULL)
    j.build(d, 0, trpl);
  else {
    int var = 0;
    do {
      while(j.degree() < trpl->poly->lm()[var] && j.isNextDeg())
        j.step_deg();

      if (j.degree() > trpl->poly->lm()[var]) {
        j.build(d, var, trpl);
        break;
      }
      else if (j.degree() == trpl->poly->lm()[var]) {
        d -= trpl->poly->lm()[var];
        var++;
        j.step_var();
      }
      else {
        j.step_deg();
        j.build(d, var, trpl);
        break;
      }
    } while(true);
  }
  //IASSERT(find(trpl->poly->lm()) != NULL);
}

void JanetTree128::update(Triple128 *trpl, vector<Triple128*> &set) {
  //IASSERT(trpl != NULL); IASSERT(trpl->poly != NULL);
  //IASSERT(find(trpl->poly->lm()) == trpl);

  JanetTree128::ConstIterator j(mRoot);
  int var = 0;
  do {
    while(j.degree() < trpl->poly->lm()[var]) {
      //IASSERT(j.isNextDeg());
      j.step_deg();
    }
    //IASSERT(j.degree() == trpl->poly->lm()[var]);

    if (j.isNextDeg()){
      if (!trpl->nmp.test(var)){
        IMyPoly128* tmp = trpl->poly->polyInterface()->copy(*trpl->poly);
        tmp->mult(var);
        set.push_back(new Triple128(tmp, trpl->anc, trpl, 0, true));
        trpl->nmp.set(var);
      }
    }
    else
      trpl->nmp.reset(var);

    ++var;
    if (j.isNextVar())
        j.step_var();
    else {
      //IASSERT(trpl == j.trpl());
      break;
    }
  } while(true);

  for (int i=var+1;i<trpl->poly->monomInterface()->dimIndepend();i++)
    trpl->nmp.reset(i);
}

void JanetTree128::clear() {
  if (mRoot) {
    JanetTree128::Iterator j(mRoot);
    j.clear();
  }
}

bitset<128> JanetTree128::nmulti(Triple128 *trpl){
  IASSERT(trpl->poly != NULL);
  IASSERT(find(trpl->poly->lm()) == trpl);

  JanetTree128::ConstIterator j(mRoot);
  bitset<128> out = 0;
  int var = 0;
  do {
    while(j.degree() < trpl->poly->lm()[var])
      j.step_deg();

    if (j.isNextDeg())
      out.set(var);

    ++var;
    if (j.isNextVar())
        j.step_var();
    else
      break;
  } while(true);

  return out;
}
