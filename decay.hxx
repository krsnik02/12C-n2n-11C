/** 
 * @file n2n/decay.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_DECAY_INCL_
#define N2N_DECAY_INCL_

#include "Error.hxx"

namespace n2n {
namespace decay {

/**
 * Parse a decay curve given in a tab-separated file into a TGraphErrors 
 * object.
 *
 * @param filename The file to read. This input file should 
 * be the output of the MPA4 software set to its CSV output mode and contain
 * data from a decay curve.
 *
 * @return A TGraphErrors object containing the input decay curve. 
 * The error is calculated to be the square root of the number of counts.
 */
TGraphErrors * ParseDataFile( char const * filename );

/**
 * Fit an exponential decay curve to a TGraphErrors object.
 * The decay curve is given by @f$N_0 e^{-\lambda t}+A@f$.

 * @param ge The TGraphErrors object to be fit.

 * @return A TFitResultPtr containing the results of the fit.
 */
TFitResultPtr FitDecayCurve( TGraphErrors * ge );

/**
 * Calculate the total number of C11 originally in the sample, @f$N_{C11}@f$.
 *
 * @f[N_{C11}=\frac{N_0e^{\lambda\,t_{trans}}}{\lambda\cdot\text{eff}}@f]
 * @f[\delta_{N_{C11}}=\frac{\delta_{N_0}e^{\lambda\,t_{trans}}}{\lambda\cdot\text{eff}}@f]
 *
 * @param fr The TFitResultPtr returned by @ref FitDecayCurve.
 * @param trans_time The elapsed time before counting began, @f$t_{trans}@f$
 * @param efficiency The efficiency of counting in this sample, @f$\text{eff}@f$
 *
 * @return The total number of C11 originally present in the sample.
 */
Error<Double_t> Counts( TFitResultPtr fr, Double_t trans_time, Double_t efficiency );

} // namespace decay
} // namespace n2n

#endif
