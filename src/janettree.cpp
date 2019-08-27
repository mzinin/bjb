#include "janettree.h"


const Triple* JanetTree::find(const Monom& monom) const
{
    const Triple* triple = nullptr;

    if (root_)
    {
        ConstIterator nodeIterator = root_;
        Monom::Integer degree = monom.degree();
        Monom::Integer var = 0;
        do
        {
            while (nodeIterator.degree() < monom[var] && nodeIterator.hasNextDegree())
            {
                nodeIterator.stepNextDegree();
            }

            if (nodeIterator.degree() > monom[var])
            {
                break;
            }
            else if (nodeIterator.hasNextVariable())
            {
                degree = static_cast<Monom::Integer>(degree - monom[var]);
                if (!degree)
                {
                    break;
                }
                ++var;
                nodeIterator.stepNextVariable();
            }
            else
            {
                triple = nodeIterator.triple();
                break;
            }
        } while (true);
    }
    return triple;
}

void JanetTree::insert(Triple* triple)
{
    if (!triple)
    {
        return;
    }

    Monom::Integer degree = triple->polynomLm().degree();
    JanetTree::Iterator nodeIterator(root_);

    if (!root_)
    {
        nodeIterator.build(degree, 0, triple);
    }
    else
    {
        Monom::Integer var = 0;
        do
        {
            while (nodeIterator.degree() < triple->polynomLm()[var] && nodeIterator.hasNextDegree())
            {
                nodeIterator.stepNextDegree();
            }
            if (nodeIterator.degree() > triple->polynomLm()[var])
            {
                nodeIterator.build(degree, var, triple);
                break;
            }
            else if (nodeIterator.degree() == triple->polynomLm()[var])
            {
                degree = static_cast<Monom::Integer>(degree - triple->polynomLm()[var]);
                ++var;
                nodeIterator.stepNextVariable();
            }
            else
            {
                nodeIterator.stepNextDegree();
                nodeIterator.build(degree, var, triple);
                break;
            }
        } while (true);
    }
}

void JanetTree::erase(const Triple* triple)
{
    if (!triple)
    {
        return;
    }

    Iterator nodeIterator = root_;
    // count bifurcations
    Monom::Integer var = 0;
    unsigned bifurcations = 0;

    if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
    {
        ++bifurcations;
    }

    do
    {
        while (nodeIterator.degree() < triple->polynomLm()[var])
        {
            nodeIterator.stepNextDegree();
            if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
            {
                ++bifurcations;
            }
        }

        if (nodeIterator.hasNextVariable())
        {
            ++var;
            nodeIterator.stepNextVariable();
            if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
            {
                ++bifurcations;
            }
        }
        else
        {
            break;
        }
    } while (true);

    // removing node
    nodeIterator = root_;
    var = 0;
    bool varDirection = false;

    if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
    {
        --bifurcations;
    }
    if (!bifurcations)
    {
        if (nodeIterator.degree() < triple->polynomLm()[var])
        {
            nodeIterator.stepNextDegree();
        }
        else
        {
            varDirection = true;
        }
    }

    while (bifurcations > 0)
    {
        while (nodeIterator.degree() < triple->polynomLm()[var] && bifurcations > 0)
        {
            nodeIterator.stepNextDegree();
            if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
            {
                --bifurcations;
            }
        }

        if (!bifurcations)
        {
            if (nodeIterator.degree() < triple->polynomLm()[var])
            {
                nodeIterator.stepNextDegree();
                break;
            }
            else
            {
                varDirection = true;
                break;
            }
        }

        ++var;
        nodeIterator.stepNextVariable();
        if (nodeIterator.hasNextDegree() && nodeIterator.hasNextVariable())
        {
            --bifurcations;
        }
        if (!bifurcations)
        {
            if (nodeIterator.degree() < triple->polynomLm()[var])
            {
                nodeIterator.stepNextDegree();
                break;
            }
            else
            {
                varDirection = true;
                break;
            }
        }
    }

    if (varDirection)
    {
        Iterator tmpIterator = nodeIterator;
        tmpIterator.stepNextVariable();
        tmpIterator.clear();
        nodeIterator.erase();
    }
    else
    {
        nodeIterator.clear();
    }
}

void JanetTree::clear()
{
    if (root_)
    {
        JanetTree::Iterator nodeIterator(root_);
        nodeIterator.clear();
    }
}

std::set<Monom::Integer> JanetTree::nonMulti(const Triple* triple) const
{
    std::set<Monom::Integer> result;

    if (triple && root_)
    {
        ConstIterator nodeIterator(root_);
        Monom::Integer var = 0;
        do
        {
            while (nodeIterator.degree() < triple->polynomLm()[var])
            {
                nodeIterator.stepNextDegree();
            }
            if (nodeIterator.hasNextDegree())
            {
                result.insert(var);
            }

            ++var;
            if (nodeIterator.hasNextVariable())
            {
                nodeIterator.stepNextVariable();
            }
            else
            {
                break;
            }
        } while (true);
    }

    return result;
}
