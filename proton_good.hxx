/** @file proton_good.hxx
 * Copyright (C) 2013 Houghton College 
 */

#ifndef N2N_PROTON_INCL
#define N2N_PROTON_INCL

namespace n2n {

struct Region
{
	Int_t min_x, max_x;
	Int_t min_y, max_y;
};

namespace proton {

TH2I * ParseDataFile( char const * const filename );
Region ParseHeaderFile( char const * const filename );
Int_t CountsInRegion( TH2I const * const data, Region const & roi );

} // namespace proton
} // namespace n2n

#endif
