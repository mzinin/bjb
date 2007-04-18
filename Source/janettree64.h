#include <iostream>
#include "imypoly64.h"

using namespace std;

struct Triple64 {
  IMyPoly64*  poly;
  IMyMonom64  anc;
  Triple64*   wanc;
  bitset<64> nmp;
  bool prolong;
  unsigned long plm;
  int degree;

  Triple64(IMyPoly64 *p, const IMyMonom64 &m, Triple64* w):
           poly(p), anc(m), wanc(w), nmp(), prolong(true), plm(poly->lm()), degree(poly->lm().degree()) {}
  Triple64(IMyPoly64 *p, const IMyMonom64 &m, Triple64* w, bitset<64> &n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr), plm(poly->lm()), degree(poly->lm().degree()) {}
  Triple64(IMyPoly64 *p, const IMyMonom64 &m, Triple64* w, int n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr), plm(poly->lm()), degree(poly->lm().degree()) {}

  Triple64(IMyPoly64 *p, const IMyMonom64 &m, Triple64* w, int n, bool pr, unsigned long lmonom, int deg):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr), plm(lmonom), degree(deg) {}
  ~Triple64() {delete poly;}
};

class JanetTree64 {
public:
  struct Node {
    int      mDeg;
    Triple64*  mTriple;
    Node*    mNextDeg;
    Node*    mNextVar;

    Node(int deg): mDeg(deg), mTriple(NULL), mNextDeg(NULL), mNextVar(NULL) {}
    ~Node() {}
  };

private:

  class ConstIterator {
    Node* i;
  public:
    ConstIterator(Node* n): i(n) {}

    void step_deg() { i = i->mNextDeg; }
    void step_var() { i = i->mNextVar; }
    operator bool() const { return (i != NULL); }
    ConstIterator nextDeg() const { return i->mNextDeg; }
    ConstIterator nextVar() const { return i->mNextVar; }
    bool isNextDeg() const { return i->mNextDeg; }
    bool isNextVar() const { return i->mNextVar; }
    Triple64* trpl() const { return i->mTriple; }
    unsigned degree() const { return i->mDeg; }
  };

  class Iterator {
    Node** i;

  public:
    Iterator(Node* &n): i(&n) {}

    void step_deg() { i = &(*i)->mNextDeg; }
    void step_var() { i = &(*i)->mNextVar; }
    operator bool() const { return (*i != NULL); }
    ConstIterator nextDeg() const { return (*i)->mNextDeg; }
    ConstIterator nextVar() const { return (*i)->mNextVar; }
    bool isNextDeg() const { return (*i)->mNextDeg; }
    bool isNextVar() const { return (*i)->mNextVar; }
    operator ConstIterator() const { return *i; }
    Triple64*& trpl() const { return (*i)->mTriple; }
    unsigned degree() const { return (*i)->mDeg; }
    void build(int d, int var, Triple64 *trpl);
    void build_binomial(int d, int var, Triple64 *trpl);
    void del();
    void exclude();
    void clear();
  };

  Node* mRoot;

public:
  JanetTree64(): mRoot(NULL) {};
  ~JanetTree64();

  Triple64* find(const IMyMonom64& m) const;
  void insert(Triple64 *trpl);
  void insert_binomial(Triple64 *trpl);
  void del(Triple64 *trpl);
  void del_binomial(int i);
  void update(Triple64 *trpl, vector<Triple64*> &set);
  void clear();
  bitset<64> nmulti(Triple64 *trpl);
};
