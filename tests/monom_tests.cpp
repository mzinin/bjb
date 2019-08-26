#include "monom.h"

#include <cstdlib>
#include <iostream>


void setVariables()
{
    Monom::addVariable("x0");
    Monom::addVariable("x1");
    Monom::addVariable("x2");
    Monom::addVariable("x3");
    Monom::addVariable("x4");
    Monom::addVariable("x5");
}

bool runTestGroup1()
{
    std::cout << "***** Running test group 1 *****" << std::endl;
    bool result = true, currentResult = false;


    Monom m1("x1");
    currentResult = m1.toString() == "x1";
    if (currentResult)
    {
        std::cout << "\tTest 1-1 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 1-1 failed." << std::endl;
        std::cerr << "\t\tm1 = " << m1 << std::endl;
    }
    result = result && currentResult;


    Monom m2("x2^2");
    currentResult = m2.toString() == "x2^2";
    if (currentResult)
    {
        std::cout << "\tTest 1-2 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 1-2 failed." << std::endl;
        std::cerr << "\t\tm2 = " << m2 << std::endl;
    }
    result = result && currentResult;


    Monom m3("x1*x2");
    currentResult = m3.toString() == "x1*x2";
    if (currentResult)
    {
        std::cout << "\tTest 1-3 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 1-3 failed." << std::endl;
        std::cout << "\t\tm3 = " << m3 << std::endl;
    }
    result = result && currentResult;


    Monom m4;
    currentResult = m4.toString() == "1";
    if (currentResult)
    {
        std::cout << "\tTest 1-4 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 1-4 failed." << std::endl;
        std::cout << "\t\tm4 = " << m4 << std::endl;
    }
    result = result && currentResult;


    Monom m5(m3);
    currentResult = m5.toString() == "x1*x2";
    if (currentResult)
    {
        std::cout << "\tTest 1-5 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 1-5 failed." << std::endl;
        std::cout << "\t\tm5 = " << m5 << std::endl;
    }
    result = result && currentResult;


    if (result)
    {
        std::cout << "***** Test group 1 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "*** Test group 1 FAILED ***" << std::endl;
    }
    return result;
}

