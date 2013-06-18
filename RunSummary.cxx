/** 
 * @file n2n/RunSummary.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "RunSummary.hxx"
#include "proton.hxx"
#include "decay.hxx"

namespace n2n {

vector<string> RunSummary::GetRun( int run_number ) const
{
	vector<string> row = GetRow( run_number + 2 );
	assert( atoi( row[n2n::RS_RUN_NUMBER].c_str() ) == run_number );
	return row;
}

void RunSummary::SetRun( int run_number, vector<string> const & row )
{
	assert( atoi( row[n2n::RS_RUN_NUMBER].c_str() ) == run_number );
	SetRow( run_number + 2, row );
}

int RunSummary::NumRuns() const
{
	return NumRows() - 2;
}

void UpdateC11( vector<string> & run, char const * dirname )
{
	int run_number = atoi( run[n2n::RS_RUN_NUMBER].c_str() );
	char const * filename_puck = 
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d_puck.csv", run_number ) );
	char const * filename_plastic =
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d_plastic.csv", run_number ) );

	double trans_time = atoi( run[n2n::RS_INTERIM_TIME].c_str() ) / 60.0;	// min
	double efficiency = 0.12;
	
	// NOTE: TSystem::AccessPathName returns *false* if the file exists!
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	if ( !gSystem->AccessPathName( filename_puck ) )
	{
		TGraphErrors * ge = decay::ParseDataFile( filename_puck );
		TFitResultPtr fr = decay::FitDecayCurve( ge );
		Error<Double_t> n_c11 = decay::Counts( fr, trans_time, efficiency );
		delete ge;

		run[n2n::RS_C11_PUCK] = TString::Format( "%f", n_c11.value );
		run[n2n::RS_C11_PUCK_ERR] = TString::Format( "%f", n_c11.error );
	}

	if ( !gSystem->AccessPathName( filename_plastic ) )
	{
		TGraphErrors * ge = decay::ParseDataFile( filename_plastic );
		TFitResultPtr fr = decay::FitDecayCurve( ge );
		Error<Double_t> n_c11 = decay::Counts( fr, trans_time, efficiency );
		delete ge;

		run[n2n::RS_C11_PLASTIC] = TString::Format( "%f", n_c11.value );
		run[n2n::RS_C11_PLASTIC_ERR] = TString::Format( "%f", n_c11.error );
	}
}

void UpdateProtons( vector<string> & run, char const * dirname )
{
	int run_number = atoi( run[n2n::RS_RUN_NUMBER].c_str() );
	char const * filename_csv =
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d_1x2.csv", run_number ) );
	char const * filename_mpa =
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d.mpa", run_number ) );

	// NOTE: TSystem::AccessPathName returns *false* if the file exists!
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	if ( !gSystem->AccessPathName( filename_csv ) &&
			!gSystem->AccessPathName( filename_mpa ) )
	{
		TH2I * data = proton::ParseDataFile( filename_csv );
		Region roi = proton::ParseHeaderFile( filename_mpa );
		Int_t protons = proton::CountsInRegion( data, roi );
		delete data;

		run[n2n::RS_ROI_XMIN] = TString::Format( "%d", roi.min_x );
		run[n2n::RS_ROI_XMAX] = TString::Format( "%d", roi.max_x );
		run[n2n::RS_ROI_YMIN] = TString::Format( "%d", roi.min_y );
		run[n2n::RS_ROI_YMAX] = TString::Format( "%d", roi.max_y );
		run[n2n::RS_GROSS_PROTONS] = TString::Format( "%d", protons );
	}
}


void RunSummary::Update( char const * dirname )
{
	for ( int i = 1; i < NumRuns(); ++i )
	{
		vector<string> run = GetRun( i );
		n2n::UpdateC11( run, gSystem->PrependPathName( dirname, "Decay Curves" ) );
		n2n::UpdateProtons( run, gSystem->PrependPathName( dirname, "Proton Telescope" ) );
		SetRun( i, run );
	}
}

} // namespace n2n
