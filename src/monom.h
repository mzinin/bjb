#pragma once

#include "fast_allocator.h"
#include "variables.h"

#include <cstdint>
#include <string>


class Monom
{
public:
    using Integer = uint16_t;

    Monom* next = nullptr;

    static const Integer invalidVariable;

public:
    Monom() = default;
    Monom(const Monom& anotherMonom);
    Monom(const std::string& str);
    ~Monom();

    std::string toString() const;

    void* operator new(std::size_t);
    void operator delete(void* ptr);

    void setOne();
    Integer degree() const;
    Integer operator[](const Integer var) const;

    const Monom& operator=(const Monom& anotherMonom);
    bool operator==(const Monom& anotherMonom) const;
    bool operator!=(const Monom& anotherMonom) const;

    void prolong(Integer var, Integer degree = 1);
    void multiply(const Monom& anotherMonom);

    void divide(const Monom& anotherMonom);
    void setQuotientOf(const Monom& monomA, const Monom& monomB);

    bool operator<(const Monom& anotherMonom) const;
    bool operator>(const Monom& anotherMonom) const;
    static int compare(const Monom& monomA, const Monom& monomB);

    bool isDivisibleBy(const Monom& anotherMonom) const;
    bool isTrueDivisibleBy(const Monom& anotherMonom) const;
    bool hasSameOnlyVariable(const Monom& anotherMonom) const;

    static Integer gcdDegree(const Monom& monomA, const Monom& monomB);
    static void addVariable(const std::string& var);
    static const std::string& variable(Integer var);
    static Integer dimIndepend();

    friend std::istream& operator>>(std::istream& in, Monom& monom);
    friend std::ostream& operator<<(std::ostream& out, const Monom& monom);

private:
    struct VarsListNode
    {
        Integer variable = 0;
        Integer degree = 0;
        VarsListNode* next = nullptr;

        void* operator new(size_t);
        void operator delete(void* ptr);

    private:
        static FastAllocator allocator_;
    };

    VarsListNode* find(const Integer var) const;

private:
    VarsListNode* listHead_ = nullptr;
    Integer totalDegree_ = 0;

    static Integer dimIndepend_;
    static Variables independVariables_;
    static FastAllocator allocator_;
};

inline void* Monom::VarsListNode::operator new(std::size_t)
{
    return allocator_.allocate();
}

inline void Monom::VarsListNode::operator delete(void* ptr)
{
    allocator_.free(ptr);
}


inline Monom::Monom(const Monom& anotherMonom)
    : totalDegree_(anotherMonom.totalDegree_)
{
    if (!anotherMonom.listHead_)
    {
        return;
    }

    totalDegree_ = anotherMonom.totalDegree_;
    VarsListNode **iterator = &listHead_,
                 *iteratorAnother = anotherMonom.listHead_;
    while (iteratorAnother)
    {
        *iterator = new VarsListNode{iteratorAnother->variable, iteratorAnother->degree, nullptr};
        iterator = &((*iterator)->next);
        iteratorAnother = iteratorAnother->next;
    }
}

inline Monom::~Monom()
{
    setOne();
}

inline void* Monom::operator new(std::size_t)
{
    return allocator_.allocate();
}

inline void Monom::operator delete(void* ptr)
{
    allocator_.free(ptr);
}

inline void Monom::setOne()
{
    totalDegree_ = 0;
    if (listHead_)
    {
        while (listHead_)
        {
            auto* tmpNode = listHead_;
            listHead_ = listHead_->next;
            delete tmpNode;
        }
    }
}

inline Monom::Integer Monom::degree() const
{
    return totalDegree_;
}

inline Monom::Integer Monom::operator[](Monom::Integer var) const
{
    const auto* varPosition = find(var);
    return (varPosition && varPosition->variable == var) ? varPosition->degree : 0;
}

inline const Monom& Monom::operator=(const Monom& anotherMonom)
{
    if (this == &anotherMonom)
    {
        return *this;
    }

    if (!anotherMonom.listHead_)
    {
        setOne();
    }
    else
    {
        totalDegree_ = anotherMonom.totalDegree_;

        VarsListNode *iteratorAnother = anotherMonom.listHead_,
                     **iterator = &listHead_;
        while (*iterator && iteratorAnother)
        {
            (*iterator)->variable = iteratorAnother->variable;
            (*iterator)->degree = iteratorAnother->degree;
            iterator = &((*iterator)->next);
            iteratorAnother = iteratorAnother->next;
        }

        if (*iterator)
        {
            VarsListNode* nodeToDelete = *iterator;
            *iterator = 0;
            while (nodeToDelete)
            {
                iteratorAnother = nodeToDelete;
                nodeToDelete = nodeToDelete->next;
                delete iteratorAnother;
            }
        }
        else while (iteratorAnother)
        {
            *iterator = new VarsListNode{iteratorAnother->variable, iteratorAnother->degree, nullptr};
            iterator = &((*iterator)->next);
            iteratorAnother = iteratorAnother->next;
        }
    }
    return *this;
}

