/** @file proton.hxx
 * Copyright (C) 2012 Houghton College
 */

#ifndef PROTON_HXX_INCLUDED_
#define PROTON_HXX_INCLUDED_

namespace proton {

/**
 * Parse a .mpa file to determine the correct region of interest for the run.
 * @param filename The .mpa file to read
 * @return The region of interest
 */
TCut MPAGetROI( char const * filename );

/**
 * Read the data for a run from a .csv file into a TNtuple.
 * @param filename The .csv file to read
 * @return A TNtuple containing the run data
 */
TNtuple * CSVGetData( char const * filename );

} // namespace proton

#endif // PROTON_HXX_INCLUDED_
