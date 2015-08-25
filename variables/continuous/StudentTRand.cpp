#include "StudentTRand.h"

StudentTRand::StudentTRand(int degree)
{
    setDegree(degree);
}

std::string StudentTRand::name()
{
    return "Student's t(" + toStringWithPrecision(getDegree()) + ")";
}

void StudentTRand::setDegree(int degree)
{
    v = std::max(degree, 1);
    Y.setDegree(v);

    pdfCoef = std::tgamma(.5 * (v + 1));
    pdfCoef /= (std::sqrt(v * M_PI) * std::tgamma(.5 * v));
}

double StudentTRand::f(double x) const
{
    double y = 1 + x * x / v;
    y = std::pow(y, -.5 * (v + 1));
    return pdfCoef * y;
}

double StudentTRand::F(double x) const
{
    return x;
}

double StudentTRand::variate() const
{
    //v = 1 -> cauchy
    //v = inf -> normal
    return NormalRand::standardVariate() / std::sqrt(Y.variate() / v);
}
