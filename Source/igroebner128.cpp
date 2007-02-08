#include <iostream>
#include "igroebner128.h"
#include "itimer.h"

using namespace std;

extern int crit;
//ITimer timer_crit, timer_zero_reduce, timer_non_zero_reduce, timer_select;

bool Criteria1(Triple128 &p, Triple128 &g){
  IMyMonom128 tmp = p.anc;
  tmp.mult1(g.anc);

  if ( !p.anc.gcd(g.anc) && p.poly->lm()==tmp)
    return true;
  else
    return false;
}

bool Criteria2(Triple128 &p, Triple128 &g){
  IMyMonom128 tmp = p.anc;
  tmp.mult1(g.anc);

  if ( p.poly->lm().divisibilityTrue(tmp) )
    return true;
  else
    return false;
}

bool Criteria3(Triple128 &p, Triple128 &g, vector<Triple128*> &T){
  IMyMonom128  m = p.anc, mp = p.anc, mg = g.anc;
  m.mult1(g.anc);

  vector<Triple128*>::const_iterator ctit(T.begin());
  while(ctit!=T.end()){
    mp = p.anc; mp.mult1((**ctit).poly->lm());
    mg = g.anc; mg.mult1((**ctit).poly->lm());
    if( m.divisibilityTrue(mp) && m.divisibilityTrue(mg) )
      return true;
    ctit++;
  }

  return false;
}

bool Criteria4(Triple128 &p, Triple128 &g, vector<Triple128*> &T, JanetTree128 &JT){
  vector<Triple128*>::const_iterator ctit(T.begin());
  int dim = p.anc.dimIndepend();
  bitset<128> nm;

  while (p.wanc!=*ctit && ctit!=T.end())
    if ((**ctit).poly->degree() != p.poly->degree()-1)
      ctit++;
    else{
      IMyMonom128 tmp1 = p.anc; tmp1.mult((**ctit).anc);
      nm = JT.nmulti(*ctit);
      for (int i=0; i<dim; i++)
        if (nm.test(i)){
          IMyMonom128 tmp2 = (**ctit).poly->lm();
          tmp2.prolong(i);
          if (tmp2==p.poly->lm())
            if (tmp2.divisibilityTrue(tmp1))
              return true;
        }
      ctit++;
    }

  return false;
}

IMyPoly128* IGBasis128::NormalForm(Triple128 &p){
  IMyPoly128 *h,*q;
  Triple128  *inv_div;
  q = p.poly->polyInterface()->create();
  q->setZero();
  h = p.poly->polyInterface()->copy(*p.poly);

  inv_div = jTree.find(h->lm());
  if (p.prolong && inv_div){
    //timer_crit.cont();
    bool c = Criteria1(p, *inv_div) || Criteria2(p, *inv_div) || Criteria3(p, *inv_div, tSet)
             || Criteria4(p, *inv_div, tSet, jTree);
    //timer_crit.stop();
    if (c){
      //crit++;
      delete h;
      return q;
    }
  }

  //timer_zero_reduce.cont();
  //timer_non_zero_reduce.cont();
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

  //if (q->isZero())
  //  timer_zero_reduce.stop();
  //else
  //  timer_non_zero_reduce.stop();

  delete h;
  return q;
}

bool Compare_Triple(Triple128* a, Triple128* b){
  if (a->poly->lm().compare(b->poly->lm())==-1)
    return true;
  else
    return false;
}

IMyPoly128* findR(IMyPoly128& p, vector<IMyPoly128*> &Q){
  if (p.isZero()) return NULL;
  vector<IMyPoly128*>::const_iterator iq(Q.begin()), q_end(Q.end());
  IMyMonom128 *plm = (IMyMonom128*)&p.lm();

  while (iq!=q_end){
    if ( plm->divisibility((**iq).lm()) )
      return *iq;
    iq++;
  }

  return NULL;
}

