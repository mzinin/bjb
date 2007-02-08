#include <iostream>
#include "imymonom128.h"

class IMyPoly128;
class IMyPolyInterface128 {
  friend class IMyPoly128;

protected:
  IMyMonomInterface128 *mMonomInterface;

public:
  IMyPolyInterface128();
  static IMyPolyInterface128* create(IMyMonomInterface128* monomInterface);
  ~IMyPolyInterface128() {}

  IMyMonomInterface128* monomInterface() const { return mMonomInterface; }
  IMyPoly128* create();
  IMyPoly128* copy(const IMyPoly128& a);
  IMyPoly128* copy(const IMyPoly128& a, int var);
};

class IMyPoly128 {
  friend class IMyPolyInterface128;

protected:
  typedef IMyMonom128* It;
  typedef const IMyMonom128* ConstIt;
  IMyMonom128*  mHead;
  IMyPolyInterface128* mRealization;
  int len;

  void additive(std::istream& in);
  void multiplicative(std::istream& in);
  void unary(std::istream& in);
  void power(std::istream& in);
  void bracket(std::istream& in);

public:
  IMyPoly128(IMyPolyInterface128* r);
  IMyPoly128(const IMyPoly128& a);
  IMyPoly128(const IMyPoly128& a, int var);
  IMyPoly128(const IMyPoly128& a, const IMyMonom128& m);

  IMyMonomInterface128* monomInterface() const { return mRealization->mMonomInterface; }

  class Iterator;
  class ConstIterator {
    friend class IMyPoly128;
    friend class Iterator;

  protected:
    IMyPoly128::ConstIt mConstIt;

  public:
    ConstIterator(): mConstIt(NULL) {};
    ConstIterator(const IMyPoly128::ConstIt constIt):mConstIt(constIt) {}
    ~ConstIterator() {}
    void operator=(const ConstIterator& a) { mConstIt = a.mConstIt; }
    operator bool() const { return mConstIt; }
    bool isNext() const { return (mConstIt->Next != NULL); }
    const IMyMonom128& monom() const { return *mConstIt; }
    const IMyMonom128& operator*() const { return *mConstIt; }
    const IMyMonom128* operator->() const { return mConstIt; }
    void operator++() { mConstIt = mConstIt->Next; }
    void operator++(int i) { mConstIt = mConstIt->Next; }
    bool operator==(const ConstIterator& a) const { return mConstIt == a.mConstIt;; }
    bool operator!=(const ConstIterator& a) const { return mConstIt != a.mConstIt; }
  };

  class Iterator {
    friend class IMyPoly128;
    friend class ConstIterator;

  protected:
    IMyPoly128::It *mIt;

  public:
    Iterator(): mIt(NULL) {};
    Iterator(IMyPoly128::It &it): mIt(&it) {}
    ~Iterator() {}
    void operator=(const Iterator& a) { mIt = a.mIt; }
    operator bool() const { return *mIt; }
    bool isNext() const { return ((*mIt)->Next != NULL); }
    const IMyMonom128& monom() const { return **mIt; }
    IMyMonom128& operator*() const { /*IASSERT(mIt); IASSERT(*mIt);*/ return **mIt; }
    IMyMonom128* operator->() const { /*IASSERT(mIt); IASSERT(*mIt);*/ return *mIt; }
    void operator++() { mIt = &(*mIt)->Next; }
    void operator++(int i) { mIt = &(*mIt)->Next; }
    bool operator==(const Iterator& a) const { return *mIt == *a.mIt; }
    bool operator!=(const Iterator& a) const { return *mIt != *a.mIt; }
    void insert(IMyMonom128 &m) {
      //IASSERT(mIt);
      m.Next = *mIt;
      *mIt = &m;
    }
    void insert(ConstIterator j) {
      //IASSERT(mIt);
      IMyMonom128 *tmp = const_cast<IMyMonom128*>(j.mConstIt);
      tmp->Next = *mIt;
      *mIt = tmp;
      tmp = NULL;
    }
    ConstIterator get() {
      //IASSERT(mIt);
      ConstIterator j(*mIt);
      *mIt = (*mIt)->Next;
      IMyMonom128 *tmp = const_cast<IMyMonom128*>(j.mConstIt);
      tmp->Next = NULL;
      tmp = NULL;
      return j;
    }
    void del() {
      //IASSERT(mIt);
      IMyMonom128 *tmp = *mIt;
      *mIt = (*mIt)->Next;
      //tmp->Next = NULL;
      delete tmp;
    }
    void clear() {
      while (*mIt)
        del();
    }
  };


  ~IMyPoly128() { begin().clear(); }
  IMyPolyInterface128* polyInterface() const { return mRealization; }

  void setOne();
  void setZero();
  void set(const IMyPoly128& a);
  void operator=(const IMyPoly128& a) { set(a); }

  void swap(IMyPoly128& a);
  bool isZero() const;
  int length() const;

  int degree() const;
  int degree_of_monom(int i) const;
  int deg(int var);

  const IMyMonom128& lm() const;
  void rid_of_lm();

  void add(const IMyMonom128 &m);
  void add(IMyPoly128& a);
  void sub(IMyPoly128& a) { add(a); }

  void mult(int var);
  void mult(int var, unsigned deg);
  void mult(const IMyMonom128& m);
  void mult(const IMyPoly128 &a);

  void pow(unsigned deg);
  void reduction(const IMyPoly128& a);
  void reduction1(const IMyPoly128& a);

  ConstIterator begin() const;
  Iterator begin();

  friend std::ostream& operator<<(std::ostream& out, const IMyPoly128& a);
  friend std::istream& operator>>(std::istream& in, IMyPoly128& a);
  friend bool operator==(const IMyPoly128 &a, const IMyPoly128 &b);
  friend bool operator!=(const IMyPoly128 &a, const IMyPoly128 &b) {return !(a==b);}

  void RawCopy(IMyPoly128& a);
  void assertValid(const char* fileName, int fileLine) const;
};

inline const IMyMonom128& IMyPoly128::lm() const {
  //IASSERT(!isZero());
  return *mHead;
}

inline int IMyPoly128::length() const {
  return len;
}

inline bool IMyPoly128::isZero() const {
  return !mHead;
}
