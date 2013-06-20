/**
 * @file n2n/Uncertain.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_UNCERTAIN_INCL_
#define N2N_UNCERTAIN_INCL_

#include <vector>

namespace n2n {

/**
 * A value and its uncertainty.
 */
struct UncertainD
{
	double val;	///< The value
	double unc;	///< The uncertainty in the value
};

UncertainD ReadUncertainD( vector<string> const & row, int val_col, int unc_col );
void WriteUncertainD( UncertainD const & value, vector<string> const & row, int val_col, int unc_col );

} // namespace n2n

#endif
