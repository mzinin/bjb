#include "janettree64.h"

void JanetTree64::Iterator::del() {
  Node* tmp = *i;
  *i = tmp->mNextDeg;
  delete tmp;
}

void JanetTree64::Iterator::build(int d, int var, Triple64 *trpl) {
  Node* r =  new JanetTree64::Node(trpl->poly->lm()[var]);
  Node* j = r;
  while(d > trpl->poly->lm()[var]) {
    d -= trpl->poly->lm()[var];
    var++;
    j->mNextVar = new JanetTree64::Node(trpl->poly->lm()[var]);
    j = j->mNextVar;
  }
  j->mTriple = trpl;

  r->mNextDeg = *i;
  *i = r;
}

void JanetTree64::Iterator::build_binomial(int d, int var, Triple64 *trpl) {
  bitset<64> exp(trpl->plm);
  Node* r =  new JanetTree64::Node(2*exp.test(var));
  Node* j = r;
  while(d > 2*exp.test(var)) {
    d -= 2*exp.test(var);
    var++;
    j->mNextVar = new JanetTree64::Node(2*exp.test(var));
    j = j->mNextVar;
  }
  j->mTriple = trpl;

  r->mNextDeg = *i;
  *i = r;
}

void JanetTree64::Iterator::clear() {
//cout<<"Here 1 "<<*i<<endl;
//cout<<nextDeg()<<endl;
//cout<<"Here 2"<<endl;
  while (nextDeg()) {
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
        break;
      }
    } while(true);
  }
  return trpl;
}

void JanetTree64::del(Triple64 *trpl) {
  Iterator j(mRoot);
  //подсчет ветвлений
  int var=0, vet=0;
  if (j.isNextDeg() && j.isNextVar()) vet++;

  do {
    while(j.degree() < trpl->poly->lm()[var]){
      j.step_deg();
      if (j.isNextDeg() && j.isNextVar()) vet++;
    }

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
}

void JanetTree64::del_binomial(int i) {
  Iterator j(mRoot);
  //подсчет ветвлений
  int var=0, vet=0, deg;
  if (j.isNextDeg() && j.isNextVar()) vet++;

  do {
    if (var==i) deg = 2;
    else deg = 0;
    while(j.degree() < deg){
      j.step_deg();
      if (j.isNextDeg() && j.isNextVar()) vet++;
    }

    if (j.nextVar()) {
      var++;
      j.step_var();
      if (j.isNextDeg() && j.isNextVar()) vet++;
    }
    else
      break;
  } while(true);

  //собственно удаление
  j = mRoot;
  var=0;
  bool varDirection=false;
  if (var==i) deg = 2;
  else deg = 0;

  if (j.isNextDeg() && j.isNextVar()) vet--;
  if (vet==0)
    if (j.degree() < deg)
      j.step_deg();
    else
      varDirection=true;

  while (vet>0) {
    while(j.degree() < deg && vet>0){
      j.step_deg();
      if (j.isNextDeg() && j.isNextVar()) vet--;
    }
    if (vet==0)
      if (j.degree() < deg){
        j.step_deg();
        break;
      }
      else{
        varDirection=true;
        break;
      }

    var++;
    if (var==i) deg = 2;
    else deg = 0;

    j.step_var();
    if (j.isNextDeg() && j.isNextVar()) vet--;
    if (vet==0){
      if (j.degree() < deg){
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
}

void JanetTree64::insert(Triple64* trpl) {
  if (!trpl->poly)
    return;
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
}

void JanetTree64::insert_binomial(Triple64* trpl) {
  unsigned d = 2;
  bitset<64> exp(trpl->plm);
  JanetTree64::Iterator j(mRoot);
  if (mRoot == NULL)
    j.build_binomial(d, 0, trpl);
  else {
    int var = 0;
    do {
      while(j.degree() < 2*(exp.test(var)) && j.isNextDeg())
        j.step_deg();

      if (j.degree() > 2*(exp.test(var))) {
        j.build_binomial(d, var, trpl);
        break;
      }
      else if (j.degree() == 2*(exp.test(var))) {
        d -= 2*(exp.test(var));
        var++;
        j.step_var();
      }
      else {
        j.step_deg();
        j.build_binomial(d, var, trpl);
        break;
      }
    } while(true);
  }
}

void JanetTree64::update(Triple64 *trpl, vector<Triple64*> &set) {
  JanetTree64::ConstIterator j(mRoot);
  int var = 0;

  do {
    while(j.degree() < trpl->poly->lm()[var])
      j.step_deg();

    if (j.isNextDeg()){
      if (!trpl->nmp.test(var)){
        IMyPoly64* tmp = trpl->poly->polyInterface()->copy(*trpl->poly);
        tmp->mult(var);
        if (tmp->length())
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
  bool IsFirstMarked = false;

  do {
    while(j.degree() < trpl->poly->lm()[var]){
      j.step_deg();
//       if (!IsFirstMarked && trpl->poly->lm()[var]){
//         out.set(var);
//         IsFirstMarked = true;
//       }
    }

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
