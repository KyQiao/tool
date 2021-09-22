#pragma once
#include <cmath>

namespace math {
double bessel_j0(double &);
double itj0(double &);

double evaluate_rational(const double *num, const double *denom, const double &z_, std::size_t count) {
  double z(z_);
  double s1, s2;
  if (z <= 1) {
    s1 = static_cast<double>(num[count - 1]);
    s2 = static_cast<double>(denom[count - 1]);
    for (int i = (int)count - 2; i >= 0; --i) {
      s1 *= z;
      s2 *= z;
      s1 += num[i];
      s2 += denom[i];
    }
  } else {
    z = 1 / z;
    s1 = static_cast<double>(num[0]);
    s2 = static_cast<double>(denom[0]);
    for (unsigned i = 1; i < count; ++i) {
      s1 *= z;
      s2 *= z;
      s1 += num[i];
      s2 += denom[i];
    }
  }
  return s1 / s2;
}

double bessel_j0(double &x) {
  static const double P1[] = {
      static_cast<double>(-4.1298668500990866786e+11),
      static_cast<double>(2.7282507878605942706e+10),
      static_cast<double>(-6.2140700423540120665e+08),
      static_cast<double>(6.6302997904833794242e+06),
      static_cast<double>(-3.6629814655107086448e+04),
      static_cast<double>(1.0344222815443188943e+02),
      static_cast<double>(-1.2117036164593528341e-01)};
  static const double Q1[]{
      static_cast<double>(2.3883787996332290397e+12),
      static_cast<double>(2.6328198300859648632e+10),
      static_cast<double>(1.3985097372263433271e+08),
      static_cast<double>(4.5612696224219938200e+05),
      static_cast<double>(9.3614022392337710626e+02),
      static_cast<double>(1.0),
      static_cast<double>(0.0)};
  static const double P2[]{
      static_cast<double>(-1.8319397969392084011e+03),
      static_cast<double>(-1.2254078161378989535e+04),
      static_cast<double>(-7.2879702464464618998e+03),
      static_cast<double>(1.0341910641583726701e+04),
      static_cast<double>(1.1725046279757103576e+04),
      static_cast<double>(4.4176707025325087628e+03),
      static_cast<double>(7.4321196680624245801e+02),
      static_cast<double>(4.8591703355916499363e+01)};
  static const double Q2[]{
      static_cast<double>(-3.5783478026152301072e+05),
      static_cast<double>(2.4599102262586308984e+05),
      static_cast<double>(-8.4055062591169562211e+04),
      static_cast<double>(1.8680990008359188352e+04),
      static_cast<double>(-2.9458766545509337327e+03),
      static_cast<double>(3.3307310774649071172e+02),
      static_cast<double>(-2.5258076240801555057e+01),
      static_cast<double>(1.0)};
  static const double PC[]{
      static_cast<double>(2.2779090197304684302e+04),
      static_cast<double>(4.1345386639580765797e+04),
      static_cast<double>(2.1170523380864944322e+04),
      static_cast<double>(3.4806486443249270347e+03),
      static_cast<double>(1.5376201909008354296e+02),
      static_cast<double>(8.8961548424210455236e-01)};
  static const double QC[]{
      static_cast<double>(2.2779090197304684318e+04),
      static_cast<double>(4.1370412495510416640e+04),
      static_cast<double>(2.1215350561880115730e+04),
      static_cast<double>(3.5028735138235608207e+03),
      static_cast<double>(1.5711159858080893649e+02),
      static_cast<double>(1.0)};
  static const double PS[]{
      static_cast<double>(-8.9226600200800094098e+01),
      static_cast<double>(-1.8591953644342993800e+02),
      static_cast<double>(-1.1183429920482737611e+02),
      static_cast<double>(-2.2300261666214198472e+01),
      static_cast<double>(-1.2441026745835638459e+00),
      static_cast<double>(-8.8033303048680751817e-03)};
  static const double QS[]{
      static_cast<double>(5.7105024128512061905e+03),
      static_cast<double>(1.1951131543434613647e+04),
      static_cast<double>(7.2642780169211018836e+03),
      static_cast<double>(1.4887231232283756582e+03),
      static_cast<double>(9.0593769594993125859e+01),
      static_cast<double>(1.0)};
  static const double x1 = static_cast<double>(2.4048255576957727686e+00),
                      x2 = static_cast<double>(5.5200781102863106496e+00),
                      x11 = static_cast<double>(6.160e+02),
                      x12 = static_cast<double>(-1.42444230422723137837e-03),
                      x21 = static_cast<double>(1.4130e+03),
                      x22 = static_cast<double>(5.46860286310649596604e-04);

  double value, factor, r, rc, rs;

  if (x < 0) {
    x = -x;  // even function
  }
  if (x == 0) {
    return static_cast<double>(1);
  }
  if (x <= 4)  // x in (0, 4]
  {
    double y = x * x;
    // BOOSdouble_MAdoubleH_ASSERdouble(sizeof(P1) == sizeof(Q1));
    r = evaluate_rational(P1, Q1, y, sizeof(P1) / sizeof(double));
    factor = (x + x1) * ((x - x11 / 256) - x12);
    value = factor * r;
  } else if (x <= 8.0)  // x in (4, 8]
  {
    double y = 1 - (x * x) / 64;
    // BOOSdouble_MAdoubleH_ASSERdouble(sizeof(P2) == sizeof(Q2));
    r = evaluate_rational(P2, Q2, y, sizeof(P2) / sizeof(double));
    factor = (x + x2) * ((x - x21 / 256) - x22);
    value = factor * r;
  } else  // x in (8, \infty)
  {
    double y = 8 / x;
    double y2 = y * y;
    // BOOSdouble_MAdoubleH_ASSERdouble(sizeof(PC) == sizeof(QC));
    // BOOSdouble_MAdoubleH_ASSERdouble(sizeof(PS) == sizeof(QS));
    rc = evaluate_rational(PC, QC, y2, sizeof(PC) / sizeof(double));
    rs = evaluate_rational(PS, QS, y2, sizeof(PS) / sizeof(double));
    factor = 5.641895835477562869480794515607725858e-01 / std::sqrt(x);
    //
    // What follows is really just:
    //
    // double z = x - pi/4;
    // value = factor * (rc * cos(z) - y * rs * sin(z));
    //
    // But using the addition formulae for sin and cos, plus
    // the special values for sin/cos of pi/4.
    //
    double sx = std::sin(x);
    double cx = std::cos(x);
    value = factor * (rc * (cx + sx) - y * rs * (sx - cx));
  }

  return value;
}

// https://github.com/scipy/scipy/blob/69d9d06526f757fa087f689b22dd19bc4ee0a863/scipy/special/specfun/specfun.f
// line 8928
double itj0(double &X) {
  double PI = 3.141592653589793;
  double EL = .5772156649015329;
  double EPS = 1e-12;
  if (X == 0) {
    return 0;
  } else if (X < 20) {
    double X2 = X * X;
    double TJ = X;
    double R = X;
    for (std::size_t K = 1; K < 60; K++) {
      R = -0.25 * R * (2 * K - 1.0) / (2 * K + 1.0) / (K * K) * X2;
      TJ = TJ + R;
    }
    return TJ;
  } else {
    double A0 = 1.0;
    double A1 = 5.0 / 8.0;
    double A[16];
    A[1] = A1;
    for (std::size_t K = 1; K < 16; K++) {
      double AF = ((1.5 * (K + .5) * (K + 5.0 / 6.0) * A1 - .5 * (K + .5) * (K + .5) * (K - .5) * A0)) / (K + 1.0);
      A[K] = AF;
      A0 = A1;
      A1 = AF;
    }

    double BF = 1.0;
    double R = 1.0;

    for (std::size_t K = 1; K < 8; K++) {
      R = -R / (X * X);
      BF = BF + A[2 * K] * R;
    }

    double BG = A[1] / X;
    R = 1.0 / X;
    for (std::size_t K = 0; K < 8; K++) {
      R = -R / (X * X);
      BG = BG + A[2 * K + 1] * R;
    }

    double XP = X + .25 * PI;
    double RC = std::sqrt(2.0 / (PI * X));
    double TJ = 1.0 - RC * (BF * std::cos(XP) + BG * std::sin(XP));
    return TJ;
  }
}
}  // namespace math