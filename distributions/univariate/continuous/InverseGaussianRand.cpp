#include "InverseGaussianRand.h"
#include "NormalRand.h"
#include "UniformRand.h"

InverseGaussianRand::InverseGaussianRand(double mean, double shape)
{
    SetParameters(mean, shape);
}

std::string InverseGaussianRand::Name() const
{
    return "Inverse-Gaussian(" + toStringWithPrecision(GetMean()) + ", " + toStringWithPrecision(GetShape()) + ")";
}

void InverseGaussianRand::SetParameters(double mean, double shape)
{
    mu = (mean > 0.0) ? mean : 1.0;
    lambda = (shape > 0.0) ? shape : 1.0;

    pdfCoef = 0.5 * std::log(0.5 * lambda * M_1_PI);
    cdfCoef = std::exp(2 * lambda / mu);
}

double InverseGaussianRand::f(double x) const
{
    if (x <= 0.0)
        return 0.0;
    return std::exp(logf(x));
}

double InverseGaussianRand::logf(double x) const
{
    if (x <= 0.0)
        return -INFINITY;
    double y = -1.5 * std::log(x);
    double z = (x - mu);
    z *= z;
    z *= -0.5 * lambda / (x * mu * mu);
    z += pdfCoef;
    return y + z;
}

double InverseGaussianRand::F(double x) const
{
    if (x <= 0.0)
        return 0.0;
    double b = std::sqrt(0.5 * lambda / x);
    double a = b * x / mu;
    double y = std::erfc(b - a);
    y += cdfCoef * std::erfc(a + b);
    return 0.5 * y;
}

double InverseGaussianRand::S(double x) const
{
    if (x <= 0.0)
        return 1.0;
    double b = std::sqrt(0.5 * lambda / x);
    double a = b * x / mu;
    double y = std::erfc(a - b);
    y -= cdfCoef * std::erfc(a + b);
    return 0.5 * y;
}

double InverseGaussianRand::Variate() const
{
    double X = NormalRand::StandardVariate();
    double U = UniformRand::StandardVariate();
    X *= X;
    double mupX = mu * X;
    double y = 4 * lambda + mupX;
    y = std::sqrt(y * mupX);
    y -= mupX;
    y *= -0.5 / lambda;
    ++y;
    if (U * (1 + y) > 1.0)
        y = 1.0 / y;
    return mu * y;
}

double InverseGaussianRand::Mean() const
{
    return mu;
}

double InverseGaussianRand::Variance() const
{
    return mu * mu * mu / lambda;
}

std::complex<double> InverseGaussianRand::CFImpl(double t) const
{
    double im = mu * mu;
    im *= t / lambda;
    std::complex<double> y(1, -im - im);
    y = 1.0 - std::sqrt(y);
    y *= lambda / mu;
    return std::exp(y);
}

double InverseGaussianRand::Mode() const
{
    double aux = 1.5 * mu / lambda;
    double mode = 1 + aux * aux;
    mode = std::sqrt(mode);
    mode -= aux;
    return mu * mode;
}

double InverseGaussianRand::Skewness() const
{
    return 3 * std::sqrt(mu / lambda);
}

double InverseGaussianRand::ExcessKurtosis() const
{
    return 15 * mu / lambda;
}
