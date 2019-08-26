#include "triple.h"


FastAllocator Triple::allocator_(sizeof(Triple));


Triple::Triple(Polynom* initialPolynom)
    : polynomial_(initialPolynom)
{
    if (polynomial_)
    {
        lm_ = &polynomial_->lm();
        ancestor_ = this;
    }
}

Triple::Triple(Polynom* initialPolynom,
               const Triple* initialAncestor,
               const std::set<Monom::Integer>& initialNmp,
               const Triple* weakAncestor,
               Monom::Integer nmVar)
    : variable_(nmVar)
{
    if (initialPolynom)
    {
        if (variable_ == -1)
        {
            polynomial_ = initialPolynom;
            ancestor_ = initialAncestor ? initialAncestor : this;
        }
        else
        {
            polynomial_ = new Polynom();
            (*polynomial_) += initialPolynom->lm();

            ancestor_ = initialAncestor;
            weakAncestor_ = weakAncestor;
        }

        lm_ = &polynomial_->lm();
        nmp_ = initialNmp;
    }
}

bool Triple::compare(const Triple* tripleA, const Triple* tripleB)
{
    return *tripleA->lm_ > *tripleB->lm_;
}