inline bool Monom::operator==(const Monom& anotherMonom) const
{
    if (totalDegree_ != anotherMonom.totalDegree_)
    {
        return false;
    }

    VarsListNode *iterator = listHead_,
                 *anotherIterator = anotherMonom.listHead_;
    while (anotherIterator)
    {
        if (iterator->variable != anotherIterator->variable ||
            iterator->degree != anotherIterator->degree)
        {
            break;
        }
        iterator = iterator->next;
        anotherIterator = anotherIterator->next;
    }
    return !anotherIterator;
}

inline bool Monom::operator!=(const Monom& anotherMonom) const
{
    return !(*this == anotherMonom);
}

inline void Monom::prolong(Monom::Integer var, Monom::Integer degree)
{
    // inserted variable is the only one
    if (!listHead_)
    {
        listHead_ = new VarsListNode{var, degree, nullptr};
    }
    else
    {
        VarsListNode* position = find(var);
        // inserted variable is the eldest one
        if (!position)
        {
            position = new VarsListNode{var, degree, listHead_};
            listHead_ = position;
        }
        // all other cases
        else if (position->variable != var)
        {
            VarsListNode* newNode = new VarsListNode{var, degree, position->next};
            position->next = newNode;
        }
        else
        {
            position->degree = static_cast<Integer>(position->degree + degree);
        }
    }
    totalDegree_ = static_cast<Integer>(totalDegree_ + degree);
}

inline void Monom::multiply(const Monom& anotherMonom)
{
    if (!listHead_)
    {
        *this = anotherMonom;
    }
    else
    {
        if (anotherMonom.listHead_)
        {
            VarsListNode **iterator = &listHead_,
                         *anotherIterator = anotherMonom.listHead_;

            while (*iterator && anotherIterator)
            {
                if ((*iterator)->variable == anotherIterator->variable)
                {
                    (*iterator)->degree = static_cast<Integer>((*iterator)->degree + anotherIterator->degree);
                    totalDegree_ = static_cast<Integer>(totalDegree_ + anotherIterator->degree);
                    iterator = &((*iterator)->next);
                    anotherIterator = anotherIterator->next;
                }
                else if ((*iterator)->variable > anotherIterator->variable)
                {
                    iterator = &((*iterator)->next);
                }
                else
                {
                    auto* newNode = new VarsListNode{anotherIterator->variable, anotherIterator->degree, *iterator};
                    *iterator = newNode;
                    totalDegree_ = static_cast<Integer>(totalDegree_ + anotherIterator->degree);

                    iterator = &(newNode->next);
                    anotherIterator = anotherIterator->next;
                }
            }

            while (anotherIterator)
            {
                *iterator = new VarsListNode{anotherIterator->variable, anotherIterator->degree, nullptr};
                totalDegree_ = static_cast<Integer>(totalDegree_ + anotherIterator->degree);

                iterator = &((*iterator)->next);
                anotherIterator = anotherIterator->next;
            }
        }
    }
}

inline void Monom::divide(const Monom& anotherMonom)
{
    VarsListNode **iterator = &listHead_,
                 *anotherIterator = anotherMonom.listHead_;

    while (*iterator && anotherIterator)
    {
        if ((*iterator)->variable == anotherIterator->variable)
        {
            if ((*iterator)->degree == anotherIterator->degree)
            {
                VarsListNode* nodeToDelete = *iterator;
                *iterator = (*iterator)->next;
                delete nodeToDelete;
            }
            else
            {
                (*iterator)->degree = static_cast<Integer>((*iterator)->degree - anotherIterator->degree);
            }
            totalDegree_ = static_cast<Integer>(totalDegree_ - anotherIterator->degree);
            anotherIterator = anotherIterator->next;
        }
        else if ((*iterator)->variable > anotherIterator->variable)
        {
            iterator = &((*iterator)->next);
        }
    }
}

