#ifndef GEOMETRICRAND_H
#define GEOMETRICRAND_H

#include "DiscreteRand.h"
#include "../continuous/UniformRand.h"
#include "../continuous/ExponentialRand.h"
#include <functional>

/**
 * @brief The GeometricRand class
 */
class RANDLIBSHARED_EXPORT GeometricRand : public DiscreteRand<int>
{
    double p, q;

    static constexpr int tableSize = 16;
    // TODO: don't storage both variables (including tableSize)
    double table[tableSize];
    ExponentialRand W;

public:
    GeometricRand(double probability);
    virtual std::string name() override;

    void setProbability(double probability);
    inline double getProbability() const { return p; }

    double P(int k) const override;
    double F(double x) const override;
    double variate() const override;

    void sample(QVector<double> &outputData);

private:
    double variateForSmallP() const;
    double variateForLargeP() const;

public:
    double E() const override { return 1.0 / p - 1; }
    double Var() const override { return (1 - p) / (p * p); }

    inline double Median() const { return std::ceil(-M_LN2 / std::log(1 - p)) - 1; }
    static double constexpr Mode() { return 0; }
    inline double Skewness() { return (2 - p) / std::sqrt(1 - p); }
    inline double ExcessiveKurtosis() { return p * p / (1 - p) + 6; }

    inline double Entropy() const {
        double q = 1 - p;
        double a = -q * std::log(q);
        double b = -p * std::log(p);
        return (a + b) / (M_LN2 * p);
    }
};

#endif // GEOMETRICRAND_H
