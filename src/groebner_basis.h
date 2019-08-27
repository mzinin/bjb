#pragma once

#include "qset.h"
#include "tset.h"

#include <list>


class GroebnerBasis
{
public:
    ~GroebnerBasis();

    void construct(const std::list<Polynom*>& set);

    const Polynom& operator[](size_t number) const;
    size_t length() const;

    friend std::ostream& operator<<(std::ostream& out, const GroebnerBasis& groebnerBasis);

private:
    bool criterion1(const Triple* p, const Triple* g) const;
    bool criterion2(const Triple* p, const Triple* g) const;
    bool criterion3(const Triple* p, const Triple* g) const;
    bool criterion4(const Triple* p) const;

    Polynom* normalForm(const Triple* triple) const;

    const Polynom* findDivisor(const Polynom* polynom, const std::list<Polynom*>& set) const;
    Polynom* reduce(Polynom* polynom, const std::list<Polynom*>& set) const;
    void reduceSet();
    void constructInvolutiveBasis();
    void reset();

private:
    std::list<Polynom*> gBasis_;
    TSet intermediateBasis_;
    QSet prolongationsSet_;
};
