/** @file growth.C
 * Copyright (C) 2013 Houghton College
 *
 * Contains functions for loading and fitting a decay curve.
 */

#include <vector>

namespace c11 {

/**
 * Parse a decay curve given in a tab-separated file into a TGraphErrors 
 * object.
 *
 * @param file The TSystemFile to read. This input file should 
 * be the output of the MPA4 software set to its CSV output mode and contain
 * data from a decay curve.
 *
 * @return A TGraphErrors object containing the input decay curve. 
 * The error is calculated to be the square root of the number of counts.
 */
TGraphErrors * CreateDecayCurve( TSystemFile * file )
{
	// Open the file for reading
	std::ifstream ifs( gSystem->ConcatFileName( 
				file->GetTitle(), file->GetName() ), 
			   std::ios::in );

	// Skip to the [DATA] section of the file
	std::string line;
	while ( std::getline( ifs, line ) )
	{
		if ( line == "[DATA]" )
			break;
	}

	// Read the data from the decay curve given in the file into a
	// growth curve that can be fit more accurately. 
	std::vector<float> times;
	std::vector<float> counts;
	std::vector<float> errors; 
	float cur_time, cur_count; 
	while ( ifs >> cur_time >> cur_count )
	{
		times.push_back( cur_time );
		counts.push_back( cur_count );
		errors.push_back( std::sqrt( cur_count ) );
	}

	// Construct the TGraphErrors object
	return new TGraphErrors( times.size(), &times[0], &counts[0], 
				 NULL, &errors[0] );
}




/**
 * Fit an exponential decay curve to a TGraphErrors object.
 * The decay curve is given by @f$N_0 e^{-\lambda t}+A@f$.
 *
 * @param ge The TGraphErrors object to be fit.
 * 
 * @return A TFitResultPtr containing the results of the fit.
 */
TFitResultPtr FitDecayCurve( TGraphErrors * ge )
{
	Double_t xmin, ymin, xmax, ymax;
	ge->ComputeRange( xmin, ymin, xmax, ymax );

	// Create the function to fit to the growth curve
	TF1 * growth = 
		new TF1( "growth", "[0]*TMath::Exp(-[1]*x)+[2]", 
			 xmin, xmax );
	growth->SetParNames( "N_{0}", "#lambda", "A" );
	growth->SetParameter( 0, ymax );
	growth->SetParameter( 1, TMath::Log( 2 ) / 20.38 );
	growth->SetParameter( 2, 0 );

	// Actually fit the decay curve.
	return ge->Fit( growth, "s", "", xmin, xmax );
}


/**
 * Calculate the total number of C11 originally in the sample.
 * This count is given by @f$\frac{N_0e^{\lambda\,\mathrm{trans\_time}}}
 * {\lambda\,\mathrm{efficiency}}@f$.
 *
 * To get the uncertainty in this value, use @ref GetErrorDecay.
 *
 * @param fr	The TFitResultPtr returned by @ref FitDecayCurve.
 * @param trans_time	The elapsed time before counting began
 * @param efficiency	The efficiency of counting in this sample
 *
 * @return The total number of C11 originally present in the sample.
 */
Double_t GetCountDecay( 
		TFitResultPtr fr, 
		Double_t trans_time, 
		Double_t efficiency )
{
	Double_t n0 = fr->Parameter( 0 );
	Double_t lambda = fr->Parameter( 1 );
	return n0 * TMath::Exp( lambda * trans_time ) / (lambda * efficiency);
}

/**
 * Calculate the uncertainty in the total number of C11.
 * The uncertainty is given by 
 * @f$\frac{e^{\lambda\,\mathrm{trans\_time}}}{\lambda\,\mathrm{efficiency}}
 * \sqrt{\delta_{N_0}^2+N_0^2(\frac{1}{\lambda}-\mathrm{trans\_time})^2
 * \delta_\lambda^2}@f$.
 *
 * To get the total number, use @ref GetCountDecay.
 *
 * @param fr	The TFitResultPtr returned by @ref FitDecayCurve.
 * @param trans_time	The elapsed time before counting began.
 * @param efficiency	The efficiency of counting this sample.
 *
 * @return The uncertainty in the total number of C11.
 */
Double_t GetErrorDecay( 
		TFitResultPtr fr,
		Double_t trans_time,
		Double_t efficiency )
{
	Double_t n0 = fr->Parameter( 0 );
	Double_t lambda = fr->Parameter( 1 );
	Double_t err_n0 = fr->ParError( 0 );
	Double_t err_lambda = fr->ParError( 1 );

	return TMath::Exp( lambda * trans_time ) / (lambda * efficiency) 
		* TMath::Sqrt( TMath::Power( err_n0, 2 )
				+ TMath::Power( n0 * (1 / lambda - trans_time) 
					* err_lambda, 2 ) );
}

} // namespace c11

