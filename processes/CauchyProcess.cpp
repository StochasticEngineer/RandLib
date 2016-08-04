#include "CauchyProcess.h"
#include "../distributions/univariate/continuous/CauchyRand.h"

CauchyProcess::CauchyProcess(double drift, double volatility, double deltaT) :
    StableProcess(1, 0, drift, volatility, deltaT)
{
}

void CauchyProcess::nextImpl()
{
    currentValue += CauchyRand::variate(mu, sigma) * dt;
}

void CauchyProcess::ProbabilityDensityFunction(double t, const std::vector<double> &x, std::vector<double> &y) const
{
    double tau = t - currentTime;
    if (tau <= 0)
        return;
    CauchyRand X(currentValue + mu * tau, sigma * tau);
    return X.ProbabilityDensityFunction(x, y);
}

void CauchyProcess::CumulativeDistributionFunction(double t, const std::vector<double> &x, std::vector<double> &y) const
{
    double tau = t - currentTime;
    if (tau <= 0)
        return;
    CauchyRand X(currentValue + mu * tau, sigma * tau);
    return X.CumulativeDistributionFunction(x, y);
}

double CauchyProcess::Quantile(double t, double p) const
{
    double tau = t - currentTime;
    return CauchyRand::quantile(p, currentValue + mu * tau, sigma * tau);
}