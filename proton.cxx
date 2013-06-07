/** @file proton.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "proton.hxx"
#include <vector>
#include <stdexcept>

namespace proton {

Region MPAGetROI( char const * filename )
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
	return Region( roi_min - TMath::Floor( roi_min / xdim ) * xdim,
		       TMath::Floor( roi_min / xdim ),
		       roi_max - TMath::Floor( roi_max / xdim ) * xdim,
		       TMath::Floor( roi_max / xdim ) );
}


TNtuple * CSVGetData( char const * filename )
{
	std::ifstream ifs( filename );
	std::string line;

	// Check that this is a valid CSV file
	std::getline( ifs, line );
	if ( line != "[DISPLAY]" )
	{
		std::cerr << "Not a valid CSV file: " << filename << std::endl;
		throw std::runtime_error( "Invalid file" );
	}

	// Skip to the [DATA] section
	while ( line != "[DATA]" )
	{
		while ( std::getline( ifs, line ) && line[0] != '[' )
		{
			// Do nothing
		}
	}

	// Parse the [DATA] section
	TNtuple * ntup = new TNtuple( "", "", "x:y:counts" );
	float a2, a1, value;
	while ( ifs >> a2 >> a1 >> value )
	{
		ntup->Fill( a2, a1, value );
	}
	return ntup;
}


int CountsInRegion( TNtuple * ntup, Region roi )
{
	TCut cut_xmin = TString::Format( "x >= %d", roi.xmin );
	TCut cut_ymin = TString::Format( "y >= %d", roi.ymin );
	TCut cut_xmax = TString::Format( "x < %d", roi.xmax );
	TCut cut_ymax = TString::Format( "y < %d", roi.ymax );
	TCut cut = cut_xmin && cut_ymin && cut_xmax && cut_ymax;
	return ntup->GetEntries( cut );
}

} // namespace proton
