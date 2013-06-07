/** @file proton.hxx
 * Copyright (C) 2012 Houghton College
 */

#ifndef PROTON_HXX_INCLUDED_
#define PROTON_HXX_INCLUDED_

/** 
 * A region of interest
 */
struct Region
{
	int xmin, ymin;
	int xmax, ymax;

	Region( int xmin, int ymin, int xmax, int ymax ) : 
		xmin( xmin ), ymin( ymin ), 
		xmax( xmax ), ymax( ymax )
	{}
};


namespace proton {

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

/**
 * Get the number of points in the TNtuple within a specified Region.
 * @param data The run data
 * @param roi The region of interest
 * @return The number of points within the region.
 */
int CountsInRegion( TNtuple * data, Region roi );

} // namespace proton

#endif // PROTON_HXX_INCLUDED_
