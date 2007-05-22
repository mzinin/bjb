#include <iostream>
#include "involutive32_64.h"
#include "timer.h"

using namespace std;

void showlm(vector<Triple32_64*> set){
  vector<Triple32_64*>::const_iterator i(set.begin());
  while (i!=set.end()){
    cout<<'('<<(*i)->poly->lm()<<','<<(*i)->anc<<")  ";
    i++;
  }
  cout<<endl<<endl;
}

bool Criteria1(Triple32_64 &p, Triple32_64 &g){
  Monom32_64 tmp = p.anc;
  tmp.mult(g.anc);

  if ( !p.anc.gcd(g.anc) && p.poly->lm()==tmp)
    return true;
  else
    return false;
}

bool Criteria2(Triple32_64 &p, Triple32_64 &g){
  Monom32_64 tmp = p.anc;
  tmp.mult(g.anc);

  if ( p.poly->lm().divisibilityTrue(tmp) )
    return true;
  else
    return false;
}

bool Criteria3(Triple32_64 &p, Triple32_64 &g, vector<Triple32_64*> &T){
  Monom32_64  m = p.anc, mp = p.anc, mg = g.anc;
  m.mult(g.anc);

  vector<Triple32_64*>::const_iterator ctit(T.begin());
  while(ctit!=T.end()){
    mp = p.anc; mp.mult((**ctit).poly->lm());
    mg = g.anc; mg.mult((**ctit).poly->lm());
    if( m.divisibilityTrue(mp) && m.divisibilityTrue(mg) )
      return true;
    ctit++;
  }

  return false;
}

bool Criteria4(Triple32_64 &p, Triple32_64 &g, vector<Triple32_64*> &T, JanetTree32_64 &JT){
  vector<Triple32_64*>::const_iterator ctit(T.begin());
  int dim = p.anc.dimIndepend();
  bitset<64> nm;

  while (p.wanc!=*ctit && ctit!=T.end())
    if ((**ctit).poly->degree() != p.poly->degree()-1)
      ctit++;
    else{
      Monom32_64 tmp1 = p.anc; tmp1.mult((**ctit).anc);
      nm = JT.nmulti(*ctit);
      for (int i=0; i<dim; i++)
        if (nm.test(i)){
          Monom32_64 tmp2 = (**ctit).poly->lm();
          tmp2.prolong(i);
          if (tmp2==p.poly->lm())
            if (tmp2.divisibilityTrue(tmp1))
              return true;
        }
      ctit++;
    }

  return false;
}

Poly32_64* GBasis32_64::NormalForm(Triple32_64 &p){
  Poly32_64 *h,*q;
  Triple32_64  *inv_div;
  q = new Poly32_64(pInterface_local);
  q->setZero();
  if (!p.poly) return q;
  h = new Poly32_64(*p.poly);
  if (tSet.empty()) {delete q; return h;}

  inv_div = jTree.find(h->lm());
  if (p.prolong && inv_div){
    bool c = Criteria1(p, *inv_div) || Criteria2(p, *inv_div) || Criteria3(p, *inv_div, tSet)
             || Criteria4(p, *inv_div, tSet, jTree);
    if (c){
      delete h;
      return q;
    }
  }

  while (!h->isZero()){
    inv_div = jTree.find(h->lm());
    while (inv_div){
      h->reduction(*inv_div->poly);
      if (!h->isZero())
        inv_div = jTree.find(h->lm());
      else
        inv_div = NULL;
    }
    if (!h->isZero()){
      q->add(h->lm());
      h->rid_of_lm();
    }
  }

/*
  while (!h->isZero()){
    inv_div = jTree.find(h->lastm());
    while (inv_div){
      h->reduction(*inv_div->poly);
      if (!h->isZero())
        inv_div = jTree.find(h->lastm());
      else
        inv_div = NULL;
    }
    if (!h->isZero()){
      q->add(h->lastm());
      h->rid_of_lastm();
    }
  }
*/
  delete h;
  return q;
}

