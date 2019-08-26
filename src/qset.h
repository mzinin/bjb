#pragma once

#include "triple.h"

#include <list>


class QSet
{
public:
    QSet() = default;
    QSet(const std::list<Polynom*>& basis);
    ~QSet();

    void insert(const std::list<Polynom*>& addList);
    void insert(std::list<Triple*>& addList);

    Triple* get();

    void clear();
    bool empty() const;
    std::size_t size() const;
    void deleteDescendants(const Triple* ancestor);

private:
    std::list<Triple*> tripleList_;
};

inline QSet::QSet(const std::list<Polynom*>& basis)
{
    std::list<Polynom*>::const_iterator itBasis(basis.begin());
    while (itBasis != basis.end())
    {
        if (*itBasis)
        {
            tripleList_.push_back(new Triple(*itBasis));
        }
        ++itBasis;
    }
    tripleList_.sort(Triple::compare);
}

inline QSet::~QSet()
{
    clear();
}

inline Triple* QSet::get()
{
    Triple* result = tripleList_.back();
    tripleList_.pop_back();
    return result;
}

inline bool QSet::empty() const
{
    return tripleList_.empty();
}

inline std::size_t QSet::size() const
{
    return tripleList_.size();
}
