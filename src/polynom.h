#pragma once

#include "fast_allocator.h"
#include "monom.h"

#include <iostream>
#include <string>


class Polynom
{
public:
    Polynom() = default;
    Polynom(const Polynom& anotherPolynom);
    Polynom(const std::string& str);
    ~Polynom();

    void setOne();
    void setZero();
    bool isZero() const;
    bool isBinomial() const;

    unsigned long length() const;
    Monom::Integer degree() const;
    const Monom& lm() const;
    void ridOfLm();

    void* operator new(std::size_t);
    void operator delete(void *ptr);

    const Polynom& operator=(const Polynom& anotherPolynom);
    const Polynom& operator+=(const Monom& newMonom);
    const Polynom& operator+=(const Polynom& anotherPolynom);

    void multiply(Monom::Integer var, Monom::Integer degree);
    void multiply(const Monom& anotherMonom);
    void multiply(const Polynom& anotherPolynom);

    bool operator==(const Polynom &anotherPolynom) const;
    bool operator!=(const Polynom &anotherPolynom) const;
    bool operator<(const Polynom& anotherPolynom) const;
    bool operator>(const Polynom& anotherPolynom) const;
    static int compare(const Polynom& polynomA, const Polynom& polynomB);

    void reduction(const Polynom& anotherPolynom);
    void headReduction(const Polynom& anotherPolynom);

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& out, const Polynom& polynom);
    friend std::istream& operator>>(std::istream& in, Polynom& polynom);

private:
    void additive(std::istream& in);
    void multiplicative(std::istream& in);
    void unary(std::istream& in);
    void power(std::istream& in);
    void bracket(std::istream& in);

    Monom** find(const Monom& monom);
    void mergeWith(Polynom& anotherPolynom);

private:
    Monom* monomListHead_ = nullptr;

    static FastAllocator allocator_;
    static Monom uniteMonom_;
};

inline Polynom::Polynom(const Polynom& anotherPolynom)
{
    if (!anotherPolynom.monomListHead_)
    {
        return;
    }

    Monom **iterator = &monomListHead_,
          *iteratorAnother = anotherPolynom.monomListHead_;
    while (iteratorAnother)
    {
        *iterator = new Monom(*iteratorAnother);

        iterator = &((*iterator)->next);
        iteratorAnother = iteratorAnother->next;
    }
}

inline Polynom::~Polynom()
{
    setZero();
}

inline void Polynom::setOne()
{
    setZero();
    monomListHead_ = new Monom();
}

inline void Polynom::setZero()
{
    if (monomListHead_)
    {
        while (monomListHead_)
        {
            auto* tmpMonom = monomListHead_;
            monomListHead_ = monomListHead_->next;
            delete tmpMonom;
        }
    }
}

inline bool Polynom::isZero() const
{
    return !monomListHead_;
}

inline unsigned long Polynom::length() const
{
    unsigned long length = 0;
    Monom* iterator = monomListHead_;
    while (iterator)
    {
        iterator = iterator->next;
        ++length;
    }
    return length;
}

inline Monom::Integer Polynom::degree() const
{
    return monomListHead_ ? monomListHead_->degree() : 0;
}

inline const Monom& Polynom::lm() const
{
    return monomListHead_ ? *monomListHead_ : uniteMonom_;
}

inline void Polynom::ridOfLm()
{
    if (monomListHead_)
    {
        auto* tmpMonom = monomListHead_;
        monomListHead_ = monomListHead_->next;
        delete tmpMonom;
    }
}

inline void* Polynom::operator new(std::size_t)
{
    return allocator_.allocate();
}

inline void Polynom::operator delete(void *ptr)
{
    allocator_.free(ptr);
}

inline const Polynom& Polynom::operator=(const Polynom& anotherPolynom)
{
    if (!anotherPolynom.monomListHead_)
    {
        setZero();
    }
    else
    {
        Monom *iteratorAnother = anotherPolynom.monomListHead_,
              **iterator = &monomListHead_;
        while (*iterator && iteratorAnother)
        {
            **iterator = *iteratorAnother;
            iterator = &((*iterator)->next);
            iteratorAnother = iteratorAnother->next;
        }

        if (*iterator)
        {
            Monom* monomToDelete = (*iterator)->next;
            *iterator = nullptr;
            while (monomToDelete)
            {
                iteratorAnother = monomToDelete;
                monomToDelete = monomToDelete->next;
                delete iteratorAnother;
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

inline bool Polynom::operator==(const Polynom& anotherPolynom) const
{
    Monom *iterator = monomListHead_,
          *anotherIterator = anotherPolynom.monomListHead_;

    while (iterator && anotherIterator)
    {
        if (*iterator != *anotherIterator)
        {
            break;
        }
        iterator = iterator->next;
        anotherIterator = anotherIterator->next;
    }
    return !iterator && !anotherIterator;
}

inline bool Polynom::operator!=(const Polynom& anotherPolynom) const
{
    Monom *iterator = monomListHead_,
          *anotherIterator = anotherPolynom.monomListHead_;

    while (iterator && anotherIterator)
    {
        if (*iterator != *anotherIterator)
        {
            break;
        }
        iterator = iterator->next;
        anotherIterator = anotherIterator->next;
    }
    return iterator || anotherIterator;
}

inline bool Polynom::operator<(const Polynom& anotherPolynom) const
{
    Monom *iterator = monomListHead_,
          *anotherIterator = anotherPolynom.monomListHead_;

    while (iterator && anotherIterator)
    {
        switch (Monom::compare(*iterator, *anotherIterator))
        {
            case -1:
                return true;

            case 1:
                return false;

            case 0:
                iterator = iterator->next;
                anotherIterator = anotherIterator->next;
                break;
        }
    }
    return !iterator && !anotherIterator;
}

inline bool Polynom::operator>(const Polynom& anotherPolynom) const
{
    Monom *iterator = monomListHead_,
          *anotherIterator = anotherPolynom.monomListHead_;

    while (iterator && anotherIterator)
    {
        switch (Monom::compare(*iterator, *anotherIterator))
        {
            case -1:
                return false;

            case 1:
                return true;

            case 0:
                iterator = iterator->next;
                anotherIterator = anotherIterator->next;
                break;
        }
    }
    return iterator && !anotherIterator;
}

inline int Polynom::compare(const Polynom& polynomA, const Polynom& polynomB)
{
    Monom *iteratorA = polynomA.monomListHead_,
          *iteratorB = polynomB.monomListHead_;

    while (iteratorA && iteratorB)
    {
        switch (Monom::compare(*iteratorA, *iteratorB))
        {
            case -1:
                return -1;

            case 1:
                return 1;

            case 0:
                iteratorA = iteratorA->next;
                iteratorB = iteratorB->next;
                break;
        }
    }

    if (iteratorA)
    {
        return 1;
    }
    if (iteratorB)
    {
        return -1;
    }
    return 0;
}

inline Monom** Polynom::find(const Monom& monom)
{
    if (!monomListHead_ || *monomListHead_ < monom)
    {
        return nullptr;
    }

    Monom** previousPointer = &monomListHead_;
    Monom** currentPointer = nullptr;
    unsigned long range(length()), middle;

    while ((middle = range >> 1) > 0)
    {
        currentPointer = previousPointer;
        for (unsigned long i = 0; i < middle; ++i)
        {
            currentPointer = &((*currentPointer)->next);
        }

        switch (Monom::compare(**currentPointer, monom))
        {
        case 1:
            previousPointer = currentPointer;
            range -= middle;
            break;

        case -1:
            range = middle;
            break;

        case 0:
            return currentPointer;
        }
    }

    return previousPointer;
}
