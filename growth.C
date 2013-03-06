/** @file growth.C
 * Copyright (C) 2012, 2013 Houghton College
 *
 * Contains functions for loading and fitting a growth curve.
 */

#include <vector>

namespace c11 {

/**
 * Parse a decay curve given in a tab-separated file into a TGraphErrors 
 * object containing the corresponding growth curve.
 *
 * @param file The TSystemFile to read. This input file should 
 * be the output of the MPA4 software set to its CSV output mode and contain
 * data from a decay curve.
 *
 * @return A TGraphErrors object containing the growth curve corresponding
 * to the input decay curve. The error is calculated to be the square root
 * of the number of counts.
 *
 * @deprecated Use @ref c11::CreateDecayCurve
 */
TGraphErrors * CreateGrowthCurve( TSystemFile * file )
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
	float cur_time, cur_count, tot_count; 
	while ( ifs >> cur_time >> cur_count )
	{
		times.push_back( cur_time );
		tot_count += cur_count;
		counts.push_back( tot_count );
		errors.push_back( std::sqrt( tot_count ) );
	}

	// Construct the TGraphErrors object
	return new TGraphErrors( times.size(), &times[0], &counts[0], 
				 NULL, &errors[0] );
}




/**
 * Fit an exponential growth curve to a TGraphErrors object.
 * The growth curve is given by @f$R_0(1-e^{-\lambda t})+At+B@f$.
 *
 * @param ge The TGraphErrors object to be fit.
 * 
 * @return A TFitResultPtr containing the results of the fit.
 *
 * @deprecated Use @ref c11::FitDecayCurve
 */
TFitResultPtr FitGrowthCurve( TGraphErrors * ge )
{
	Double_t xmin, ymin, xmax, ymax;
	ge->ComputeRange( xmin, ymin, xmax, ymax );

	// Create the function to fit to the growth curve
	TF1 * growth = 
		new TF1( "growth", "[0]*(1-TMath::Exp(-[1]*x))+[2]*x+[3]", 
			 xmin, xmax );
	growth->SetParNames( "R_{0}", "#lambda", "A", "B" );
	growth->SetParameter( 0, ymax );
	growth->SetParameter( 1, TMath::Log( 2 ) / 20.38 );
	growth->SetParameter( 2, 0 );
	growth->SetParameter( 3, 0 );

	// Actually fit the decay curve.
	return ge->Fit( growth, "s", "", xmin, xmax );
}


/**
 * Calculate the total number of C11 originally in the sample.
 * This count is given by @f$R_0e^{\lambda\,\mathrm{trans\_time}}
 * /\mathrm{efficiency}@f$.
 *
 * To get the uncertainty in this value, use @ref GetErrorGrowth.
 *
 * @param fr	The TFitResultPtr returned by @ref FitGrowthCurve.
 * @param trans_time	The elapsed time before counting began
 * @param efficiency	The efficiency of counting in this sample
 *
 * @return The total number of C11 originally present in the sample.
 *
 * @deprecated Use c11::GetCountDecay.
 */
Double_t GetCountGrowth( 
		TFitResultPtr fr, 
		Double_t trans_time, 
		Double_t efficiency )
{
	double r0 = fr->Parameter( 0 );
	double lambda = fr->Parameter( 1 );
	return r0 * TMath::Exp( lambda * trans_time ) / efficiency;
}

/**
 * Calculate the uncertainty in the total number of C11.
 * The uncertainty is given by 
 * @f$\frac{\sqrt{(e^{\lambda\,\mathrm{trans\_time}}\delta_{R_0})^2
 * +(R_0\lambda e^{\lambda\,\mathrm{trans\_time}}\delta_\lambda)^2}}
 * {\mathrm{efficiency}}@f$.
 *
 * To get the total number, use @ref GetCountGrowth.
 *
 * @param fr	The TFitResultPtr returned by @ref FitGrowthCurve.
 * @param trans_time	The elapsed time before counting began.
 * @param efficiency	The efficiency of counting this sample.
 *
 * @return The uncertainty in the total number of C11.
 *
 * @deprecated Use c11::GetErrorDecay
 */
Double_t GetErrorGrowth( 
		TFitResultPtr fr,
		Double_t trans_time,
		Double_t efficiency )
{
	double r0 = fr->Parameter( 0 );
	double lambda = fr->Parameter( 1 );
	double r0_error = fr->ParError( 0 );
	double lambda_error = fr->ParError( 1 );
	double elt = TMath::Exp( lambda * trans_time );
	return TMath::Sqrt( TMath::Power( elt * r0_error, 2 ) +
			TMath::Power( r0 * lambda * elt * lambda_error, 2 ) ) 
		/ efficiency;
}

} //namespace c11
