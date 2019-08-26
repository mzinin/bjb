#pragma once

#include "fast_allocator.h"
#include "polynom.h"

#include <set>


class Triple
{
public:
    Triple(Polynom* initialPolynom);

    Triple(Polynom* initialPolynom,
           const Triple* initialAncestor,
           const std::set<Monom::Integer>& initialNmp,
           const Triple* weakAncestor,
           Monom::Integer nmVar);

    ~Triple();

    void* operator new(std::size_t);
    void operator delete(void* ptr);

    const Polynom* polynom() const;
    const Monom& polynomLm() const;
    const Triple* ancestor() const;
    const Triple* weakAncestor() const;
    Monom::Integer variable() const;
    const std::set<Monom::Integer>& nmp() const;

    void setNmp(const std::set<Monom::Integer>& newNmp);
    void setNmp(Monom::Integer variable);
    bool testNmp(Monom::Integer variable) const;

    static bool compare(const Triple* tripleA, const Triple* tripleB);

private:
    static FastAllocator allocator_;

    const Monom* lm_ = nullptr;
    Polynom* polynomial_ = nullptr;
    const Triple* ancestor_ = nullptr;
    const Triple* weakAncestor_ = nullptr;
    std::set<Monom::Integer> nmp_;
    const Monom::Integer variable_ = -1;
};


inline Triple::~Triple()
{
    delete polynomial_;
}

inline void* Triple::operator new(std::size_t)
{
    return allocator_.allocate();
}

inline void Triple::operator delete(void *ptr)
{
    allocator_.free(ptr);
}

inline const Polynom* Triple::polynom() const
{
    return polynomial_;
}

inline const Monom& Triple::polynomLm() const
{
    return *lm_;
}

inline const Triple* Triple::ancestor() const
{
    return ancestor_;
}

inline const Triple* Triple::weakAncestor() const
{
    return weakAncestor_;
}

inline Monom::Integer Triple::variable() const
{
    return variable_;
}

inline const std::set<Monom::Integer>& Triple::nmp() const
{
    return nmp_;
}

inline void Triple::setNmp(const std::set<Monom::Integer>& newNmp)
{
    nmp_ = newNmp;
}

inline void Triple::setNmp(Monom::Integer var)
{
    nmp_.insert(var);
}

inline bool Triple::testNmp(Monom::Integer var) const
{
    return nmp_.count(var);
}
