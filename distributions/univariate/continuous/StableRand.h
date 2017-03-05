#ifndef STABLERAND_H
#define STABLERAND_H

#include <functional>
#include "LimitingDistribution.h"

/**
 * @brief The StableRand class
 * Stable distribution
 *
 * X ~ Stable(α, β, σ, μ)
 *
 * Related distributions:
 * If X ~ Normal(μ, σ), then X ~ Stable(2, 0, σ, μ)
 * If X ~ Cauchy(μ, σ), then X ~ Stable(1, 0, σ / 2^(1/2), μ)
 * If +/-X ~ Levy(μ, σ), then X ~ Stable(0.5, +/-1, σ, μ)
 */
class RANDLIBSHARED_EXPORT StableRand : public LimitingDistribution
{
    double xi, omega, zeta; /// coefficients for common α
    double alpham1Inv, alpha_alpham1; /// 1 / (α - 1) and α / (α - 1)

    static constexpr double BIG_NUMBER = 1e9; /// aka infinity for pdf and cdf calculations
    static constexpr double ALMOST_TWO = 1.99999; /// parameter used to identify α close to 2

    enum DISTRIBUTION_ID {
        NORMAL, /// α = 2
        LEVY, /// α = 0.5, |β| = 1
        CAUCHY, /// α = 1, β = 0
        UNITY_EXPONENT, /// α = 1, β != 0
        COMMON /// the rest
    };

    DISTRIBUTION_ID distributionId;

protected:
    double pdfCoef;
    double pdfXLimit; /// boundary k such that for |x| > k we can use tail approximation

public:
    StableRand(double exponent, double skewness, double scale = 1, double location = 0);
    virtual ~StableRand() {}

    std::string Name() const override;
    SUPPORT_TYPE SupportType() const override {
        if (alpha < 1) {
            if (beta == 1)
                return RIGHTSEMIFINITE_T;
            if (beta == -1)
                return LEFTSEMIFINITE_T;
        }
        return INFINITE_T;
    }
    double MinValue() const override { return (alpha < 1 && beta == 1) ? mu : -INFINITY; }
    double MaxValue() const override { return (alpha < 1 && beta == -1) ? mu : INFINITY; }

    void SetParameters(double exponent, double skewness);
    void SetScale(double scale);

    /// Probability distribution functions
protected:
    /**
     * @brief pdfNormal
     * @param x
     * @return probability density function of normal distribution
     */
    double pdfNormal(double x) const;
    /**
     * @brief logpdfNormal
     * @param x
     * @return logarithm of probability density function of normal distribution
     */
    double logpdfNormal(double x) const;
    /**
     * @brief pdfCauchy
     * @param x
     * @return probability density function of Cauchy distribution
     */
    double pdfCauchy(double x) const;
    /**
     * @brief logpdfCauchy
     * @param x
     * @return logarithm of probability density function of Cauchy distribution
     */
    double logpdfCauchy(double x) const;
    /**
     * @brief pdfLevy
     * @param x
     * @return probability density function of Levy distribution
     */
    double pdfLevy(double x) const;
    /**
     * @brief logpdfLevy
     * @param x
     * @return logarithm of probability density function of Levy distribution
     */
    double logpdfLevy(double x) const;
private:
    /**
     * @brief fastpdfExponentiation
     * @param u
     * @return exp(u - exp(u)), accelerated by truncation of input u
     */
    static double fastpdfExponentiation(double u);

    /// functions for pdf calculation for α = 1
    double limitCaseForIntegrandAuxForUnityExponent(double theta, double xAdj) const;
    double integrandAuxForUnityExponent(double theta, double xAdj) const;
    double integrandForUnityExponent(double theta, double xAdj) const;
    double pdfForUnityExponent(double x) const;

