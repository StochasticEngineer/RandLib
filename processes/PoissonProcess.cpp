#include "PoissonProcess.h"

PoissonProcess::PoissonProcess(double rate, double deltaT) :
    StochasticProcess(deltaT),
    lambda(rate > 0.0 ? rate : 1.0),
    futureJumpTime(ExponentialRand::StandardVariate() / lambda)
{

}

void PoissonProcess::NextImpl()
{
    while (currentTime > futureJumpTime) {
        ++currentValue;
        futureJumpTime += ExponentialRand::StandardVariate() / lambda;
    }
}

double PoissonProcess::MeanImpl(double t) const
{
    return currentValue + lambda * (t - currentTime);
}

double PoissonProcess::VarianceImpl(double t) const
{
    return lambda * (t - currentTime);
}

double PoissonProcess::Quantile(double t, double p) const
{
    PoissonRand X(lambda * (t - currentTime));
    return X.Quantile(p) + currentValue;
}

