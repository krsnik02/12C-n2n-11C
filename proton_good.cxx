#include "proton_good.hxx"

#include <stdexcept>

namespace n2n {
namespace proton {

TH2I * ParseDataFile( char const * const filename )
{
	ifstream is( filename );

	string line;
	getline( is, line );
	if ( line.substr( 0, 9 ) != "[DISPLAY]" )
	{
		cerr << "Not a valid CSV Data file: " << filename << endl;
		throw runtime_error( "Invalid CSV file" );
	}

	while ( line != "[DATA]" )
	{
		while ( getline( is, line ) && line[0] != '[' )
		{
			// Do nothing
		}
	}

	TH2I * hist = new TH2I( filename, filename, 1024, 1, 1024, 1024, 1, 1024 );
	Int_t a2, a1, value;
	while ( is >> a2 >> a1 >> value )
	{
		hist->Fill( a2, a1, value );
	}

	return hist;
}

Int_t CountsInRegion( TH2I const * const data, Region const & roi )
{
	Int_t sum = 0;
	for ( Int_t x = roi.min_x; x <= roi.max_x; ++x )
	{
		for ( Int_t y = roi.min_y; y <= roi.max_y; ++y )
		{
			Int_t bin = data->GetBin( x, y );
			sum += data->GetBinContent( bin );
		}
	}
	return sum;
}

} // namespace proton
} // namespace n2n
