/** 
 * @file n2n/Region.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_REGION_INCL_
#define N2N_REGION_INCL_

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

} // namespace n2n

#endif