bool Compare_Triple(Triple32_64* a, Triple32_64* b){
  if (a->poly->lm().compare(b->poly->lm())==-1)
    return true;
  else
    return false;
}

Poly32_64* findR(Poly32_64& p, vector<Poly32_64*> &Q){
  if (p.isZero()) return NULL;
  vector<Poly32_64*>::const_iterator iq(Q.begin()), q_end(Q.end());
  Monom32_64 *plm = (Monom32_64*)&p.lm();

  while (iq!=q_end){
    if ( plm->divisibility((**iq).lm()) )
      return *iq;
    iq++;
  }

  return NULL;
}

Poly32_64* Reduce(Poly32_64 &p, vector<Poly32_64*> &Q){
  Poly32_64 *r,*q, *red;
  q = new Poly32_64(p.polyInterface());
  q->setZero();
  r = new Poly32_64(p);
  vector<Poly32_64*> rrq;
  vector<Poly32_64*>::iterator it;

  while (!r->isZero()){
    red = findR(*r,Q);
    while (red){
      r->reduction(*red);
      red = findR(*r,Q);
    }
    if (!r->isZero()){
      q->add(r->lm());
      r->rid_of_lm();
    }
  }

  delete r;
  return q;
}

GBasis32_64::GBasis32_64():
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {}

void GBasis32_64::ReduceSet(vector<Poly32_64*> &set/*, int i*/) {
  vector<Poly32_64*> R,P,Q;
  vector<Poly32_64*>::iterator ir(R.begin()), ip(P.begin()), iq(Q.begin());
  vector<Poly32_64*>::const_iterator j(set.begin()), q_end, r_end, p_end;
  Poly32_64 *h,*h1;
/*  if (i)
    while (j!=set.end()){
      ir=R.insert(ir,*j);
      ++j;
    }
  else*/
    R = set;

  int num;

  while (!R.empty()){
    ir = R.begin();
    h = *ir;
    ir = R.erase(ir);
    h = Reduce(*h,P);
    if (!h->isZero()){
      //Q - ? не помню старый комментарий
      Monom32_64 *hlm=(Monom32_64*)&h->lm();
      ip = P.begin();
      p_end = P.end();
      while (ip!=p_end){
        if ((**ip).lm().divisibility(*hlm))
          iq=Q.insert(iq,*ip);
	++ip;
      }
      //R = R U Q
      iq = Q.begin();
      q_end = Q.end();
      while (iq!=q_end){
        ir = R.begin();
        r_end = R.end();
        while (ir!=r_end && (**ir)!=(**iq))
	  ++ir;
        if (ir==r_end)
          ir=R.insert(ir,*iq);
	++iq;
      }
      //P = (P-Q) U {h}
      ip = P.begin();
      while (ip!=P.end()) {
        iq = Q.begin();
        q_end = Q.end();
        while (iq!=q_end && (**iq)!=(**ip))
	  ++iq;
        if (iq!=q_end) ip=P.erase(ip);
	else ++ip;
      }
      ip=P.insert(ip,h);
    }
  }

  R.clear(); //не помню
  Q.clear(); //вместо S из книги
  ir = R.begin();
  iq = Q.begin();
  ip = P.begin();
  p_end = P.end();
  while (ip!=p_end){
    iq=Q.insert(iq,*ip);
    ++ip;
  }
  ip = P.begin();
  while (ip!=p_end){
    h = *ip;
    iq = Q.begin();
    while ((*h)!=(**iq)) ++iq;
    iq=Q.erase(iq);
    h1 = Reduce(*h,Q);
    iq=Q.insert(iq,h);
    if (!h1->isZero())
      ir=R.insert(ir,h1);
    ++ip;
  }
  set.clear();
  set = R;
}

