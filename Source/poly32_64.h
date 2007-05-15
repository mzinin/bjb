#include <iostream>
#include "monom32_64.h"

class Poly32_64;
class PolyInterface32_64 {
  friend class Poly32_64;

protected:
  MonomInterface32_64 *mMonomInterface;

public:
  PolyInterface32_64(MonomInterface32_64 *mInterface);
  ~PolyInterface32_64() {}

  MonomInterface32_64* monomInterface() const { return mMonomInterface; }
  Poly32_64* create();
};

class Poly32_64 {
  friend class PolyInterface32_64;

protected:
  typedef Monom32_64* It;
  typedef const Monom32_64* ConstIt;
  Monom32_64*  mHead;
  PolyInterface32_64* mRealization;
  int len;

  void additive(std::istream& in);
  void multiplicative(std::istream& in);
  void unary(std::istream& in);
  void power(std::istream& in);
  void bracket(std::istream& in);

public:

  class Iterator;
  class ConstIterator {
    friend class Poly32_64;
    friend class Iterator;

  protected:
    Poly32_64::ConstIt mConstIt;

  public:
    ConstIterator(): mConstIt(NULL) {};
    ConstIterator(const Poly32_64::ConstIt constIt): mConstIt(constIt) {}
    ~ConstIterator() {}
    void operator=(const ConstIterator& a) { mConstIt = a.mConstIt; }
    operator bool() const { return mConstIt; }
    bool isNext() const { return (mConstIt->Next != NULL); }
    const Monom32_64& monom() const { return *mConstIt; }
    const Monom32_64& operator*() const { return *mConstIt; }
    const Monom32_64* operator->() const { return mConstIt; }
    void operator++() { mConstIt = mConstIt->Next; }
    void operator++(int i) { mConstIt = mConstIt->Next; }
    bool operator==(const ConstIterator& a) const { return mConstIt == a.mConstIt;; }
    bool operator!=(const ConstIterator& a) const { return mConstIt != a.mConstIt; }
  };

  class Iterator {
    friend class Poly32_64;
    friend class ConstIterator;

  protected:
    Poly32_64::It *mIt;

  public:
    Iterator(): mIt(NULL) {};
    Iterator(Poly32_64::It &it): mIt(&it) {}
    ~Iterator() {}
    void operator=(const Iterator& a) { mIt = a.mIt; }
    operator bool() const { return *mIt; }
    bool isNext() const { return ((*mIt)->Next != NULL); }
    const Monom32_64& monom() const { return **mIt; }
    Monom32_64& operator*() const { /*IASSERT(mIt); IASSERT(*mIt);*/ return **mIt; }
    Monom32_64* operator->() const { /*IASSERT(mIt); IASSERT(*mIt);*/ return *mIt; }
    void operator++() { mIt = &(*mIt)->Next; }
    void operator++(int i) { mIt = &(*mIt)->Next; }
    bool operator==(const Iterator& a) const { return *mIt == *a.mIt; }
    bool operator!=(const Iterator& a) const { return *mIt != *a.mIt; }
    void insert(const Monom32_64 &m) {
      //IASSERT(mIt);
      Monom32_64 *tmp = new Monom32_64(m);
      tmp->Next = *mIt;
      *mIt = tmp;
    }
    void insert(ConstIterator j) {
      //IASSERT(mIt);
      Monom32_64 *tmp = const_cast<Monom32_64*>(j.mConstIt);
      tmp->Next = *mIt;
      *mIt = tmp;
      tmp = NULL;
    }
    ConstIterator get() {
      //IASSERT(mIt);
      ConstIterator j(*mIt);
      *mIt = (*mIt)->Next;
      Monom32_64 *tmp = const_cast<Monom32_64*>(j.mConstIt);
      tmp->Next = NULL;
      tmp = NULL;
      return j;
    }
    void del() {
      //IASSERT(mIt);
      Monom32_64 *tmp = *mIt;
      *mIt = (*mIt)->Next;
      delete tmp;
    }
    void clear() {
      while (*mIt)
        del();
    }
  };

  MonomInterface32_64* monomInterface() const { return mRealization->mMonomInterface; }
  PolyInterface32_64* polyInterface() const { return mRealization; }

  Poly32_64(PolyInterface32_64* r);
  Poly32_64(const Poly32_64& a);

  ~Poly32_64() { begin().clear(); }

  void setOne();
  void setZero();
  void set(const Poly32_64& a);
  void operator=(const Poly32_64& a) { set(a); }

  void swap(Poly32_64& a);
  bool isZero() const;
  bool isBinomial() const;
  int length() const;

  int degree() const;
  int degree_of_monom(int i) const;
  int deg(int var);

  const Monom32_64& lm() const;
  void rid_of_lm();
  const Monom32_64& lastm() const;
  void rid_of_lastm();

  void add(const Monom32_64 &m);
  void add(Poly32_64& a);

  void mult(int var, unsigned deg=1);
  void mult(const Monom32_64& m);
  void mult(const Poly32_64 &a);

  void mult1(int var, unsigned deg=1);
  void mult1(const Monom32_64& m);
  void mult1(const Poly32_64 &a);

  void reduction(const Poly32_64& a);
  void head_reduction(const Poly32_64& a);
  void reduction1(const Poly32_64& a);

  ConstIterator begin() const;
  Iterator begin();

  friend std::ostream& operator<<(std::ostream& out, const Poly32_64& a);
  friend std::istream& operator>>(std::istream& in, Poly32_64& a);
  friend bool operator==(const Poly32_64 &a, const Poly32_64 &b);
  friend bool operator!=(const Poly32_64 &a, const Poly32_64 &b) {return !(a==b);}

  void assertValid(const char* fileName, int fileLine) const;
};

inline const Monom32_64& Poly32_64::lm() const {
  //IASSERT(!isZero());
  return *mHead;
}

inline const Monom32_64& Poly32_64::lastm() const {
  Monom32_64 *mLast = mHead;
  while (mLast->Next)
    mLast = mLast->Next;
  return *mLast;
}

inline int Poly32_64::length() const {
  return len;
}

inline bool Poly32_64::isZero() const {
  return !mHead;
}

inline Poly32_64::ConstIterator Poly32_64::begin() const {
  return mHead;
}

inline Poly32_64::Iterator Poly32_64::begin() {
  return mHead;
}
