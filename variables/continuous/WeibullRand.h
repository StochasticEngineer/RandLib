#ifndef WEIBULLRAND_H
#define WEIBULLRAND_H

#include "ContinuousRand.h"
#include "ExponentialRand.h"

/**
 * @brief The WeibullRand class
 */
class RANDLIBSHARED_EXPORT WeibullRand : public ContinuousRand
{
    double l, k;
    double lInv; /// 1.0 / l

public:
    WeibullRand(double scale = 1, double shape = 1);
    virtual std::string name() override;

    void setParameters(double scale, double shape);
    inline double getScale() const { return l; }
    inline double getShape() const { return k; }

    double f(double x) const override;
    double F(double x) const override;
    double variate() const override;

    double E() const override { return l * std::tgamma(1 + 1.0 / k); }
    double Var() const override {
        double res = std::tgamma(1 + 1.0 / k);
        res *= res;
        res += std::tgamma(1 + 2.0 / k);
        return l * l * res;
    }
};

#endif // WEIBULLRAND_H
