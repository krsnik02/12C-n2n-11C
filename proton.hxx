/** 
 * @file n2n/proton.hxx
 * Copyright (C) 2013 Houghton College 
 */

#ifndef N2N_PROTON_INCL_
#define N2N_PROTON_INCL_

#include "Region.hxx"

namespace n2n {
namespace proton {

/**
 * Parse the .csv data file produced by MPA4 for the proton telescope.
 *
 * @param filename The path to the file.
 *
 * @return A 2D-histogram containing the data as a dE-E plot.
 */
TH2I * ParseDataFile( char const * const filename );

/**
 * Parse the .mpa file produced by MPA4 for the proton telescope.
 *
 * @param filename The path to the file.
 *
 * @return The region of interest for the run.
 */
Region ParseHeaderFile( char const * const filename );

/**
 * Determine the total number of counts in the region of interest.
 *
 * @param data The dE-E data.
 * @param roi The region of interest.
 *
 * @return The number of counts in the region.
 */
Int_t CountsInRegion( TH2I const * const data, Region const & roi );

} // namespace proton
} // namespace n2n

#endif
