#pragma once

#include "timer.h"

#include <cstdint>


class ResourceCounter
{
public:
    Timer groebnerBasisTimer;
    Timer autoReductionTimer;

    uint64_t nonMultiProlongations = 0;
    uint64_t nonMultiProlongationsLength = 0;
    uint64_t nonZeroReductions = 0;
    uint64_t nonZeroReductionsLength = 0;

public:
    ResourceCounter(const ResourceCounter&) = delete;
    ResourceCounter& operator=(const ResourceCounter&) = delete;

    void printFullStatistics(std::ostream& out) const;
    void printBriefStatistics(std::ostream& out) const;

private:
    ResourceCounter() = default;

    friend ResourceCounter& getResourceCounter();
};

ResourceCounter& getResourceCounter();
