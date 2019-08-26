#include "tset.h"

#include <set>


void TSet::clear()
{
    jTree_.clear();

    for (auto* triple : tripleList_)
    {
        delete triple;
    }
    tripleList_.clear();
}

void TSet::collectNonMultiProlongations(TSet::Iterator& iterator, std::list<Triple*>& set)
{
    if (iterator == tripleList_.end() || !(*iterator))
    {
        return;
    }

    auto nonMultiVars = jTree_.nonMulti(*iterator);
    for (auto nmvIterator = nonMultiVars.begin(); nmvIterator != nonMultiVars.end(); ++nmvIterator)
    {
        if (!(**iterator).testNmp(*nmvIterator))
        {
            (**iterator).setNmp(*nmvIterator);
            Polynom* tmpPolynom = new Polynom(*(**iterator).polynom());

            tmpPolynom->multiply(*nmvIterator, 1);
            set.push_back(new Triple(tmpPolynom,
                                     (**iterator).ancestor(),
                                     (**iterator).nmp(),
                                     (*iterator),
                                     *nmvIterator));
            delete tmpPolynom;
        }
    }
}