IMyPoly128* Reduce(IMyPoly128 &p, vector<IMyPoly128*> &Q){
  IMyPoly128 *r,*q;
  IMyPoly128 *red;
  q = p.polyInterface()->create();
  q->setZero();
  r = p.polyInterface()->copy(p);
  vector<IMyPoly128*> rrq;
  vector<IMyPoly128*>::iterator it;

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

IGBasis128::IGBasis128():
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {}

void IGBasis128::ReduceSet(vector<IMyPoly128*> &set, int i) {
  vector<IMyPoly128*> R,P,Q;
  vector<IMyPoly128*>::iterator ir(R.begin()), ip(P.begin()), iq(Q.begin());
  vector<IMyPoly128*>::const_iterator j(set.begin()), q_end, r_end, p_end;
  IMyPoly128 *h,*h1;
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
      //Q
      IMyMonom128 *hlm=(IMyMonom128*)&h->lm();
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

  R.clear();
  Q.clear();
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

void IGBasis128::InvolutiveBasisI(){
  vector<Triple128*>::iterator qit(qSet.begin()), tit(tSet.begin());
  IMyPoly128* h;
  //int zero_red=0, nonzero_red=0;

  //timer_select.start();
  qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
  //timer_select.stop();
  tit = tSet.insert(tit, *qit);
  jTree.insert(*qit);
  qit = qSet.erase(qit);

  bool lm_changed;

  while (!qSet.empty()){
    //timer_select.cont();
    qit = min_element(qSet.begin(), qSet.end(), Compare_Triple);
    //timer_select.stop();
    h = NormalForm(**qit);

    if (!h->isZero() && h->lm()==(**qit).poly->lm())
      lm_changed = false;
    else
      lm_changed = true;
    bitset<128> n = (**qit).nmp;
    IMyMonom128 qanc = (**qit).anc;

    delete *qit;
    qit = qSet.erase(qit);

    if (!h->isZero()){
      //nonzero_red++;
      tit = tSet.begin();
      while (tit!=tSet.end())
        if ((*tit)->poly->lm().divisibilityTrue(h->lm())){
          qit = qSet.begin();
          IMyMonom128 tlm = (**tit).poly->lm();
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
          tit++;;

      if (lm_changed)
        tSet.push_back(new Triple128(h,h->lm(),NULL,0,false));
      else
        tSet.push_back(new Triple128(h,qanc,NULL,n,false));
      tit = tSet.end();
      tit--;
      jTree.insert(*tit);

      while (tit!=tSet.begin()){
        jTree.update(*tit, qSet);
        tit--;
      }
      jTree.update(*tit, qSet);
    }
    else{
      //zero_red++;
      delete h;
    }
  }

  //cout<<"Polynoms considered: "<<zero_red+nonzero_red<<endl;
  //cout<<"Zero reductions: "<<zero_red<<endl;
  //cout<<"Criterion refused: "<<crit<<endl;
  //cout<<"Non-zero reductions: "<<nonzero_red<<endl;
  jTree.clear();
}

IGBasis128::IGBasis128(vector<IMyPoly128*> set):
  gBasis(), qSet(), tSet(), jTree(), vars(), mInterface_local(NULL), pInterface_local(NULL) {

  vector<IMyPoly128*>::const_iterator i1(set.begin());
  vector<IMyPoly128*>::iterator i2(gBasis.begin());

  mInterface_local = (**i1).monomInterface();
  pInterface_local = (**i1).polyInterface();
  int dim = mInterface_local->dimIndepend(),i;

  while (i1!=set.end()){
    i2=gBasis.insert(i2, pInterface_local->copy(**i1));
    for (i = 0; i < dim; i++){
      i2=gBasis.insert(i2, pInterface_local->copy(**i1));
      (**i2).mult(i);
    }
    ++i1;
  }

  ReduceSet(gBasis,1);
  //cout<<endl;

  i2 = gBasis.begin();
  while (i2!=gBasis.end()){
    qSet.push_back(new Triple128(*i2,(*i2)->lm(),NULL,0,false));
    i2++;
  }
  gBasis.clear();

  InvolutiveBasisI();

  vector<Triple128*>::const_iterator i3(tSet.begin());
  while(i3!=tSet.end()){
    gBasis.push_back((*i3)->poly);
    i3++;
  }
  tSet.clear();

  ReduceSet(gBasis,0);
  //cout<<"Criterion's time: "<<endl<<timer_crit<<endl;
  //cout<<"Zero reduction time: "<<endl<<timer_zero_reduce<<endl;
  //cout<<"Nonzero reduction time: "<<endl<<timer_non_zero_reduce<<endl;
  //cout<<"Selection time: "<<endl<<timer_select<<endl;
}

IMyPoly128* IGBasis128::operator[](int num){
  vector<IMyPoly128*>::const_iterator it(gBasis.begin());
  it+=length()-1-num;
  return *it;
}

int IGBasis128::length(){
  return gBasis.size();
}

std::ostream& operator<<(std::ostream& out, IGBasis128& GBasis) {
  int i=0;
  for (i=0;i<GBasis.length();i++)
    out<<'['<<i<<"] = "<<*GBasis[i]<<'\n';

  return out;
}
