#ifndef HYPERGEOMETRICRAND_H
#define HYPERGEOMETRICRAND_H

#include "DiscreteDistribution.h"
#include "BernoulliRand.h"

/**
 * @brief The HyperGeometricRand class <BR>
 * Hypergeometric distribution
 *
 * X ~ HG(N, K, n)
 */
class RANDLIBSHARED_EXPORT HyperGeometricRand : public DiscreteDistribution
{
    size_t N = 1; ///< population size
    size_t K = 1; /// number of possible successes
    size_t n = 1; /// number of draws
    double pmfCoef = 1; ///< C(N, n)
    double p0 = 1; ///< K/N

public:
    HyperGeometricRand(size_t totalSize, size_t drawsNum, size_t successesNum);
    std::string Name() const override;
    SUPPORT_TYPE SupportType() const override { return FINITE_T; }
    int MinValue() const override { return std::max(0, n - N + K); }
    int MaxValue() const override { return std::min(n, K); }

    void SetParameters(size_t totalSize, size_t drawsNum, size_t successesNum);
    inline int GetTotalSize() { return N; }
    inline int GetDrawsNum() { return n; }
    inline int GetSuccessesNum() { return K; }

    double P(const int & k) const override;
    double logP(const int & k) const override;
    double F(const int & k) const override;
    int Variate() const override;

    double Mean() const override;
    double Variance() const override;
    int Mode() const override;
    double Skewness() const override;
    double ExcessKurtosis() const override;
};

#endif // HYPERGEOMETRICRAND_H
