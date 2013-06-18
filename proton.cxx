/** 
 * @file n2n/proton.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "proton.hxx"

#include <stdexcept>

namespace n2n {
namespace proton {

TH2I * ParseDataFile( char const * const filename )
{
	ifstream is( filename );

	string line;
	getline( is, line );
	if ( line.substr( 0, 9 ) != "[DISPLAY]" )
	{
		cerr << "Not a valid CSV Data file: " << filename << endl;
		throw runtime_error( "Invalid CSV file" );
	}

	while ( line != "[DATA]" )
	{
		while ( getline( is, line ) && line[0] != '[' )
		{
			// Do nothing
		}
	}

	TH2I * hist = new TH2I( filename, filename, 1024, 1, 1024, 1024, 1, 1024 );
	Int_t a2, a1, value;
	while ( is >> a2 >> a1 >> value )
	{
		hist->Fill( a2, a1, value );
	}

	return hist;
}

Region ParseHeaderFile( char const * const filename )
{
	std::ifstream ifs( filename );
	std::string line;
	
	// Check that the file is actually a MPA file
	std::getline( ifs, line );
	if ( line.substr( 0, 7 ) != "[MPA4A]" )
	{
		std::cerr << "Not a valid MPA file: " << filename << std::endl;
		throw std::runtime_error( "Invalid file" );
	}

	// Skip to the [MAP0] section
	while ( line.substr( 0, 6 ) != "[MAP0]" )
	{
		while ( std::getline( ifs, line ) && line[0] != '[' )
		{
			// Do nothing
		}
	}

	// Parse the [MAP0] section
	std::string str_xdim, str_roi;
	while ( std::getline( ifs, line ) && line[0] != '[' )
	{
		// Ensure [MAP0] is actually a_2 x a_1
		if ( line.substr( 0, 6 ) == "param=" && line != "param=1" )
		{
			std::cerr << "Expected 'param=1'" << std::endl;
			std::clog << line << std::endl;
			return false;
		}

		// Locate X dimension
		if ( line.substr( 0, 5 ) == "xdim=" )
			str_xdim = line;

		// Locate regions of interest
		if ( line.substr( 0, 4 ) == "roi=" )
			str_roi = line;
	}

	// Determine region of interest
	int xdim, roi_min, roi_max;
	std::sscanf( str_xdim.c_str(), "xdim=%d", &xdim );
	std::sscanf( str_roi.c_str(), "roi=%d %d", &roi_min, &roi_max );

	// Calculate region boundaries
	Region roi;
	roi.min_x = roi_min - (roi_min / xdim) * xdim;
	roi.min_y = (roi_min / xdim);
	roi.max_x = roi_max - (roi_max / xdim) * xdim;
	roi.max_y = (roi_max / xdim);
	return roi;
}

Int_t CountsInRegion( TH2I const * const data, Region const & roi )
{
	Int_t sum = 0;
	for ( Int_t x = roi.min_x; x <= roi.max_x; ++x )
	{
		for ( Int_t y = roi.min_y; y <= roi.max_y; ++y )
		{
			Int_t bin = data->GetBin( x, y );
			sum += data->GetBinContent( bin );
		}
	}
	return sum;
}

} // namespace proton
} // namespace n2n