void GBasis32_64::InvolutiveBasis(){
  vector<Triple32_64*>::iterator qit(qSet.begin()), tit(tSet.begin());
  Poly32_64* h;

  qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
  tit = tSet.insert(tit, *qit);
  jTree.insert(*qit);
  qit = qSet.erase(qit);

  bool lm_changed;

  while (!qSet.empty()){
    //showlm(tSet);showlm(qSet);cout<<endl;
    qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
    h = NormalForm(**qit);

    if (!h->isZero() && h->lm()==(**qit).poly->lm())
      lm_changed = false;
    else
      lm_changed = true;
    bitset<64> n = (**qit).nmp;
    Monom32_64 qanc = (**qit).anc;

    delete *qit;
    qit = qSet.erase(qit);

    if (!h->isZero()){
      tit = tSet.begin();
      while (tit!=tSet.end())
        if ((*tit)->poly->lm().divisibilityTrue(h->lm())){
        //if ((*tit)->poly->lm().compare(h->lm())==1){
          qit = qSet.begin();
          Monom32_64 tlm = (**tit).poly->lm();
          while (qit!=qSet.end())
            if ((**qit).anc == tlm){
              delete *qit;
              qit = qSet.erase(qit);
            }
            else
              qit++;
          qSet.push_back(*tit);
          jTree.del(*tit);
          tit = tSet.erase(tit);
        }
        else
          tit++;

      if (lm_changed)
        tSet.push_back(new Triple32_64(h,h->lm(),NULL,0,false));
      else
        tSet.push_back(new Triple32_64(h,qanc,NULL,n,false));
      tit = tSet.end();
      tit--;
      jTree.insert(*tit);
      while (tit!=tSet.begin()){
        jTree.update(*tit, qSet);
        tit--;
      }
      jTree.update(*tit, qSet);
    }
    else
      delete h;
  }

  jTree.clear();
}

GBasis32_64::GBasis32_64(vector<Poly32_64*> set):
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {

  vector<Poly32_64*>::const_iterator i1(set.begin());
  vector<Poly32_64*>::iterator i2(gBasis.begin());

  mInterface_local = (**i1).monomInterface();
  pInterface_local = (**i1).polyInterface();
  Dim = mInterface_local->dimIndepend();
  int i;

  while (i1!=set.end()){
    i2=gBasis.insert(i2, new Poly32_64(**i1));
    for (i = 0; i < Dim; i++){
      i2=gBasis.insert(i2, new Poly32_64(**i1));
      (**i2).mult(i);
    }
    ++i1;
  }

  for (i=0; i<Dim; i++){
    Poly32_64 *bin = new Poly32_64(pInterface_local);
    bin->setOne();
    bin->mult(i,2);
    Poly32_64 *tbin = new Poly32_64(pInterface_local);
    tbin->setOne();
    tbin->mult(i,1);
    bin->add(*tbin);
    delete tbin;
    i2 = gBasis.insert(i2, bin);
  }

  ReduceSet(gBasis);

  i2 = gBasis.begin();
  while (i2!=gBasis.end()){
    qSet.push_back(new Triple32_64(*i2,(*i2)->lm(),NULL,0,false));
    i2++;
  }
  gBasis.clear();

  InvolutiveBasis();

  vector<Triple32_64*>::const_iterator i3(tSet.begin());
  while(i3!=tSet.end()){
    if ((*i3)->poly->lm()==(*i3)->anc /*&& !(*i3)->poly->isBinomial()*/)
      gBasis.push_back((*i3)->poly);
    i3++;
  }
  tSet.clear();

  ReduceSet(gBasis);
  i2 = gBasis.begin();
  while (i2!=gBasis.end()){
    if ((**i2).isBinomial())
      i2 = gBasis.erase(i2);
    else
      i2++;
  }
}

Poly32_64* GBasis32_64::operator[](int num){
  vector<Poly32_64*>::const_iterator it(gBasis.begin());
  it+=length()-1-num;
  return *it;
}

int GBasis32_64::length(){
  return gBasis.size();
}

std::ostream& operator<<(std::ostream& out, GBasis32_64& GBasis) {
  int i=0;
  for (i=0;i<GBasis.length();i++)
    out<<'['<<i<<"] = "<<*GBasis[i]<<'\n';

  return out;
}
