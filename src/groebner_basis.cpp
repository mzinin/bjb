#include "groebner_basis.h"
#include "pcomparator.h"
#include "resource_counter.h"
#include "settings_manager.h"
#include "timer.h"

#include <iostream>
#include <set>


GroebnerBasis::~GroebnerBasis()
{
    reset();
}

void GroebnerBasis::construct(const std::list<Polynom*>& set)
{
    reset();
    gBasis_ = set;

    for (Monom::Integer i = 0; i < Monom::dimIndepend(); ++i)
    {
        Polynom* binomial = new Polynom();
        binomial->setOne();
        binomial->multiply(i, 1);
        (*binomial) += Monom();
        binomial->multiply(i, 1);
        gBasis_.push_back(binomial);
    }

    getResourceCounter().autoReductionTimer.start();
    reduceSet();
    getResourceCounter().autoReductionTimer.stop();

    prolongationsSet_.insert(gBasis_);
    gBasis_.clear();
    constructInvolutiveBasis();
    prolongationsSet_.clear();

    for (auto it = intermediateBasis_.begin(); it != intermediateBasis_.end(); ++it)
    {
        if ((*it)->ancestor() == *it)
        {
            gBasis_.push_back(const_cast<Polynom*>((**it).polynom()));
        }
    }
    reduceSet();

    std::list<Polynom*>::iterator gbIterator = gBasis_.begin();
    while (gbIterator != gBasis_.end())
    {
        if ((**gbIterator).isBinomial())
        {
            gbIterator = gBasis_.erase(gbIterator);
        }
        else
        {
            ++gbIterator;
        }
    }
}

const Polynom& GroebnerBasis::operator[](size_t num) const
{
    std::list<Polynom*>::const_iterator it(gBasis_.begin());
    for (size_t i = length() - 1 - num; i > 0; --i)
    {
        ++it;
    }
    return **it;
}

size_t GroebnerBasis::length() const
{
    return gBasis_.size();
}

bool GroebnerBasis::criterion1(const Triple* p, const Triple* g) const
{
    const Monom& pAncestorLm = p->ancestor()->polynomLm();
    const Monom& gAncestorLm = g->ancestor()->polynomLm();
    Monom tmpMonom = pAncestorLm;
    tmpMonom.multiply(gAncestorLm);

    return !Monom::gcdDegree(pAncestorLm, gAncestorLm) && p->polynomLm() == tmpMonom;
}

bool GroebnerBasis::criterion2(const Triple* p, const Triple* g) const
{
    Monom tmpMonom = p->ancestor()->polynomLm();
    tmpMonom.multiply(g->ancestor()->polynomLm());

    return p->polynomLm().isTrueDivisibleBy(tmpMonom);
}

bool GroebnerBasis::criterion3(const Triple* p, const Triple* g) const
{
    Monom tmpMonom = p->ancestor()->polynomLm();
    tmpMonom.multiply(g->ancestor()->polynomLm());

    for (auto iterator = intermediateBasis_.begin(); iterator != intermediateBasis_.end(); ++iterator)
    {
        Monom monomP = p->ancestor()->polynomLm();
        monomP.multiply((**iterator).polynomLm());
        Monom monomG = g->ancestor()->polynomLm();
        monomG.multiply((**iterator).polynomLm());

        if (tmpMonom.isTrueDivisibleBy(monomP) && tmpMonom.isTrueDivisibleBy(monomG))
        {
            return true;
        }
    }

    return false;
}

bool GroebnerBasis::criterion4(const Triple* p) const
{
    for (auto iterator = intermediateBasis_.begin();
         iterator != intermediateBasis_.end() && p->weakAncestor() != *iterator;
         ++iterator)
    {
        if ((**iterator).polynom()->degree() != p->polynom()->degree() - 1)
        {
            continue;
        }

        Monom tmpMonom1 = p->ancestor()->polynomLm();
        tmpMonom1.multiply((**iterator).ancestor()->polynomLm());

        std::set<Monom::Integer> nmv = intermediateBasis_.nonMulti(*iterator);
        for (const auto& var : nmv)
        {
            Monom tmpMonom2 = (**iterator).polynomLm();
            tmpMonom2.prolong(var);
            if (tmpMonom2 == p->polynomLm() && tmpMonom2.isTrueDivisibleBy(tmpMonom1))
            {
                return true;
            }
        }
    }
    return false;
}

Polynom* GroebnerBasis::normalForm(const Triple* triple) const
{
    if (!triple)
    {
        return 0;
    }

    if (triple->variable() != Monom::invalidVariable)
    {
        const Triple* involutiveDivisor = intermediateBasis_.findDivisor(triple->polynomLm());
        if (involutiveDivisor &&
            (criterion1(triple, involutiveDivisor) ||
             criterion2(triple, involutiveDivisor)/* ||
             criterion3(triple, involutiveDivisor) ||
             criterion4(triple)*/))
        {
            return 0;
        }
    }

    Polynom* originalForm = nullptr;
    if (triple->variable() == Monom::invalidVariable)
    {
        originalForm = new Polynom(*triple->polynom());
    }
    else
    {
        originalForm = new Polynom(*triple->weakAncestor()->polynom());
        originalForm->multiply(triple->variable(), 1);
    }

    if (getSettingsManager().collectStatistics())
    {
        ++getResourceCounter().nonMultiProlongations;
        getResourceCounter().nonMultiProlongationsLength += originalForm->length();
    }

    Polynom* normalForm = new Polynom();
    while (!originalForm->isZero())
    {
        const Triple* involutiveDivisor = intermediateBasis_.findDivisor(originalForm->lm());
        while (involutiveDivisor)
        {
            originalForm->headReduction(*involutiveDivisor->polynom());
            if (!originalForm->isZero())
            {
                involutiveDivisor = intermediateBasis_.findDivisor(originalForm->lm());
            }
            else
            {
                involutiveDivisor = nullptr;
            }
        }

        if (!originalForm->isZero())
        {
            (*normalForm) += originalForm->lm();
            originalForm->ridOfLm();
        }
    }

    delete originalForm;
    return normalForm;
}

