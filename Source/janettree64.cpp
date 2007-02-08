#include "janettree64.h"

void JanetTree64::Iterator::del() {
  Node* tmp = *i;
  *i = tmp->mNextDeg;
  delete tmp;
}

void JanetTree64::Iterator::build(int d, int var, Triple64 *trpl) {
  //IASSERT(d >= trpl->poly->lm()[var]);
  Node* r =  new JanetTree64::Node(trpl->poly->lm()[var]);
  Node* j = r;
  while(d > trpl->poly->lm()[var]) {
    d -= trpl->poly->lm()[var];
    var++;
    j->mNextVar = new JanetTree64::Node(trpl->poly->lm()[var]);
    j = j->mNextVar;
  }
  j->mTriple = trpl;

  //IASSERT(!*i || r->mDeg < (*i)->mDeg);
  r->mNextDeg = *i;
  *i = r;
}

void JanetTree64::Iterator::clear() {
  while (nextDeg()) {
    //IASSERT(nextVar());
    JanetTree64::Iterator((*i)->mNextVar).clear();
    del();
  }
  del();
}

JanetTree64::~JanetTree64() {
  clear();
  delete mRoot;
}

Triple64* JanetTree64::find(const IMyMonom64& m) const {
  Triple64* trpl = NULL;

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

void JanetTree64::del(Triple64 *trpl) {
  //IASSERT(find(trpl->poly->lm()) != NULL);
  Iterator j(mRoot);
  //подсчет ветвлений
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

  //собственно удаление
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

void JanetTree64::insert(Triple64* trpl) {
  //IASSERT(trpl != NULL);
  //IASSERT(find(trpl->poly->lm()) == NULL);

  unsigned d = trpl->poly->lm().degree();
  JanetTree64::Iterator j(mRoot);
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

void JanetTree64::update(Triple64 *trpl, vector<Triple64*> &set) {
  //IASSERT(trpl != NULL); IASSERT(trpl->poly != NULL);
  //IASSERT(find(trpl->poly->lm()) == trpl);

  JanetTree64::ConstIterator j(mRoot);
  int var = 0;
  do {
    while(j.degree() < trpl->poly->lm()[var]) {
      //IASSERT(j.isNextDeg());
      j.step_deg();
    }
    //IASSERT(j.degree() == trpl->poly->lm()[var]);

    if (j.isNextDeg()){
      if (!trpl->nmp.test(var)){
        IMyPoly64* tmp = trpl->poly->polyInterface()->copy(*trpl->poly);
        tmp->mult(var);
        set.push_back(new Triple64(tmp, trpl->anc, trpl, 0, true));
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

void JanetTree64::clear() {
  if (mRoot) {
    JanetTree64::Iterator j(mRoot);
    j.clear();
  }
}

bitset<64> JanetTree64::nmulti(Triple64 *trpl){
  IASSERT(trpl->poly != NULL);
  IASSERT(find(trpl->poly->lm()) == trpl);

  JanetTree64::ConstIterator j(mRoot);
  bitset<64> out = 0;
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
