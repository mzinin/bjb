#include <iostream>
#include "poly64_64.h"

using namespace std;

struct Triple64_64 {
  Poly64_64*  poly;
  Monom64_64  anc;
  Triple64_64*   wanc;
  bitset<64> nmp;
  bool prolong;

  Triple64_64(Poly64_64 *p, const Monom64_64 &m, Triple64_64* w):
           poly(p), anc(m), wanc(w), nmp(), prolong(true) {}
  Triple64_64(Poly64_64 *p, const Monom64_64 &m, Triple64_64* w, bitset<64> &n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr) {}
  Triple64_64(Poly64_64 *p, const Monom64_64 &m, Triple64_64* w, int n, bool pr):
           poly(p), anc(m), wanc(w), nmp(n), prolong(pr) {}
  ~Triple64_64() {delete poly;}
};

class JanetTree64_64 {
public:
  struct Node {
    int           mDeg;
    Triple64_64*  mTriple;
    Node*         mNextDeg;
    Node*         mNextVar;

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
    Triple64_64* trpl() const { return i->mTriple; }
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
    Triple64_64*& trpl() const { return (*i)->mTriple; }
    unsigned degree() const { return (*i)->mDeg; }
    void build(int d, int var, Triple64_64 *trpl);
    void del();
    void exclude();
    void clear();
  };

  Node* mRoot;

public:
  JanetTree64_64(): mRoot(NULL) {};
  ~JanetTree64_64();

  Triple64_64* find(const Monom64_64& m) const;
  void insert(Triple64_64 *trpl);
  void del(Triple64_64 *trpl);
  void update(Triple64_64 *trpl, vector<Triple64_64*> &set);
  void clear();
  bitset<64> nmulti(Triple64_64 *trpl);
};
