#ifndef STABLERAND_H
#define STABLERAND_H

#include <functional>
#include "UniformRand.h"
#include "ExponentialRand.h"
#include "NormalRand.h"
#include "CauchyRand.h"
#include "LevyRand.h"

/**
 * @brief The StableRand class
 */
class RANDLIBSHARED_EXPORT StableRand : public ContinuousRand
{
    double alpha, beta, mu, sigma;
    UniformRand U;
    ExponentialRand Exp;

    // TODO: shouldn't storage them all
    NormalRand N;
    CauchyRand C;
    LevyRand L;

    double B, S, alphaInv; /// coefficients for alpha != 1
    double logSigma; /// coefficients for alpha == 1
    double alpham1Inv, alpha_alpham1; /// 1 / (alpha - 1) and alpha / (alpha - 1)
    double pdfCoef;
    double zeta, xi, integrandCoef;

public:
    StableRand(double exponent, double skewness, double scale = 1, double location = 0);

    void setParameters(double exponent, double skewness, double scale, double location);

    inline double getAlpha() const { return alpha; }
    inline double getBeta() const { return beta; }
    inline double getSigma() const { return sigma; }
    inline double getMu() const { return mu; }

    virtual double f(double x) const override;
    virtual double F(double x) const override;
    virtual double variate() override;

    double E() const override { return (alpha > 1) ? mu : NAN; }
    double Var() const override { return (alpha == 2) ? 2 * sigma * sigma : INFINITY; }

private:
    double variateForCommonAlpha();
    double variateForAlphaEqualOne();

    /// pdf
    double pdfForCommonAlpha(double x) const;
    double integrandAuxForCommonAlpha(double theta, double xAdj, double xiAdj) const;
    double integrandForCommonAlpha(double theta, double xAdj, double xiAdj) const;

    double pdfForAlphaEqualOne(double x) const;
    double integrandAuxForAlphaEqualOne(double theta, double xAdj) const;
    double integrandForAlphaEqualOne(double theta, double xAdj) const;

    /// cdf
    // isn't written yet!
    double cdfForCommonAlpha(double x) const { return x; }
    double cdfForAlphaEqualOne(double x) const { return x; }
};

#endif // STABLERAND_H