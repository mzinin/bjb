#include <iostream>
#include "igroebner64.h"
#include "itimer.h"

using namespace std;

int crit=0;

bool Criteria1(Triple64 &p, Triple64 &g){
  IMyMonom64 tmp = p.anc;
  tmp.mult1(g.anc);

  if ( !p.anc.gcd(g.anc) && p.poly->lm()==tmp)
    return true;
  else
    return false;
}

bool Criteria2(Triple64 &p, Triple64 &g){
  IMyMonom64 tmp = p.anc;
  tmp.mult1(g.anc);

  if ( p.poly->lm().divisibilityTrue(tmp) )
    return true;
  else
    return false;
}

bool Criteria3(Triple64 &p, Triple64 &g, vector<Triple64*> &T){
  IMyMonom64  m = p.anc, mp = p.anc, mg = g.anc;
  m.mult1(g.anc);

  vector<Triple64*>::const_iterator ctit(T.begin());
  while(ctit!=T.end()){
    mp = p.anc; mp.mult1((**ctit).poly->lm());
    mg = g.anc; mg.mult1((**ctit).poly->lm());
    if( m.divisibilityTrue(mp) && m.divisibilityTrue(mg) )
      return true;
    ctit++;
  }

  return false;
}

bool Criteria4(Triple64 &p, Triple64 &g, vector<Triple64*> &T, JanetTree64 &JT){
  vector<Triple64*>::const_iterator ctit(T.begin());
  int dim = p.anc.dimIndepend();
  bitset<64> nm;

  while (p.wanc!=*ctit && ctit!=T.end())
    if ((**ctit).poly->degree() != p.poly->degree()-1)
      ctit++;
    else{
      IMyMonom64 tmp1 = p.anc; tmp1.mult((**ctit).anc);
      nm = JT.nmulti(*ctit);
      for (int i=0; i<dim; i++)
        if (nm.test(i)){
          IMyMonom64 tmp2 = (**ctit).poly->lm();
          tmp2.prolong(i);
          if (tmp2==p.poly->lm())
            if (tmp2.divisibilityTrue(tmp1))
              return true;
        }
      ctit++;
    }

  return false;
}

