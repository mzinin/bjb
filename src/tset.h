#pragma once

#include "janettree.h"
#include <list>


class TSet
{
public:
    using Iterator = std::list<Triple*>::iterator;
    using ConstIterator = std::list<Triple*>::const_iterator;

public:
    TSet() = default;
    ~TSet();

    Iterator begin();
    ConstIterator begin() const;
    Iterator end();
    ConstIterator end() const;

    void clear();
    Iterator erase(Iterator it);
    void pushBack(Triple* newTriple);
    std::size_t size() const;

    const Triple* findDivisor(const Monom& monom) const;
    std::set<Monom::Integer> nonMulti(const Triple* triple) const;
    Triple* const back() const;

    void collectNonMultiProlongations(Iterator& iterator, std::list<Triple*>& set);

private:
    std::list<Triple*> tripleList_;
    JanetTree jTree_;
};

inline TSet::~TSet()
{
    clear();
}

inline TSet::Iterator TSet::begin()
{
    return tripleList_.begin();
}

inline TSet::ConstIterator TSet::begin() const
{
    return tripleList_.begin();
}

inline TSet::Iterator TSet::end()
{
    return tripleList_.end();
}

inline TSet::ConstIterator TSet::end() const
{
    return tripleList_.end();
}

inline TSet::Iterator TSet::erase(TSet::Iterator it)
{
    jTree_.erase(*it);
    return tripleList_.erase(it);
}

inline void TSet::pushBack(Triple* newTriple)
{
    tripleList_.push_back(newTriple);
    jTree_.insert(newTriple);
}

inline std::size_t TSet::size() const
{
    return tripleList_.size();
}

inline const Triple* TSet::findDivisor(const Monom& monom) const
{
    return jTree_.find(monom);
}

inline std::set<Monom::Integer> TSet::nonMulti(const Triple* triple) const
{
    return jTree_.nonMulti(triple);
}

inline Triple* const TSet::back() const
{
    return tripleList_.back();
}
