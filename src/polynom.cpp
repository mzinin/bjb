#include "polynom.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>


FastAllocator Polynom::allocator_(sizeof(Polynom));
Monom Polynom::uniteMonom_;


Polynom::Polynom(const std::string& str)
{
    std::stringstream tmpStream(str);
    tmpStream >> *this;
}

bool Polynom::isBinomial() const
{
    Monom* iterator1 = monomListHead_;
    if (!iterator1 || iterator1->degree() != 2)
    {
        return false;
    }
    Monom* iterator2 = iterator1->next;
    if (!iterator2 || iterator2->degree() != 1)
    {
        return false;
    }
    return !(iterator2->next) && iterator1->hasSameOnlyVariable(*iterator2);
}

const Polynom& Polynom::operator+=(const Monom& newMonom)
{
    Monom** position = find(newMonom);
    Monom* tmpMonom = 0;

    if (!position)
    {
        tmpMonom = new Monom(newMonom);
        tmpMonom->next = monomListHead_;
        monomListHead_ = tmpMonom;
    }
    else
    {
        if (*position && **position == newMonom)
        {
            tmpMonom = *position;
            *position = (*position)->next;
            delete tmpMonom;
        }
        else
        {
            tmpMonom = new Monom(newMonom);
            tmpMonom->next = (*position)->next;
            (*position)->next = tmpMonom;
        }
    }

    return *this;
}

const Polynom& Polynom::operator+=(const Polynom& anotherPolynom)
{
    if (anotherPolynom.monomListHead_)
    {
        Monom **iterator = &monomListHead_,
               *iteratorAnother = anotherPolynom.monomListHead_,
               *tmpMonom = 0;

        while (*iterator && iteratorAnother)
        {
            switch (Monom::compare(**iterator, *iteratorAnother))
            {
                case -1:
                    tmpMonom = new Monom(*iteratorAnother);
                    tmpMonom->next = *iterator;
                    *iterator = tmpMonom;
                    iterator = &(tmpMonom->next);
                    iteratorAnother = iteratorAnother->next;
                    break;

                case 0:
                    tmpMonom = *iterator;
                    *iterator = (*iterator)->next;
                    delete tmpMonom;
                    iteratorAnother = iteratorAnother->next;
                    break;

                case 1:
                    iterator = &((*iterator)->next);
                    break;
            }
        }

        while (iteratorAnother)
        {
            *iterator = new Monom(*iteratorAnother);
            iterator = &((*iterator)->next);
            iteratorAnother = iteratorAnother->next;
        }
    }

    return *this;
}

void Polynom::multiply(Monom::Integer var, Monom::Integer degree)
{
    if (monomListHead_)
    {
        Monom* iterator = monomListHead_;
        while (iterator)
        {
            iterator->prolong(var, degree);
            iterator = iterator->next;
        }
    }
}

void Polynom::multiply(const Monom& anotherMonom)
{
    if (monomListHead_)
    {
        Monom* iterator = monomListHead_;
        while (iterator)
        {
            iterator->multiply(anotherMonom);
            iterator = iterator->next;
        }
    }
}

void Polynom::multiply(const Polynom& anotherPolynom)
{
    if (monomListHead_)
    {
        Polynom *tmpPolynom = nullptr,
                *tmpResult = new Polynom();
        Monom* iteratorAnother = anotherPolynom.monomListHead_;

        while (iteratorAnother)
        {
            tmpPolynom = new Polynom(*this);
            tmpPolynom->multiply(*iteratorAnother);
            tmpResult->mergeWith(*tmpPolynom);
            delete tmpPolynom;
            iteratorAnother = iteratorAnother->next;
        }
        setZero();
        monomListHead_ = tmpResult->monomListHead_;
        tmpResult->monomListHead_ = nullptr;
        delete tmpResult;
    }
}

void Polynom::reduction(const Polynom& anotherPolynom)
{
    if (monomListHead_ && anotherPolynom.monomListHead_)
    {
        Monom* tmpMonom = new Monom();
        Polynom* tmpPolynom = nullptr;
        Monom* iterator = monomListHead_;
        const Monom& anotherLm = anotherPolynom.lm();

        while (iterator)
        {
            if (iterator->isDivisibleBy(anotherLm))
            {
                tmpMonom->setQuotientOf(*iterator, anotherLm);
                tmpPolynom = new Polynom(anotherPolynom);
                tmpPolynom->multiply(*tmpMonom);
                mergeWith(*tmpPolynom);
                delete tmpPolynom;
                iterator = monomListHead_;
            }
            else
            {
                break;
            }
        }

        if (monomListHead_)
        {
            Monom* iterator2 = iterator;
            iterator = iterator->next;
            while (iterator)
            {
                if (iterator->isDivisibleBy(anotherLm))
                {
                    tmpMonom->setQuotientOf(*iterator, anotherLm);
                    tmpPolynom = new Polynom(anotherPolynom);
                    tmpPolynom->multiply(*tmpMonom);
                    mergeWith(*tmpPolynom);
                    delete tmpPolynom;
                    iterator = iterator2->next;
                }
                else
                {
                    iterator2 = iterator2->next;
                    iterator = iterator2->next;
                }
            }
        }
        delete tmpMonom;
    }
}

