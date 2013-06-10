/** @file decay.cxx
 * Copyright (C) 2013 Houghton College
 *
 * Contains functions for determining C11 activation counts.
 */

#include "decay.hxx"
#include <vector>

namespace decay {

TGraphErrors * CSVGetData( char const * filename )
{
	// Open the file for reading
	std::ifstream ifs( filename, std::ios::in );

	// Skip to the [DATA] section of the file
	std::string line;
	while ( std::getline( ifs, line ) )
	{
		if ( line == "[DATA]" )
			break;
	}

	// Read the data from the decay curve given in the file.
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




TFitResultPtr Fit( TGraphErrors * ge )
{
	Double_t xmin, ymin, xmax, ymax;
	ge->ComputeRange( xmin, ymin, xmax, ymax );

	// Create the function to fit to the growth curve
	TF1 * growth = 
		new TF1( "growth", "[0]*TMath::Exp(-[1]*x)+[2]", 
			 xmin, xmax );
	growth->SetParNames( "N_{0}", "#lambda", "A" );
	growth->SetParameter( 0, ymax );
	growth->FixParameter( 1, TMath::Log( 2 ) / 20.334 );
	growth->SetParameter( 2, 0 );

	// Actually fit the decay curve.
	return ge->Fit( growth, "s", "", xmin, xmax );
}


Double_t GetCount( TFitResultPtr fr, Double_t trans_time, Double_t efficiency )
{
	Double_t n0 = fr->Parameter( 0 );
	Double_t lambda = fr->Parameter( 1 );
	return n0 * TMath::Exp( lambda * trans_time ) / (lambda * efficiency);
}

Double_t GetError( TFitResultPtr fr, Double_t trans_time, Double_t efficiency )
{
	Double_t lambda = fr->Parameter( 1 );
	Double_t err_n0 = fr->ParError( 0 );
	return err_n0 * TMath::Exp( lambda * trans_time ) / (lambda * efficiency);
}

} // namespace decay

