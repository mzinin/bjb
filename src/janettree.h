#pragma once

#include "triple.h"


class JanetTree
{
public:
    ~JanetTree();

    const Triple* find(const Monom& monom) const;
    void insert(Triple* triple);
    void erase(const Triple* triple);
    void clear();

    std::set<Monom::Integer> nonMulti(const Triple* triple) const;

private:
    struct Node
    {
        Monom::Integer degree;
        Triple* currentTriple = nullptr;
        Node* nextDegree = nullptr;
        Node* nextVariable = nullptr;

        Node(Monom::Integer deg);
    };

    class ConstIterator
    {
    public:
        ConstIterator(Node* node);

        void stepNextDegree();
        void stepNextVariable();
        operator bool() const;
        bool hasNextDegree() const;
        bool hasNextVariable() const;
        const Triple* triple() const;
        Monom::Integer degree() const;

    private:
        Node* currentNode_;
    };

    class Iterator
    {
    public:
        Iterator(Node*& node);

        void stepNextDegree();
        void stepNextVariable();
        operator bool() const;
        bool hasNextDegree() const;
        bool hasNextVariable() const;

        operator ConstIterator() const;
        Triple*& triple() const;
        Monom::Integer degree() const;

        void build(Monom::Integer degree, Monom::Integer var, Triple* triple);
        void erase();
        void clear();

    private:
        Node** currentNode_;
    };

private:
    Node* root_ = nullptr;
};

inline JanetTree::Node::Node(Monom::Integer deg)
    : degree(deg)
{
}

inline JanetTree::ConstIterator::ConstIterator(Node* node)
    : currentNode_(node)
{
}

inline void JanetTree::ConstIterator::stepNextDegree()
{
    currentNode_ = currentNode_->nextDegree;
}

inline void JanetTree::ConstIterator::stepNextVariable()
{
    currentNode_ = currentNode_->nextVariable;
}

inline JanetTree::ConstIterator::operator bool() const
{
    return currentNode_;
}

inline bool JanetTree::ConstIterator::hasNextDegree() const
{
    return currentNode_->nextDegree;
}

inline bool JanetTree::ConstIterator::hasNextVariable() const
{
    return currentNode_->nextVariable;
}

inline const Triple* JanetTree::ConstIterator::triple() const
{
    return currentNode_->currentTriple;
}

inline Monom::Integer JanetTree::ConstIterator::degree() const
{
    return currentNode_->degree;
}

inline JanetTree::Iterator::Iterator(Node*& node)
    : currentNode_(&node)
{
}

inline void JanetTree::Iterator::stepNextDegree()
{
    currentNode_ = &(*currentNode_)->nextDegree;
}

inline void JanetTree::Iterator::stepNextVariable()
{
    currentNode_ = &(*currentNode_)->nextVariable;
}

inline JanetTree::Iterator::operator bool() const
{
    return *currentNode_;
}

inline bool JanetTree::Iterator::hasNextDegree() const
{
    return (*currentNode_)->nextDegree;
}

inline bool JanetTree::Iterator::hasNextVariable() const
{
    return (*currentNode_)->nextVariable;
}

inline JanetTree::Iterator::operator JanetTree::ConstIterator() const
{
    return *currentNode_;
}

inline Triple*& JanetTree::Iterator::triple() const
{
    return (*currentNode_)->currentTriple;
}

inline Monom::Integer JanetTree::Iterator::degree() const
{
    return (*currentNode_)->degree;
}

inline void JanetTree::Iterator::build(Monom::Integer degree, Monom::Integer var, Triple* triple)
{
    if (!triple)
    {
        return;
    }

    Node* r =  new JanetTree::Node(triple->polynomLm()[var]);
    Node* j = r;
    while (degree > triple->polynomLm()[var])
    {
        degree = static_cast<Monom::Integer>(degree - triple->polynomLm()[var]);
        ++var;
        j->nextVariable = new JanetTree::Node(triple->polynomLm()[var]);
        j = j->nextVariable;
    }
    j->currentTriple = triple;

    r->nextDegree = *currentNode_;
    *currentNode_ = r;
}

inline void JanetTree::Iterator::erase()
{
    if (*currentNode_)
    {
        Node* tmp = *currentNode_;
        *currentNode_ = tmp->nextDegree;
        delete tmp;
    }
}

inline void JanetTree::Iterator::clear()
{
    while (hasNextDegree())
    {
        if ((*currentNode_)->nextVariable)
        {
            JanetTree::Iterator((*currentNode_)->nextVariable).clear();
        }
        erase();
    }

    if ((*currentNode_)->nextVariable)
    {
        JanetTree::Iterator((*currentNode_)->nextVariable).clear();
    }
    erase();
}

inline JanetTree::~JanetTree()
{
    clear();
    delete root_;
}
