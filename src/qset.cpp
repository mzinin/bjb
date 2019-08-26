#include "qset.h"


void QSet::insert(const std::list<Polynom*>& addList)
{
    std::list<Polynom*>::const_iterator itBasis(addList.begin());
    while (itBasis != addList.end())
    {
        if (*itBasis)
        {
            tripleList_.push_back(new Triple(*itBasis));
        }
        ++itBasis;
    }
    tripleList_.sort(Triple::compare);
}

void QSet::insert(std::list<Triple*>& addList)
{
    addList.sort(Triple::compare);
    tripleList_.merge(addList, Triple::compare);
}

void QSet::clear()
{
    for (auto* item : tripleList_)
    {
        delete item;
    }
    tripleList_.clear();
}

void QSet::deleteDescendants(const Triple* ancestor)
{
    if (!ancestor)
    {
        return;
    }

    for (auto it = tripleList_.begin(); it != tripleList_.end();)
    {
        if ((**it).ancestor() == ancestor || (**it).weakAncestor() == ancestor)
        {
            delete *it;
            it = tripleList_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
