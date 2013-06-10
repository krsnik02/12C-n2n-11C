/** @file decay.hxx
 * Copyright (C) 2013 Houghton College
 */

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
TGraphErrors * CSVGetData( char const * filename );

/**
 * Fit an exponential decay curve to a TGraphErrors object.
 * The decay curve is given by @f$N_0 e^{-\lambda t}+A@f$.
 *
 * @param ge The TGraphErrors object to be fit.
 * 
 * @return A TFitResultPtr containing the results of the fit.
 */
TFitResultPtr Fit( TGraphErrors * ge );

/**
 * Calculate the total number of C11 originally in the sample.
 * This count is given by @f$\frac{N_0e^{\lambda\,\cdot\,\mathrm{trans\_time}}}
 * {\lambda\,\cdot\,\mathrm{efficiency}}@f$.
 *
 * To get the uncertainty in this value, use @ref GetError.
 *
 * @param fr	The TFitResultPtr returned by @ref Fit.
 * @param trans_time	The elapsed time before counting began
 * @param efficiency	The efficiency of counting in this sample
 *
 * @return The total number of C11 originally present in the sample.
 */
Double_t GetCount( TFitResultPtr fr, Double_t trans_time, Double_t efficiency );

/**
 * Calculate the uncertainty in the total number of C11.
 * The uncertainty is given by
 * @f$\frac{\delta_{N_0}e^{\lambda\,\cdot\,\mathrm{trans\_time}}}
 * {\lambda\,\cdot\,\mathrm{efficiency}}@f$
 *
 * To get the total number, use @ref GetCount.
 *
 * @param fr	The TFitResultPtr returned by @ref Fit.
 * @param trans_time	The elapsed time before counting began.
 * @param efficiency	The efficiency of counting this sample.
 *
 * @return The uncertainty in the total number of C11.
 */
Double_t GetError( TFitResultPtr fr, Double_t trans_time, Double_t efficiency );

} // namespace decay
