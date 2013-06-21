/**
 * @file n2n/Uncertain.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "Uncertain.hxx"

namespace n2n {

UncertainD ReadUncertainD( vector<string> const & row, int val_col, int unc_col )
{
	UncertainD ret;
	ret.val = atof( row[val_col].c_str() );
	ret.unc = atof( row[unc_col].c_str() );
	return ret;
}

void WriteUncertainD( UncertainD const & value, vector<string> * const row, int val_col, int unc_col )
{
	(*row)[val_col] = TString::Format( "%f", value.val );
	(*row)[unc_col] = TString::Format( "%f", value.unc );
}

} // namespace n2n