    /// functions for pdf calculation for α != 1
    DoublePair seriesZeroParams;
    /**
     * @brief pdfAtZero
     * @return probability density function for x == 0
     */
    double pdfAtZero() const;
    /**
     * @brief pdfSeriesExpansionAtZero
     * @param logX log(x)
     * @param xiAdj adjusted ξ
     * @param k number of elements in series
     * @return series expansion of probability density function for x near 0
     */
    double pdfSeriesExpansionAtZero(double logX, double xiAdj, int k) const;
    /**
     * @brief pdfSeriesExpansionAtInf
     * @param logX log(x)
     * @param xiAdj adjusted ξ
     * @param k number of elements in series
     * @return series expansion of probability density function for large x
     */
    double pdfSeriesExpansionAtInf(double logX, double xiAdj, int k) const;
    double pdfTaylorExpansionTailNearCauchy(double x) const; /// ~ f(x, α) - f(x, 1)
    double limitCaseForIntegrandAuxForCommonExponent(double theta, double xiAdj) const;
    double integrandAuxForCommonExponent(double theta, double xAdj, double xiAdj) const;
    double integrandForCommonExponent(double theta, double xAdj, double xiAdj) const;
    double pdfForCommonExponent(double x) const;
public:    
    double f(const double & x) const override;
    double logf(const double & x) const override;

    /// Cumulative distribution functions
protected:
    /**
     * @brief cdfNormal
     * @param x
     * @return cumulative distribution function of normal distribution
     */
    double cdfNormal(double x) const;
    /**
     * @brief cdfNormalCompl
     * @param x
     * @return complementary cumulative distribution function of normal distribution
     */
    double cdfNormalCompl(double x) const;
    /**
     * @brief cdfCauchy
     * @param x
     * @return cumulative distribution function of Cauchy distribution
     */
    double cdfCauchy(double x) const;
    /**
     * @brief cdfCauchyCompl
     * @param x
     * @return complementary cumulative distribution function of Cauchy distribution
     */
    double cdfCauchyCompl(double x) const;
    /**
     * @brief cdfLevy
     * @param x
     * @return cumulative distribution function of Levy distribution
     */
    double cdfLevy(double x) const;
    /**
     * @brief cdfLevyCompl
     * @param x
     * @return complementary cumulative distribution function of Levy distribution
     */
    double cdfLevyCompl(double x) const;
private:
    /**
     * @brief fastcdfExponentiation
     * @param u
     * @return exp(-exp(u)), accelerated by truncation of input u
     */
    static double fastcdfExponentiation(double u);

    /**
     * @brief cdfForUnityExponent
     * @param x
     * @return cumulative distribution function for α = 1, β != 0
     */
    double cdfForUnityExponent(double x) const;
    /**
     * @brief cdfIntegralRepresentation
     * @param absXSt absolute value of standardised x
     * @param xiAdj adjusted ξ
     * @return integral representation of cumulative distribution function for common case of α != 1
     */
    double cdfIntegralRepresentation(double absXSt, double xiAdj) const;
    /**
     * @brief cdfForCommonExponent
     * @param x
     * @return cumulative distribution function for common case of α != 1
     */
    double cdfForCommonExponent(double x) const;
public:
    double F(const double & x) const override;
    double S(const double & x) const override;

    /// Variates
private:
    /**
     * @brief variateForUnityExponent
     * @return variate, generated by algorithm for α = 1, β != 0
     */
    double variateForUnityExponent() const;
    /**
     * @brief variateForCommonExponent
     * @return variate, generated by algorithm for common case of α != 1
     */
    double variateForCommonExponent() const;
public:
    double Variate() const override;
    void Sample(std::vector<double> &outputData) const override;

public:
    double Variance() const override;
    double Median() const override;
    double Mode() const override;
    double Skewness() const override;
    double ExcessKurtosis() const override;

private:
    std::complex<double> CFImpl(double t) const override;
};


/**
 * @brief The HoltsmarkRand class
 *
 * X ~ Holtsmark(σ, μ)
 *
 * Related distributions:
 * X ~ Stable(1.5, 0, σ, μ)
 */
class RANDLIBSHARED_EXPORT HoltsmarkRand : public StableRand
{
public:
    HoltsmarkRand(double scale = 1, double location = 0) : StableRand(1.5, 0.0, scale, location) {}
    std::string Name() const override;
private:
    using StableRand::SetParameters;
};

/**
 * @brief The LandauRand class
 *
 * X ~ Landau(σ, μ)
 *
 * Related distributions:
 * X ~ Stable(1, 1, σ, μ)
 */
class RANDLIBSHARED_EXPORT LandauRand : public StableRand
{
public:
    LandauRand(double scale = 1, double location = 0) : StableRand(1.0, 1.0, scale, location) {}
    std::string Name() const override;
private:
    using StableRand::SetParameters;
};

#endif // STABLERAND_H
