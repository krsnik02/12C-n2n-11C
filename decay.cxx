/** 
 * @file n2n/decay.cxx
 * Copyright (C) 2013 Houghton College
 *
 * Contains functions for determining C11 activation counts.
 */

#include "decay.hxx"
#include <vector>

namespace n2n {
namespace decay {

TGraphErrors * ParseDataFile( char const * filename )
{
	ifstream is( filename );

	string line;
	while ( getline( is, line ) )
	{
		if ( line == "[DATA]" )
			break;
	}

	vector<float> times;
	vector<float> counts;
	vector<float> errors; 
	float cur_time, cur_count; 
	while ( is >> cur_time >> cur_count )
	{
		times.push_back( cur_time );
		counts.push_back( cur_count );
		errors.push_back( sqrt( cur_count ) );
	}

	return new TGraphErrors( times.size(), &times[0], &counts[0], 
				 NULL, &errors[0] );
}

TFitResultPtr FitDecayCurve( TGraphErrors * ge )
{
	Double_t xmin, ymin, xmax, ymax;
	ge->ComputeRange( xmin, ymin, xmax, ymax );

	TF1 * decay = 
		new TF1( "decay", "[0]*TMath::Exp(-[1]*x)+[2]", 
			 xmin, xmax );
	decay->SetParNames( "N_{0}", "#lambda", "A" );
	decay->SetParameter( 0, ymax );
	decay->FixParameter( 1, TMath::Log( 2 ) / 20.334 );
	decay->SetParameter( 2, 0 );

	return ge->Fit( decay, "s", "", xmin, xmax );
}

Error<Double_t> Counts( TFitResultPtr fr, Double_t trans_time, Double_t efficiency )
{
	Error<Double_t> n0;
	n0.value = fr->Parameter( 0 );
	n0.error = fr->ParError( 0 );
	Double_t lambda = fr->Parameter( 1 );

	Error<Double_t> n_c11;
	n_c11.value = n0.value * TMath::Exp( lambda * trans_time ) / (lambda * efficiency);
	n_c11.error = n0.error * TMath::Exp( lambda * trans_time ) / (lambda * efficiency);
	return n_c11;
}

} // namespace decay
} // namespace n2n
