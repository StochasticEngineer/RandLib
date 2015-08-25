#ifndef RAYLEIGHRAND_H
#define RAYLEIGHRAND_H

#include "ContinuousRand.h"
#include "ExponentialRand.h"

/**
 * @brief The RayleighRand class
 */
class RANDLIBSHARED_EXPORT RayleighRand : public ContinuousRand
{
    double sigma;
    double sigmaSqInv_2; // 2 * sigma^2

public:
    RayleighRand(double scale = 1);
    virtual std::string name() override;

    void setScale(double scale);
    inline double getScale() const { return sigma; }

    double f(double x) const override;
    double F(double x) const override;
    double variate() const override;

    double E() const override { return sigma * M_SQRTPI * M_SQRT1_2; }
    double Var() const override { return (1 - M_PI_4) * sigmaSqInv_2; }

    inline double Median() const { return sigma * std::sqrt(2 * M_LN2); }
    inline double Mode() const { return sigma; }
    static constexpr double Skewness() { return 2 * M_SQRTPI * (M_PI - 3) / std::pow(4.0 - M_PI, 1.5); }
    static constexpr double ExcessKurtosis() { return (6 * M_PI / (M_PI - 4) - 16 / (M_PI - 4) / (M_PI - 4)); }

    bool fitToData(const QVector<double> &sample);
};
#endif // RAYLEIGHRAND_H