void Polynom::headReduction(const Polynom& anotherPolynom)
{
    if (monomListHead_ && anotherPolynom.monomListHead_)
    {
        Monom* tmpMonom = new Monom();
        Polynom* tmpPolynom = nullptr;
        Monom* iterator = monomListHead_;
        const Monom& anotherLm = anotherPolynom.lm();

        while (iterator)
        {
            if (iterator->isDivisibleBy(anotherLm))
            {
                tmpMonom->setQuotientOf(*iterator, anotherLm);
                tmpPolynom = new Polynom(anotherPolynom);
                tmpPolynom->multiply(*tmpMonom);
                mergeWith(*tmpPolynom);
                delete tmpPolynom;
                iterator = monomListHead_;
            }
            else
            {
                break;
            }
        }
        delete tmpMonom;
    }
}

void Polynom::mergeWith(Polynom& anotherPolynom)
{
    Monom **iterator = &monomListHead_,
           *iteratorAnother = anotherPolynom.monomListHead_,
           *tmpPointer = nullptr;

    while (*iterator && iteratorAnother)
    {
        switch (Monom::compare(**iterator, *iteratorAnother))
        {
        case -1:
            tmpPointer = iteratorAnother;
            iteratorAnother = iteratorAnother->next;
            tmpPointer->next = *iterator;
            *iterator = tmpPointer;
            iterator = &(tmpPointer->next);
            break;

        case 0:
            tmpPointer = *iterator;
            *iterator = (*iterator)->next;
            delete tmpPointer;
            tmpPointer = iteratorAnother;
            iteratorAnother = iteratorAnother->next;
            delete tmpPointer;
            break;

        case 1:
            iterator = &((*iterator)->next);
            break;
        }
    }

    if (iteratorAnother)
    {
        *iterator = iteratorAnother;
    }

    anotherPolynom.monomListHead_ = 0;
}

std::string Polynom::toString() const
{
    std::stringstream tmpStream;
    tmpStream << *this;
    return tmpStream.str();
}

std::ostream& operator<<(std::ostream& out, const Polynom& polynom)
{
    if (!polynom.monomListHead_)
    {
        out << "0";
    }
    else
    {
        Monom* iteratorA(polynom.monomListHead_);
        out << *iteratorA;
        iteratorA = iteratorA->next;
        while (iteratorA)
        {
            out << " + " << *iteratorA;
            iteratorA = iteratorA->next;
        }
    }

    return out;
}

std::istream& operator>>(std::istream& in, Polynom& polynom)
{
    polynom.additive(in);
    return in;
}

void Polynom::additive(std::istream& in)
{
    multiplicative(in);

    int op = (in >> std::ws).peek();
    Polynom tmpPolynom;
    while (op == '+' || op == '-')
    {
        in.get();
        tmpPolynom.multiplicative(in);
        *this += tmpPolynom;
        op = in.peek();
    }
}

void Polynom::multiplicative(std::istream& in)
{
    unary(in);

    int op = (in >> std::ws).peek();
    Polynom tmpPolynom;
    while (op == '*')
    {
        in.get();
        tmpPolynom.unary(in);
        multiply(tmpPolynom);
        op = in.peek();
    }
}

void Polynom::unary(std::istream& in)
{
    int ch = (in >> std::ws).peek();
    if (ch != '+' && ch != '-')
    {
        power(in);
    }
    else
    {
        do
        {
            ch = (in >> std::ws).peek();
        } while (ch == '+' || ch == '-');
        power(in);
    }
}

void Polynom::power(std::istream& in)
{
    bracket(in);

    int op = (in >> std::ws).peek();
    if (op == '^')
    {
        in.get();
        int degree = 0;
        in >> std::ws >> degree;
        if (in.fail() || degree <= 0)
        {
            std::cerr << "expected 'degree > 0'" << std::endl;
        }
        for (int i = 1; i < degree; ++i)
        {
            multiply(*this);
        }
    }
}

void Polynom::bracket(std::istream& in)
{
    int op = (in >> std::ws).peek();
    if (op == '(')
    {
        in.get();
        additive(in);
        if (in.get() != ')')
        {
            throw std::invalid_argument("expected ')' while polynom parsing");
        }
    }
    else
    {
        setOne();
        Monom* tmpMonom = new Monom();
        in >> *tmpMonom;
        if (!in.fail())
        {
            multiply(*tmpMonom);
            delete tmpMonom;
        }
        else
        {
            delete tmpMonom;
            in.clear();
            op = in.get();
            if (op == '0')
            {
                setZero();
            }
            else if (op != '1')
            {
                throw std::invalid_argument("expected monomial while polynom parsing");
            }
        }
    }
}
