#include "monom.h"

#include <algorithm>
#include <sstream>


const Monom::Integer Monom::invalidVariable = static_cast<Monom::Integer>(-1);
FastAllocator Monom::VarsListNode::allocator_(sizeof(Monom::VarsListNode));
FastAllocator Monom::allocator_(sizeof(Monom));
Monom::Integer Monom::dimIndepend_ = 0;
Variables Monom::independVariables_;


Monom::Monom(const std::string& str)
{
    std::stringstream tmpStream(str);
    tmpStream >> *this;
}

std::string Monom::toString() const
{
    std::stringstream tmpStream;
    tmpStream << *this;
    return tmpStream.str();
}

void Monom::addVariable(const std::string& var)
{
    if (independVariables_.add(var))
    {
        ++dimIndepend_;
    }
}

const std::string& Monom::variable(Monom::Integer var)
{
    return independVariables_.variable(var);
}

bool Monom::isDivisibleBy(const Monom& anotherMonom) const
{
    VarsListNode *iterator = listHead_,
                 *anotherIterator = anotherMonom.listHead_;
    while (iterator && anotherIterator)
    {
        if (iterator->variable == anotherIterator->variable)
        {
            if (iterator->degree < anotherIterator->degree)
            {
                break;
            }
            iterator = iterator->next;
            anotherIterator = anotherIterator->next;
        }
        else if (iterator->variable > anotherIterator->variable)
        {
            iterator = iterator->next;
        }
        else
        {
            break;
        }
    }

    return !anotherIterator;
}

bool Monom::isTrueDivisibleBy(const Monom& anotherMonom) const
{
    if (totalDegree_ <= anotherMonom.totalDegree_)
    {
        return false;
    }
    return isDivisibleBy(anotherMonom);
}

Monom::Integer Monom::gcdDegree(const Monom& monomA, const Monom& monomB)
{
    Integer result = 0;
    VarsListNode *iteratorA(monomA.listHead_),
                 *iteratorB(monomB.listHead_);
    while (iteratorA && iteratorB)
    {
        if (iteratorA->variable == iteratorB->variable)
        {
            result = static_cast<Integer>(result + std::min(iteratorA->degree, iteratorB->degree));
            iteratorA = iteratorA->next;
            iteratorB = iteratorB->next;
        }
        else if (iteratorA->variable > iteratorB->variable)
        {
            iteratorA = iteratorA->next;
        }
        else
        {
            iteratorB = iteratorB->next;
        }
    }
    return result;
}

Monom::Integer Monom::lcmDegree(const Monom& monomA, const Monom& monomB)
{
    Integer result = 0;
    VarsListNode *iteratorA(monomA.listHead_),
                 *iteratorB(monomB.listHead_);
    while (iteratorA && iteratorB)
    {
        if (iteratorA->variable == iteratorB->variable)
        {
            result = static_cast<Integer>(result + std::max(iteratorA->degree, iteratorB->degree));
            iteratorA = iteratorA->next;
            iteratorB = iteratorB->next;
        }
        else if (iteratorA->variable > iteratorB->variable)
        {
            result = static_cast<Integer>(result + iteratorA->degree);
            iteratorA = iteratorA->next;
        }
        else
        {
            result = static_cast<Integer>(result + iteratorB->degree);
            iteratorB = iteratorB->next;
        }
    }

    while (iteratorA)
    {
        result = static_cast<Integer>(result + iteratorA->degree);
        iteratorA = iteratorA->next;
    }

    while (iteratorB)
    {
        result = static_cast<Integer>(result + iteratorB->degree);
        iteratorB = iteratorB->next;
    }

    return result;
}

void Monom::setGcdOf(const Monom& monomA, const Monom& monomB)
{
    setOne();
    VarsListNode **iterator = &listHead_,
                 *iteratorA(monomA.listHead_),
                 *iteratorB(monomB.listHead_);
    while (iteratorA && iteratorB)
    {
        if (iteratorA->variable == iteratorB->variable)
        {
            *iterator = new VarsListNode{iteratorA->variable, std::min(iteratorA->degree, iteratorB->degree), nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorA = iteratorA->next;
            iteratorB = iteratorB->next;
        }
        else if (iteratorA->variable > iteratorB->variable)
        {
            iteratorA = iteratorA->next;
        }
        else
        {
            iteratorB = iteratorB->next;
        }
    }
}

void Monom::setLcmOf(const Monom& monomA, const Monom& monomB)
{
    setOne();
    VarsListNode **iterator = &listHead_,
                 *iteratorA(monomA.listHead_),
                 *iteratorB(monomB.listHead_);
    while (iteratorA && iteratorB)
    {
        if (iteratorA->variable == iteratorB->variable)
        {
            *iterator = new VarsListNode{iteratorA->variable, std::max(iteratorA->degree, iteratorB->degree), nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorA = iteratorA->next;
            iteratorB = iteratorB->next;
        }
        else if (iteratorA->variable > iteratorB->variable)
        {
            *iterator = new VarsListNode{iteratorA->variable, iteratorA->degree, nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorA = iteratorA->next;
        }
        else
        {
            *iterator = new VarsListNode{iteratorB->variable, iteratorB->degree, nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorB = iteratorB->next;
        }
    }

    while (iteratorA)
    {
            *iterator = new VarsListNode{iteratorA->variable, iteratorA->degree, nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorA = iteratorA->next;
    }

    while (iteratorB)
    {
            *iterator = new VarsListNode{iteratorB->variable, iteratorB->degree, nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);

            iterator = &((*iterator)->next);
            iteratorB = iteratorB->next;
    }
}

std::istream& operator>>(std::istream& in, Monom& monom)
{
    std::streampos posbeg = in.tellg();
    int var = monom.independVariables_.read(in);
    if (var < 0)
    {
        in.clear();
        in.setstate(std::ios::failbit);
    }
    else
    {
        monom.setOne();
        int degree;
        do
        {
            degree = 1;
            posbeg = in.tellg();
            if ((in >> std::ws).peek() == '^')
            {
                in.get();
                in >> std::ws >> degree;
                if (in.fail() || degree < 0)
                {
                    in.setstate(std::ios::failbit);
                }
            }
            monom.prolong(static_cast<Monom::Integer>(var), static_cast<Monom::Integer>(degree));
            posbeg = in.tellg();
            if (in.peek() != '*')
            {
                var = -1;
            }
            else
            {
                in.get();
                var = monom.independVariables_.read(in);
                if (var < 0)
                {
                    in.clear();
                    in.seekg(posbeg);
                }
            }
        } while (var >= 0);

        if (in.eof() && degree >= 0)
        {
            in.clear();
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const Monom& monom)
{
    if (!monom.degree())
    {
        out << '1';
    }
    else
    {
        Monom::Integer i = 0;
        Variables::ConstIterator j(monom.independVariables_.begin());
        while (monom[i] == 0)
        {
            ++i;
            ++j;
        }
        out << *j;
        if (monom[i] > 1)
        {
            out << '^' << monom[i];
        }
        ++i;
        ++j;
        while (j != monom.independVariables_.end())
        {
            if (monom[i])
            {
                out << '*' << *j;
                if (monom[i] > 1)
                {
                    out << '^' << monom[i];
                }
            }
            ++i;
            ++j;
        }
    }
    return out;
}