const Polynom* GroebnerBasis::findDivisor(const Polynom* polynom, const std::list<Polynom*>& set) const
{
    if (!polynom || polynom->isZero())
    {
        return nullptr;
    }

    const Monom& plm = polynom->lm();
    for (const auto* polynomPtr : set)
    {
        if (plm.isDivisibleBy(polynomPtr->lm()))
        {
            return polynomPtr;
        }
    }

    return nullptr;
}

Polynom* GroebnerBasis::reduce(Polynom* polynom, const std::list<Polynom*>& set) const
{
    if (!polynom)
    {
        return nullptr;
    }

    Polynom* result = new Polynom();
    const Polynom* currentReducer = nullptr;

    while (!polynom->isZero())
    {
        currentReducer = findDivisor(polynom, set);
        while (currentReducer)
        {
            polynom->reduction(*currentReducer);
            currentReducer = findDivisor(polynom, set);
        }
        if (!polynom->isZero())
        {
            (*result) += polynom->lm();
            polynom->ridOfLm();
        }
    }

    polynom = result;
    return result;
}

void GroebnerBasis::reduceSet()
{
    std::list<Polynom*> tmpPolySet;
    gBasis_.sort(PointerGreaterComparator<Polynom>());

    while (!gBasis_.empty())
    {
        Polynom* currentPolynom = gBasis_.front();
        gBasis_.pop_front();
        currentPolynom = reduce(currentPolynom, tmpPolySet);

        if (currentPolynom && !currentPolynom->isZero())
        {
            const Monom& hLm = currentPolynom->lm();
            for (auto iteratorTmpPolySet = tmpPolySet.begin(); iteratorTmpPolySet != tmpPolySet.end();)
            {
                if ((**iteratorTmpPolySet).lm().isDivisibleBy(hLm))
                {
                    gBasis_.push_back(*iteratorTmpPolySet);
                    iteratorTmpPolySet = tmpPolySet.erase(iteratorTmpPolySet);
                }
                else
                {
                    ++iteratorTmpPolySet;
                }
            }
            tmpPolySet.push_back(currentPolynom);
        }
    }

    size_t tmpPolySetSize = tmpPolySet.size();
    for (size_t i = 0; i < tmpPolySetSize; ++i)
    {
        Polynom* currentPolynom = tmpPolySet.front();
        tmpPolySet.pop_front();
        currentPolynom = reduce(currentPolynom, tmpPolySet);
        if (!currentPolynom || currentPolynom->isZero())
        {
            --tmpPolySetSize;
        }
        else
        {
            tmpPolySet.push_back(currentPolynom);
        }
    }

    gBasis_ = tmpPolySet;
}

void GroebnerBasis::constructInvolutiveBasis()
{
    Polynom* newNormalForm = nullptr;
    const Triple* currentTriple = nullptr;

    while (!prolongationsSet_.empty())
    {
        currentTriple = prolongationsSet_.get();
        newNormalForm = normalForm(currentTriple);

        std::set<Monom::Integer> currentNmpSet;
        const Triple* currentAncestor = nullptr;
        if (newNormalForm && !newNormalForm->isZero() && newNormalForm->lm() == currentTriple->polynomLm())
        {
            currentNmpSet = currentTriple->nmp();
            currentAncestor = currentTriple->ancestor();
            if (currentAncestor == currentTriple)
            {
                currentAncestor = nullptr;
            }
        }
        delete currentTriple;

        if (newNormalForm && !newNormalForm->isZero())
        {
            if (getSettingsManager().collectStatistics())
            {
                ++getResourceCounter().nonZeroReductions;
                getResourceCounter().nonZeroReductionsLength += newNormalForm->length();
            }

            std::list<Triple*> newProlongations;
            for (auto tit = intermediateBasis_.begin(); tit != intermediateBasis_.end();)
            {
                if ((**tit).polynomLm().isTrueDivisibleBy(newNormalForm->lm()))
                {
                    prolongationsSet_.deleteDescendants(*tit);
                    newProlongations.push_back(*tit);
                    tit = intermediateBasis_.erase(tit);
                }
                else
                {
                    ++tit;
                }
            }

            intermediateBasis_.pushBack(new Triple(newNormalForm, currentAncestor, currentNmpSet, 0, Monom::invalidVariable));
            if (!newNormalForm->degree())
            {
                return;
            }

            for (auto tit = intermediateBasis_.begin(); tit != intermediateBasis_.end(); ++tit)
            {
                intermediateBasis_.collectNonMultiProlongations(tit, newProlongations);
            }
            prolongationsSet_.insert(newProlongations);
        }
        else
        {
            delete newNormalForm;
        }
    }
}

void GroebnerBasis::reset()
{
    intermediateBasis_.clear();
    prolongationsSet_.clear();
    gBasis_.clear();
    getResourceCounter().nonMultiProlongations = 0;
    getResourceCounter().nonZeroReductions = 0;
}

std::ostream& operator<<(std::ostream& out, const GroebnerBasis& groebnerBasis)
{
    for (unsigned i = 0; i < groebnerBasis.length(); ++i)
    {
        out << '[' << i << "] = " << groebnerBasis[i] << '\n';
    }

    return out;
}
