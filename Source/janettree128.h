#include <iostream>
#include "imypoly128.h"

using namespace std;

struct Triple128 {
  IMyPoly128*  poly;
  IMyMonom128  anc;
  Triple128*   wanc;
  bitset<128> nmp;
  bool prolong;

  Triple128(IMyPoly128 *p, const IMyMonom128 &m, Triple128* w):
           poly(p), anc(m), wanc(w), nmp(), prolong(true) {}
  Triple128(IMyPoly128 *p, const IMyMonom128 &m, Triple128* w, bitset<128> &n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr) {}
  Triple128(IMyPoly128 *p, const IMyMonom128 &m, Triple128* w, int n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr) {}
  ~Triple128() {delete poly;}
};

class JanetTree128 {
public:
  struct Node {
    int      mDeg;
    Triple128*  mTriple;
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
    Triple128* trpl() const { return i->mTriple; }
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
    Triple128*& trpl() const { return (*i)->mTriple; }
    unsigned degree() const { return (*i)->mDeg; }
    void build(int d, int var, Triple128 *trpl);
    void del();
    void exclude();
    void clear();
  };

  Node* mRoot;

public:
  JanetTree128(): mRoot(NULL) {};
  ~JanetTree128();

  Triple128* find(const IMyMonom128& m) const;
  void insert(Triple128 *trpl);
  void del(Triple128 *trpl);
  void update(Triple128 *trpl, vector<Triple128*> &set);
  void clear();
  bitset<128> nmulti(Triple128 *trpl);
};
