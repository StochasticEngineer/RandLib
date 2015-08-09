#include "StableRand.h"
#include <QDebug>

StableRand::StableRand(double exponent, double skewness, double scale, double location) :    
    U(-M_PI_2, M_PI_2), N(0, M_SQRT2)
{
    setParameters(exponent, skewness, scale, location);
}

void StableRand::setParameters(double exponent, double skewness, double scale, double location)
{
    alpha = std::min(exponent, 2.0);
    alpha = std::max(alpha, MIN_POSITIVE);
    alphaInv = 1.0 / alpha;
    alpha_alpham1 = alpha / (alpha - 1.0);
    alpham1Inv = alpha_alpham1 - 1.0;

    beta = std::min(skewness, 1.0);
    beta = std::max(beta, -1.0);

    sigma = std::max(scale, MIN_POSITIVE);
    mu = location;

    /// Should be cautious, known distributions in priority
    if (std::fabs(alpha - 2) < MIN_POSITIVE)
        alpha = 2;
    else if (std::fabs(alpha - 1) < MIN_POSITIVE)
        alpha = 1;
    else if (std::fabs(alpha - .5) < MIN_POSITIVE)
        alpha = .5;

    if (std::fabs(beta) < MIN_POSITIVE)
        beta = 0;
    else if (std::fabs(beta - 1) < MIN_POSITIVE)
        beta = 1;
    else if (std::fabs(beta + 1) < MIN_POSITIVE)
        beta = -1;

    if (alpha == 2) /// X ~ Normal(mu, 2sigma^2)
    {
        N.setMean(mu);
        N.setSigma(sigma * M_SQRT2);
    }
    else if (alpha == 1)
    {
        if (beta == 0) /// X ~ Cauchy(mu, sigma)
        {
            C.setLocation(mu);
            C.setScale(sigma);
        }
        else /// just alpha == 1
        {
            logSigma = std::log(sigma);
            pdfCoef = 0.5 / beta;
        }
    }
    else if (alpha == .5)
    {
        if (std::fabs(beta) == 1) /// +/- X ~ Levy(mu, sigma)
        {
            L.setLocation(mu);
            L.setScale(sigma);
        }
    }
    else /// Common case: alpha != 1
    {
        B = beta * std::tan(M_PI_2 * alpha);
        zeta = -B;
        S = std::pow(1 + B * B, .5 * alphaInv);
        B = std::atan(B);
        pdfCoef = M_1_PI * alpha / (std::fabs(1 - alpha) * sigma);
        xi = alphaInv * B;
        integrandCoef = std::pow(qFastCos(B), alpham1Inv);
    }
}

double StableRand::variate()
{
    /// Check all 'good' cases
    if (alpha == 2)
        return N.variate();
    if (alpha == 1 && beta == 0)
        return C.variate();
    if (alpha == .5 && beta == 1)
    {
        if (beta == 1)
            return L.variate();
        if (beta == -1)
            return -L.variate();
    }

    /// Now check the others
    double rv = 0;
    int iter = 0;
    do {
        if (alpha == 1)
            rv = variateForAlphaEqualOne();
        else
            rv = variateForCommonAlpha();
        ++iter;
    } while ((std::isnan(rv) || std::isinf(rv)) && /// there could occure some numerical problems
             iter < 10); /// if we got nan 10 times - we have a problem, get out
    return rv;
}

double StableRand::variateForCommonAlpha()
{
    double V = U.variate();
    double W = Exp.variate();
    double alphaVB = alpha * V + B;
    double rv = S * qFastSin(alphaVB); /// S * sin(alpha * V + B)
    double W_adj = W / qFastCos(V - alphaVB);
    rv *= W_adj; /// S * sin(alpha * V + B) * W / cos((1 - alpha) * V - B)
    rv *= std::pow(W_adj * qFastCos(V), -alphaInv);/// S * sin(alpha * V + B) * W / cos((1 - alpha) * V - B) /
                                                   /// ((W * cos(V) / cos((1 - alpha) * V - B)) ^ (1 / alpha))
    return mu + sigma * rv;
}

double StableRand::variateForAlphaEqualOne()
{
    double V = U.variate();
    double W = Exp.variate();
    double pi_2BetaV = M_PI_2 + beta * V;

    double rv = logSigma;
    rv -= std::log(M_PI_2 * W * qFastCos(V) / pi_2BetaV);
    rv *= beta;
    rv += pi_2BetaV * std::tan(V);
    rv *= M_2_PI;
    return mu + sigma * rv;
}

double StableRand::f(double x) const
{
    /// Check all 'good' cases
    if (alpha == 2)
        return N.f(x);
    if (alpha == 1 && beta == 0)
        return C.f(x);
    if (alpha == .5 && beta == 1)
    {
        if (beta == 1)
            return L.f(x);
        if (beta == -1)
            return L.f(-x);
    }

    /// Now check the others
    if (alpha == 1)
        return pdfForAlphaEqualOne(x);
    return pdfForCommonAlpha(x);
}