IMyPoly64* IGBasis64::NormalForm(Triple64 &p){
  IMyPoly64 *h,*q;
  Triple64  *inv_div;
  q = pInterface_local->create();
  q->setZero();

  if (!p.poly) return q;

  h = pInterface_local->copy(*p.poly);
  if (tSet.empty()) {delete q; return h;}

  inv_div = jTree.find(h->lm());
  if (p.prolong && inv_div){
    bool c = false;//Criteria1(p, *inv_div) || Criteria2(p, *inv_div) || Criteria3(p, *inv_div, tSet)
             //|| Criteria4(p, *inv_div, tSet, jTree);
    if (c){
      delete h;
      return q;
    }
  }

  while (!h->isZero()){
    inv_div = jTree.find(h->lm());
    while (inv_div){
      h->reduction1(*inv_div->poly);
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

  delete h;
  return q;
}

bool Compare_Triple(Triple64* a, Triple64* b){
  if (a->degree < b->degree)
    return true;
  else if (a->degree > b->degree)
    return false;
  else
    if (a->plm > b->plm)
      return true;
    else
      return false;
}

IMyPoly64* findR(IMyPoly64& p, vector<IMyPoly64*> &Q){
  if (p.isZero()) return NULL;
  vector<IMyPoly64*>::const_iterator iq(Q.begin()), q_end(Q.end());
  IMyMonom64 *plm = (IMyMonom64*)&p.lm();

  while (iq!=q_end){
    if ( plm->divisibility((**iq).lm()) )
      return *iq;
    iq++;
  }

  return NULL;
}

IMyPoly64* Reduce(IMyPoly64 &p, vector<IMyPoly64*> &Q){
  IMyPoly64 *r,*q;
  IMyPoly64 *red;
  q = p.polyInterface()->create();
  q->setZero();
  r = p.polyInterface()->copy(p);
  vector<IMyPoly64*> rrq;
  vector<IMyPoly64*>::iterator it;

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

IGBasis64::IGBasis64():
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {}

void IGBasis64::ReduceSet(vector<IMyPoly64*> &set, int i) {
  vector<IMyPoly64*> R,P,Q;
  vector<IMyPoly64*>::iterator ir(R.begin()), ip(P.begin()), iq(Q.begin());
  vector<IMyPoly64*>::const_iterator j(set.begin()), q_end, r_end, p_end;
  IMyPoly64 *h,*h1;
  if (i)
    while (j!=set.end()){
      ir=R.insert(ir,*j);
      ++j;
    }
  else
    R = set;

  int num;

  while (!R.empty()){
    ir = R.begin();
    h = *ir;
    ir = R.erase(ir);
    h = Reduce(*h,P);
    if (!h->isZero()){
      //Q - ? не помню старый комментарий
      IMyMonom64 *hlm=(IMyMonom64*)&h->lm();
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

void IGBasis64::InvolutiveBasis(){
  vector<Triple64*>::iterator qit(qSet.begin()), tit(tSet.begin());
  IMyPoly64* h;
  int i;

  qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
  if ((**qit).poly){
    tit = tSet.insert(tit, *qit);
    jTree.insert(*qit);
    switch ((**qit).poly->lm().degree()){
      case 0:
        binomials_in_tset.set();
        break;
      case 1:
	binomials_in_tset.set((**qit).poly->lm().num_of_only_var());
	break;
    }
  }
  else {
    jTree.insert_binomial(*qit);
    binomials_in_tset|=(*qit)->plm;
  }
  qit = qSet.erase(qit);

  bool lm_changed;

  while (!qSet.empty()){
    qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
    if (!(**qit).poly){
      bitset<64> WeDontNeedThisBinomial = binomials_in_tset;
      WeDontNeedThisBinomial&=(*qit)->plm;
      if (WeDontNeedThisBinomial==0){
        jTree.insert_binomial(*qit);
        binomials_in_tset|=(*qit)->plm;
      }
      else
        binomials_in_tset^=WeDontNeedThisBinomial;
    }
    h = NormalForm(**qit);

    if (!h->isZero() && h->lm()==(**qit).poly->lm())
      lm_changed = false;
    else
      lm_changed = true;
    bitset<64> n = (**qit).nmp;
    IMyMonom64 qanc = (**qit).anc;

    delete *qit;
    qit = qSet.erase(qit);

    if (!h->isZero()){
       switch (h->lm().degree()){
	 case 0:
           for (i=0; i<Dim; i++)
             if (binomials_in_tset.test(i))
               jTree.del_binomial(i);
           binomials_in_tset.reset();
           break;
         case 1:
           i = h->lm().num_of_only_var();
           if (binomials_in_tset.test(i)){
             jTree.del_binomial(i);
             binomials_in_tset.reset(i);
           }
           else
             binomials_in_tset.set(i);
           break;
       }

      tit = tSet.begin();
      while (tit!=tSet.end())
        if ((*tit)->poly->lm().divisibilityTrue(h->lm())){
          qit = qSet.begin();
          IMyMonom64 tlm = (**tit).poly->lm();
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
        tSet.push_back(new Triple64(h,h->lm(),NULL,0,false));
      else
        tSet.push_back(new Triple64(h,qanc,NULL,n,false));
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

IGBasis64::IGBasis64(vector<IMyPoly64*> set):
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {

  vector<IMyPoly64*>::const_iterator i1(set.begin());
  vector<IMyPoly64*>::iterator i2(gBasis.begin());

  mInterface_local = (**i1).monomInterface();
  pInterface_local = (**i1).polyInterface();
  Dim = mInterface_local->dimIndepend();
  binomials_in_tset = 0;
  int i;
  unsigned long lm = 1;
  IMyMonom64 *monomOne = mInterface_local->create();
  monomOne->setZero();

  while (i1!=set.end()){
    i2=gBasis.insert(i2, pInterface_local->copy(**i1));
    //for (i = 0; i < Dim; i++){
    //  i2=gBasis.insert(i2, pInterface_local->copy(**i1));
    //  (**i2).mult(i);
    //}
    ++i1;
  }

  ReduceSet(gBasis,1);

  i2 = gBasis.begin();
  while (i2!=gBasis.end()){
    qSet.push_back(new Triple64(*i2,(*i2)->lm(),NULL,0,false));
    i2++;
  }

//-------------добавляем пустышки
  for (i=0; i<Dim; i++){
    qSet.push_back(new Triple64(NULL,*monomOne,NULL,i,false,lm,2));
    lm = lm<<1;
  }
  gBasis.clear();

  InvolutiveBasis();

  vector<Triple64*>::const_iterator i3(tSet.begin());
  while(i3!=tSet.end()){
    if ((*i3)->poly->lm()==(*i3)->anc)
      gBasis.push_back((*i3)->poly);
    i3++;
  }
  tSet.clear();

  ReduceSet(gBasis,0);
}

IMyPoly64* IGBasis64::operator[](int num){
  vector<IMyPoly64*>::const_iterator it(gBasis.begin());
  it+=length()-1-num;
  return *it;
}

int IGBasis64::length(){
  return gBasis.size();
}

std::ostream& operator<<(std::ostream& out, IGBasis64& GBasis) {
  int i=0;
  for (i=0;i<GBasis.length();i++)
    out<<'['<<i<<"] = "<<*GBasis[i]<<'\n';

  return out;
}
