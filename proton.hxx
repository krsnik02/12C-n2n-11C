/** @file proton.hxx
 * Copyright (C) 2013 Houghton College 
 */

#ifndef N2N_PROTON_INCL
#define N2N_PROTON_INCL

namespace n2n {

/**
 * A region of interest.
 */
struct Region
{
	Int_t min_x; 	///< The smallest x-value in the region.
	Int_t max_x;	///< The largest x-value in the region.
	Int_t min_y;	///< The smallest y-value in the region.
	Int_t max_y;	///< The largest y-value in the region.
};

namespace proton {

/**
 * Parse the .csv data file produced by MPA4 for the proton telescope.
 * @param filename The path to the file.
 * @return A 2D-histogram containing the data as a dE-E plot.
 */
TH2I * ParseDataFile( char const * const filename );

/**
 * Parse the .mpa file produced by MPA4 for the proton telescope.
 * @param filename The path to the file.
 * @return The region of interest for the run.
 */
Region ParseHeaderFile( char const * const filename );

/**
 * Determine the total number of counts in the region of interest.
 * @param data The dE-E data.
 * @param roi The region of interest.
 * @return The number of counts in the region.
 */
Int_t CountsInRegion( TH2I const * const data, Region const & roi );

} // namespace proton
} // namespace n2n

#endif
