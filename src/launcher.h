#pragma once

#include "groebner_basis.h"
#include "polynom.h"
#include "settings_manager.h"

#include <list>
#include <string>


class Launcher
{
public:
    Launcher();
    bool init(int argc, char* argv[]);
    bool run();

private:
    static void fillOptions();

    void printUsage() const;
    void printVersion() const;
    bool analizeArguments(int argc, char* argv[]);

    bool getTaskFromFile(std::list<Polynom*>& initialSet, std::list<Polynom*>& initialAnswer) const;

    void readVariables(std::ifstream& inputFileStream) const;
    void readPolynomList(std::ifstream& inputFileStream, std::list<Polynom*>& list) const;
    void clearPolynomList(std::list<Polynom*>& list) const;

    void printResult(GroebnerBasis& gBasis, std::list<Polynom*>& initialAnswer) const;
    bool checkAnswer(GroebnerBasis& gBasis, std::list<Polynom*>& initialAnswer) const;

private:
    std::string applicationName_;
    std::string inputFileName_;
};