bool runTestGroup2()
{
    std::cout << "***** Running test group 2 *****" << std::endl;
    bool result = true, currentResult = false;


    Monom* m1 = new Monom("x1*x2*x3*x4^2");
    currentResult = m1 && m1->toString() == "x1*x2*x3*x4^2";
    if (currentResult)
    {
        std::cout << "\tTest 2-1 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 2-1 failed." << std::endl;
        std::cerr << "\t\tm1 = " << m1 << "; *m1 = " << *m1 << std::endl;
    }
    result = result && currentResult;


    delete m1;
    m1 = 0;
    currentResult = !m1;
    if (currentResult)
    {
        std::cout << "\tTest 2-2 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 2-2 failed." << std::endl;
        std::cerr << "\t\tm1 = " << m1 << std::endl;
    }
    result = result && currentResult;


    Monom m3("x1*x2");
    m3.setOne();
    currentResult = m3.toString() == "1";
    if (currentResult)
    {
        std::cout << "\tTest 2-3 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 2-3 failed." << std::endl;
        std::cout << "\t\tm3 = " << m3 << std::endl;
    }
    result = result && currentResult;


    Monom m4("x2*x3^2*x5"), m5("x2*x3*x5");
    currentResult = m4.degree() == 4 && m5.degree() == 3;
    if (currentResult)
    {
        std::cout << "\tTest 2-4 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 2-4 failed." << std::endl;
        std::cout << "\t\tm4.degree() = " << m4.degree() << "; m5.degree() = " << m5.degree() << std::endl;
    }
    result = result && currentResult;


    Monom m6("x2*x3^2*x5");
    currentResult = m6[0] == 0 && m6[1] == 0 && m6[2] == 1 && m6[3] == 2 && m6[4] == 0 && m6[5] == 1;
    if (currentResult)
    {
        std::cout << "\tTest 2-5 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 2-5 failed." << std::endl;
        std::cout << "\t\tm6[var] = " << m6[0] << "," << m6[1] << "," << m6[2] << "," << m6[3] << "," << m6[4] << "," << m6[5] << ",";
    }
    result = result && currentResult;


    if (result)
    {
        std::cout << "***** Test group 2 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 2 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup3()
{
    std::cout << "***** Running test group 3 *****" << std::endl;
    bool result = true, currentResult = false;


    Monom m1("x1*x2*x3*x4^2"), m2("x1*x2");
    m1 = m2;
    currentResult = m1.toString() == "x1*x2";
    if (currentResult)
    {
        std::cout << "\tTest 3-1 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 3-1 failed." << std::endl;
        std::cerr << "\t\tm1 = " << m1 << std::endl;
    }
    result = result && currentResult;


    Monom m3("x1*x2"), m4("x1*x2");
    currentResult = m3 == m4;
    if (currentResult)
    {
        std::cout << "\tTest 3-2 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 3-2 failed." << std::endl;
    }
    result = result && currentResult;


    Monom m5("x1*x2"), m6("x1*x3");
    currentResult = !(m5 == m6);
    if (currentResult)
    {
        std::cout << "\tTest 3-3 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 3-3 failed." << std::endl;
    }
    result = result && currentResult;


    Monom m7("x1*x2"), m8("x1*x2");
    currentResult = !(m7 != m8);
    if (currentResult)
    {
        std::cout << "\tTest 3-4 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 3-4 failed." << std::endl;
    }
    result = result && currentResult;


    Monom m9("x1*x2"), m10("x1*x3");
    currentResult = m9 != m10;
    if (currentResult)
    {
        std::cout << "\tTest 3-5 succeded." << std::endl;
    }
    else
    {
        std::cerr << "\t\tTest 3-5 failed." << std::endl;
    }
    result = result && currentResult;


    if (result)
    {
        std::cout << "***** Test group 3 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 3 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup4()
{
    std::cout << "***** Running test group 4 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1*x4");
        m1.prolong(1);
        currentResult = m1.toString() == "x1^2*x4" && m1.degree() == 3;
        if (currentResult)
        {
            std::cout << "\tTest 4-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 4-1 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x4");
        m1.prolong(1, 2);
        currentResult = m1.toString() == "x1^3*x4" && m1.degree() == 4;
        if (currentResult)
        {
            std::cout << "\tTest 4-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 4-2 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x4");
        m1.prolong(2);
        currentResult = m1.toString() == "x1*x2*x4" && m1.degree() == 3;
        if (currentResult)
        {
            std::cout << "\tTest 4-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 4-3 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x4");
        m1.prolong(2, 2);
        currentResult = m1.toString() == "x1*x2^2*x4" && m1.degree() == 4;
        if (currentResult)
        {
            std::cout << "\tTest 4-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 4-4 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 4 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 4 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup5()
{
    std::cout << "***** Running test group 5 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1*x4"), m2("x2*x3");
        m1.multiply(m2);
        currentResult = m1.toString() == "x1*x2*x3*x4" && m1.degree() == 4;
        if (currentResult)
        {
            std::cout << "\tTest 5-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 5-1 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x4"), m2("x1*x3");
        m1.multiply(m2);
        currentResult = m1.toString() == "x1^2*x3*x4" && m1.degree() == 4;
        if (currentResult)
        {
            std::cout << "\tTest 5-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 5-2 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x4"), m2("x2*x4^2");
        m1.multiply(m2);
        currentResult = m1.toString() == "x1*x2*x4^3" && m1.degree() == 5;
        if (currentResult)
        {
            std::cout << "\tTest 5-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 5-3 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 5 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 5 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup6()
{
    std::cout << "***** Running test group 6 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1^2*x3*x4"), m2("x1");
        m1.divide(m2);
        currentResult = m1.toString() == "x1*x3*x4" && m1.degree() == 3;
        if (currentResult)
        {
            std::cout << "\tTest 6-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-1 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3*x4"), m2("x1^2");
        m1.divide(m2);
        currentResult = m1.toString() == "x3*x4" && m1.degree() == 2;
        if (currentResult)
        {
            std::cout << "\tTest 6-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-2 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3*x4"), m2("x1^2*x4");
        m1.divide(m2);
        currentResult = m1.toString() == "x3" && m1.degree() == 1;
        if (currentResult)
        {
            std::cout << "\tTest 6-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-3 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x3*x4"), m2("x3*x4");
        m1.divide(m2);
        currentResult = m1.toString() == "x1" && m1.degree() == 1;
        if (currentResult)
        {
            std::cout << "\tTest 6-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-4 failed." << std::endl;
            std::cerr << "\t\tm1 = " << m1 << "; m1.degree() = " << m1.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3*x4"), m2("x1^2*x4"), m3;
        m3.setQuotientOf(m1, m2);
        currentResult = m3.toString() == "x3" && m3.degree() == 1;
        if (currentResult)
        {
            std::cout << "\tTest 6-5 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-5 failed." << std::endl;
            std::cerr << "\t\tm3 = " << m3 << "; m3.degree() = " << m3.degree() << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3*x4"), m2("x1*x3"), m3;
        m3.setQuotientOf(m1, m2);
        currentResult = m3.toString() == "x1*x4" && m3.degree() == 2;
        if (currentResult)
        {
            std::cout << "\tTest 6-6 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 6-6 failed." << std::endl;
            std::cerr << "\t\tm3 = " << m3 << "; m3.degree() = " << m3.degree() << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 6 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 6 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup7()
{
    std::cout << "***** Running test group 7 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1^2"), m2("x2^2*x3");
        currentResult = m1 < m2;
        if (currentResult)
        {
            std::cout << "\tTest 7-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-1 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x2^2*x3"), m2("x1^2*x3");
        currentResult = m1 < m2;
        if (currentResult)
        {
            std::cout << "\tTest 7-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-2 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3^2"), m2("x1*x3");
        currentResult = m1 > m2;
        if (currentResult)
        {
            std::cout << "\tTest 7-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-3 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x3"), m2("x1*x4");
        currentResult = m1 > m2;
        if (currentResult)
        {
            std::cout << "\tTest 7-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-4 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3"), m2("x2*x3*x4");
        currentResult = Monom::compare(m1, m2) == 1;
        if (currentResult)
        {
            std::cout << "\tTest 7-5 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-5 failed." << std::endl;
            std::cerr << "\t\tMonom::compare(x1^2*x3, x2*x3*x4) = " << Monom::compare(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x3*x5"), m2("x1*x3*x4");
        currentResult = Monom::compare(m1, m2) == -1;
        if (currentResult)
        {
            std::cout << "\tTest 7-6 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-6 failed." << std::endl;
            std::cerr << "\t\tMonom::compare(x1*x3, x2*x3*x4) = " << Monom::compare(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x3"), m2("x1*x3");
        currentResult = Monom::compare(m1, m2) == 0;
        if (currentResult)
        {
            std::cout << "\tTest 7-7 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 7-7 failed." << std::endl;
            std::cerr << "\t\tMonom::compare(x1*x3, x1*x3) = " << Monom::compare(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 7 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 7 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup8()
{
    std::cout << "***** Running test group 8 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1^2*x2*x5"), m2("x2*x5");
        currentResult = m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-1 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x2*x5"), m2("x1^2*x5");
        currentResult = m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-2 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x5"), m2("x1*x4");
        currentResult = !m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-3 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x5"), m2("x1^2*x2");
        currentResult = !m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-4 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x3"), m2("x1*x2*x3*x4");
        currentResult = !m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-5 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-5 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x3"), m2("x1*x2*x3");
        currentResult = m1.isDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-6 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-6 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x3"), m2("x1*x2*x3");
        currentResult = !m1.isTrueDivisibleBy(m2);
        if (currentResult)
        {
            std::cout << "\tTest 8-7 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 8-7 failed." << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 8 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 8 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup9()
{
    std::cout << "***** Running test group 9 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1"), m2("x2");
        currentResult = !m1.hasSameOnlyVariable(m2);
        if (currentResult)
        {
            std::cout << "\tTest 9-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 9-1 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2"), m2("x1");
        currentResult = m1.hasSameOnlyVariable(m2);
        if (currentResult)
        {
            std::cout << "\tTest 9-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 9-2 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x4^2"), m2("x5");
        currentResult = !m1.hasSameOnlyVariable(m2);
        if (currentResult)
        {
            std::cout << "\tTest 9-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 9-3 failed." << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x2^2"), m2("x2^2");
        currentResult = m1.hasSameOnlyVariable(m2);
        if (currentResult)
        {
            std::cout << "\tTest 9-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 9-4 failed." << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 9 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 9 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

bool runTestGroup10()
{
    std::cout << "***** Running test group 10 *****" << std::endl;
    bool result = true, currentResult = false;

    {
        Monom m1("x1^2*x2*x5"), m2("x1*x4*x5");
        currentResult = Monom::gcdDegree(m1, m2) == 2;
        if (currentResult)
        {
            std::cout << "\tTest 10-1 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-1 failed." << std::endl;
            std::cerr << "\t\tMonom::gcdDegree(" << m1 << ", " << m2 << ") = " << Monom::gcdDegree(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x2*x5"), m2("x3*x4");
        currentResult = Monom::gcdDegree(m1, m2) == 0;
        if (currentResult)
        {
            std::cout << "\tTest 10-2 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-2 failed." << std::endl;
            std::cerr << "\t\tMonom::gcdDegree(" << m1 << ", " << m2 << ") = " << Monom::gcdDegree(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x2*x5"), m2("x1*x4*x5");
        currentResult = Monom::lcmDegree(m1, m2) == 5;
        if (currentResult)
        {
            std::cout << "\tTest 10-3 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-3 failed." << std::endl;
            std::cerr << "\t\tMonom::lcmDegree(" << m1 << ", " << m2 << ") = " << Monom::lcmDegree(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x5"), m2("x2*x3*x5");
        currentResult = Monom::lcmDegree(m1, m2) == 4;
        if (currentResult)
        {
            std::cout << "\tTest 10-4 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-4 failed." << std::endl;
            std::cerr << "\t\tMonom::lcmDegree(" << m1 << ", " << m2 << ") = " << Monom::lcmDegree(m1, m2) << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x5"), m2("x2*x3*x5"), m3;
        m3.setGcdOf(m1, m2);
        currentResult = m3.toString() == "x5";
        if (currentResult)
        {
            std::cout << "\tTest 10-5 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-5 failed." << std::endl;
            std::cerr << "\t\tGcd(" << m1 << ", " << m2 << ") = " << m3 << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3^2"), m2("x2^2*x3^2*x5"), m3;
        m3.setGcdOf(m1, m2);
        currentResult = m3.toString() == "x3^2";
        if (currentResult)
        {
            std::cout << "\tTest 10-6 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-6 failed." << std::endl;
            std::cerr << "\t\tGcd(" << m1 << ", " << m2 << ") = " << m3 << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1^2*x3"), m2("x2*x3*x5"), m3;
        m3.setLcmOf(m1, m2);
        currentResult = m3.toString() == "x1^2*x2*x3*x5";
        if (currentResult)
        {
            std::cout << "\tTest 10-7 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-7 failed." << std::endl;
            std::cerr << "\t\tLcm(" << m1 << ", " << m2 << ") = " << m3 << std::endl;
        }
        result = result && currentResult;
    }

    {
        Monom m1("x1*x3"), m2("x2*x3"), m3;
        m3.setLcmOf(m1, m2);
        currentResult = m3.toString() == "x1*x2*x3";
        if (currentResult)
        {
            std::cout << "\tTest 10-8 succeded." << std::endl;
        }
        else
        {
            std::cerr << "\t\tTest 10-8 failed." << std::endl;
            std::cerr << "\t\tLcm(" << m1 << ", " << m2 << ") = " << m3 << std::endl;
        }
        result = result && currentResult;
    }

    if (result)
    {
        std::cout << "***** Test group 10 succeded *****" << std::endl << std::endl;
    }
    else
    {
        std::cout << "***** Test group 10 FAILED *****" << std::endl << std::endl;
    }
    return result;
}

int main()
{
    setVariables();

    bool result = runTestGroup1();
    result = result && runTestGroup2();
    result = result && runTestGroup3();
    result = result && runTestGroup4();
    result = result && runTestGroup5();
    result = result && runTestGroup6();
    result = result && runTestGroup7();
    result = result && runTestGroup8();
    result = result && runTestGroup9();
    result = result && runTestGroup10();

    if (result)
    {
        std::cout << "***** All tests succeeded *****" << std::endl;
    }
    else
    {
        std::cout << "***** SOME TESTS FAILED!!! *****" << std::endl;
    }

    return EXIT_SUCCESS;
}
