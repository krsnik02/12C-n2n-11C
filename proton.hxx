/** @file proton.hxx
 * Copyright (C) 2012 Houghton College
 */

#ifndef PROTON_HXX_INCLUDED_
#define PROTON_HXX_INCLUDED_

namespace proton {


struct Region
{
	int xmin, ymin, xmax, ymax;
	Region( int xmin, int ymin, int xmax, int ymax )
		: xmin( xmin ), ymin( ymin ), xmax( xmax ), ymax( ymax )
	{}

	TCut AsTCut()
	{
		TCut cut_xmin = TString::Format( "x >= %d", xmin );
		TCut cut_ymin = TString::Format( "y >= %d", ymin );
		TCut cut_xmax = TString::Format( "x < %d", xmax );
		TCut cut_ymax = TString::Format( "y < %d", ymax );
		return cut_xmin && cut_ymin && cut_xmax && cut_ymax;	
	}
};

/**
 * Parse a .mpa file to determine the correct region of interest for the run.
 * @param filename The .mpa file to read
 * @return The region of interest
 */
Region MPAGetROI( char const * filename );

/**
 * Read the data for a run from a .csv file into a TNtuple.
 * @param filename The .csv file to read
 * @return A TNtuple containing the run data
 */
TNtuple * CSVGetData( char const * filename );

} // namespace proton

#endif // PROTON_HXX_INCLUDED_