inline void Monom::setQuotientOf(const Monom& monomA, const Monom& monomB)
{
    setOne();
    VarsListNode **iterator = &listHead_,
                 *iteratorA = monomA.listHead_,
                 *iteratorB = monomB.listHead_;

    while (iteratorA && iteratorB)
    {
        if (iteratorA->variable == iteratorB->variable)
        {
            if (iteratorA->degree != iteratorB->degree)
            {
                *iterator = new VarsListNode{iteratorA->variable,
                                             static_cast<Integer>(iteratorA->degree - iteratorB->degree),
                                             nullptr};
                totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);
                iterator = &((*iterator)->next);
            }
            iteratorA = iteratorA->next;
            iteratorB = iteratorB->next;
        }
        else
        {
            *iterator = new VarsListNode{iteratorA->variable, iteratorA->degree, nullptr};
            totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);
            iterator = &((*iterator)->next);

            if (iteratorA->variable > iteratorB->variable)
            {
                iteratorA = iteratorA->next;
            }
        }
    }

    while (iteratorA)
    {
        *iterator = new VarsListNode{iteratorA->variable, iteratorA->degree, nullptr};
        totalDegree_ = static_cast<Integer>(totalDegree_ + (*iterator)->degree);
        iterator = &((*iterator)->next);

        iteratorA = iteratorA->next;
    }
}

inline bool Monom::operator<(const Monom& anotherMonom) const
{
    if (totalDegree_ < anotherMonom.totalDegree_)
    {
        return true;
    }
    if (totalDegree_ > anotherMonom.totalDegree_)
    {
        return false;
    }

    VarsListNode *iterator = listHead_,
                 *anotherIterator = anotherMonom.listHead_;
    while (anotherIterator)
    {
        if (iterator->variable < anotherIterator->variable)
        {
            return false;
        }
        if (iterator->variable > anotherIterator->variable)
        {
            return true;
        }

        if (iterator->degree < anotherIterator->degree)
        {
            return false;
        }
        else if (iterator->degree > anotherIterator->degree)
        {
            return true;
        }

        iterator = iterator->next;
        anotherIterator = anotherIterator->next;
    }
    return false;
}

inline bool Monom::operator>(const Monom& anotherMonom) const
{
    if (totalDegree_ < anotherMonom.totalDegree_)
    {
        return false;
    }
    if (totalDegree_ > anotherMonom.totalDegree_)
    {
        return true;
    }

    VarsListNode *iterator = listHead_,
                 *anotherIterator = anotherMonom.listHead_;
    while (anotherIterator)
    {
        if (iterator->variable < anotherIterator->variable)
        {
            return true;
        }
        if (iterator->variable > anotherIterator->variable)
        {
            return false;
        }

        if (iterator->degree < anotherIterator->degree)
        {
            return true;
        }
        else if (iterator->degree > anotherIterator->degree)
        {
            return false;
        }

        iterator = iterator->next;
        anotherIterator = anotherIterator->next;
    }
    return false;
}

inline int Monom::compare(const Monom& monomA, const Monom& monomB)
{
    if (monomA.totalDegree_ < monomB.totalDegree_)
    {
        return -1;
    }
    if (monomA.totalDegree_ > monomB.totalDegree_)
    {
        return 1;
    }

    VarsListNode *iteratorA(monomA.listHead_),
                 *iteratorB(monomB.listHead_);
    while (iteratorA)
    {
        if (iteratorA->variable < iteratorB->variable)
        {
            return 1;
        }
        if (iteratorA->variable > iteratorB->variable)
        {
            return -1;
        }

        if (iteratorA->degree < iteratorB->degree)
        {
            return 1;
        }
        else if (iteratorA->degree > iteratorB->degree)
        {
            return -1;
        }

        iteratorA = iteratorA->next;
        iteratorB = iteratorB->next;
    }
    return 0;
}

inline bool Monom::hasSameOnlyVariable(const Monom& anotherMonom) const
{
    return listHead_ &&
           anotherMonom.listHead_ &&
           listHead_->variable == anotherMonom.listHead_->variable &&
           !listHead_->next &&
           !anotherMonom.listHead_->next;
}

inline Monom::Integer Monom::dimIndepend()
{
    return dimIndepend_;
}

inline Monom::VarsListNode* Monom::find(const Monom::Integer var) const
{
    if (!listHead_ || listHead_->variable < var)
    {
        return 0;
    }

    VarsListNode* position = listHead_;
    while (position && position->next && position->next->variable >= var)
    {
        position = position->next;
    }
    return position;
}