double StableRand::pdfForCommonAlpha(double x) const
{
    x = (x - mu) / sigma; /// Standardize

    if (std::fabs(x) < 0.1) /// if we are close to 0 then we do interpolation avoiding dangerous variates
    {
        double numerator = std::tgamma(1 + alphaInv);
        numerator *= qFastCos(xi);
        double denominator = sigma * std::pow(1 + zeta * zeta, .5 * alphaInv);
        double y0 =  M_1_PI * numerator / denominator; /// f(0)
        if (std::fabs(x) < MIN_POSITIVE)
            return y0;
        double b = (x > 0) ? 0.11 : -0.11;
        double y1 = pdfForCommonAlpha(mu + sigma * b);
        return RandMath::linearInterpolation(0, b, y0, y1, x);
    }

    double xiAdj = xi; /// +- xi
    if (x > 0)
    {
        if (alpha < 1 && beta == -1)
            return 0;
    }
    else
    {
        if (alpha < 1 && beta == 1)
            return 0;
        x = -x;
        xiAdj = -xi;
    }

    double xAdj = std::pow(x, alpha_alpham1);

    /// find the peak of integrand
    double theta0 = M_PI_4 - .5 * xiAdj;
    RandMath::findRoot([this, xAdj, xiAdj] (double theta)
    {
        return integrandAuxForCommonAlpha(theta, xAdj, xiAdj) - 1.0;
    },
    -xiAdj, std::max(-xiAdj, 0.99 * M_PI_2), theta0);

    /// calculate two integrals
    std::function<double (double)> integrandPtr = std::bind(&StableRand::integrandForCommonAlpha, this, std::placeholders::_1, xAdj, xiAdj);
    double int1 = RandMath::integral(integrandPtr, -xiAdj, theta0);
    double int2 = RandMath::integral(integrandPtr, theta0, M_PI_2);
    return pdfCoef * (int1 + int2) / x;
}

double StableRand::pdfForAlphaEqualOne(double x) const
{
    x = (x - mu) / sigma - M_2_PI * beta * logSigma; /// Standardize

    double xAdj = std::exp(-M_PI * x * pdfCoef);

    /// find peak of integrand
    double theta0 = 0;
    if (beta > 0) {
        RandMath::findRoot([this, xAdj] (double theta)
        {
            return integrandAuxForAlphaEqualOne(theta, xAdj) - 1.0;
        },
        -M_PI_2, 0.99 * M_PI_2, theta0);
    }
    else {
        RandMath::findRoot([this, xAdj] (double theta)
        {
            return integrandAuxForAlphaEqualOne(theta, xAdj) - 1.0;
        },
        -0.99 * M_PI_2, M_PI_2, theta0);
    }

    std::function<double (double)> integrandPtr = std::bind(&StableRand::integrandForAlphaEqualOne, this, std::placeholders::_1, xAdj);
    double int1 = RandMath::integral(integrandPtr, -M_PI_2, theta0);
    double int2 = RandMath::integral(integrandPtr, theta0, M_PI_2);

    return std::fabs(pdfCoef) * (int1 + int2) / sigma;
}

double StableRand::integrandAuxForAlphaEqualOne(double theta, double xAdj) const
{
    double cosTheta = qFastCos(theta);
    /// if theta ~ +-pi / 2
    if (std::fabs(cosTheta) < MIN_POSITIVE)
        return 0.0;
    double thetaAdj = (M_PI_2 + beta * theta) / cosTheta;
    double u = M_2_PI * thetaAdj;
    u *= std::exp(thetaAdj * qFastSin(theta) / beta);
    return u * xAdj;
}

double StableRand::integrandForAlphaEqualOne(double theta, double xAdj) const
{
    double u = integrandAuxForAlphaEqualOne(theta, xAdj);
    return u * std::exp(-u);
}

double StableRand::integrandAuxForCommonAlpha(double theta, double xAdj, double xiAdj) const
{
    double thetaAdj = alpha * (theta + xiAdj);
    double sinThetaAdj = qFastSin(thetaAdj);
    /// if theta ~ 0
    if (std::fabs(sinThetaAdj) < MIN_POSITIVE)
        return 0.0;
    double cosTheta = qFastCos(theta);
    double y = cosTheta / sinThetaAdj;
    /// if theta ~ pi / 2
    if (std::fabs(y) < MIN_POSITIVE)
        return 0.0;
    y = std::pow(y, alpha_alpham1);
    y *= qFastCos(thetaAdj - theta);
    y /= cosTheta;
    return integrandCoef * xAdj * y;
}

double StableRand::integrandForCommonAlpha(double theta, double xAdj, double xiAdj) const
{
    double u = integrandAuxForCommonAlpha(theta, xAdj, xiAdj);
    return u * std::exp(-u);
}

double StableRand::F(double x) const
{
    /// Check all 'good' cases
    if (alpha == 2)
        return N.F(x);
    if (alpha == 1 && beta == 0)
        return C.F(x);
    if (alpha == .5 && beta == 1)
    {
        if (beta == 1)
            return L.F(x);
        if (beta == -1)
            return 1.0 - L.f(-x);
    }

    /// Now check the others
    if (alpha == 1)
        return cdfForAlphaEqualOne(x);
    return cdfForCommonAlpha(x);
}