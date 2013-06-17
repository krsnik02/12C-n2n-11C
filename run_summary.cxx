/** @file run_summary.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "run_summary.hxx"

namespace rs {

vector<string> RunSummary::GetRun( int run_number ) const
{
	vector<string> row = GetRow( run_number + 2 );
	assert( atoi( row[RS_RUN_NUMBER].c_str() ) == run_number );
	return row;
}

void RunSummary::SetRun( int run_number, vector<string> const & row )
{
	assert( atoi( row[RS_RUN_NUMBER].c_str() ) == run_number );
	SetRow( run_number + 2, row );
}

int RunSummary::NumRuns() const
{
	return NumRows() - 2;
}

void UpdateC11( vector<string> & run, char const * dirname )
{
	int run_number = atoi( run[RS_RUN_NUMBER].c_str() );
	char const * filename_puck = 
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d_puck.csv", run_number ) );
	char const * filename_plastic =
		gSystem->PrependPathName( dirname,
				TString::Format( "Run%03d_plastic.csv", run_number ) );

	double trans_time = atoi( run[RS_INTERIM_TIME].c_str() ) / 60.0;	// min
	double efficiency = 0.12;
	
	// NOTE: TSystem::AccessPathName returns *false* if the file exists!
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	if ( !gSystem->AccessPathName( filename_puck ) )
	{
		TGraphErrors * ge = decay::CSVGetData( filename_puck );
		TFitResultPtr fr = decay::Fit( ge );
		run[RS_C11_PUCK] = 
			TString::Format( "%f", decay::GetCount( fr, trans_time, efficiency ) );
		run[RS_C11_PUCK_ERR] = 
			TString::Format( "%f", decay::GetError( fr, trans_time, efficiency ) );
	}

	if ( !gSystem->AccessPathName( filename_plastic ) )
	{
		TGraphErrors * ge = decay::CSVGetData( filename_plastic );
		TFitResultPtr fr = decay::Fit( ge );
		run[RS_C11_PLASTIC] = 
			TString::Format( "%f", decay::GetCount( fr, trans_time, efficiency ) );
		run[RS_C11_PLASTIC_ERR] = 
			TString::Format( "%f", decay::GetError( fr, trans_time, efficiency ) );
	}
}

void UpdateProtons( vector<string> & run, char const * dirname )
{
	int run_number = atoi( run[RS_RUN_NUMBER].c_str() );
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
		TNtuple * data = proton::CSVGetData( filename_csv );
		proton::Region roi = proton::MPAGetROI( filename_mpa );

		run[RS_ROI_XMIN] = TString::Format( "%d", roi.xmin );
		run[RS_ROI_XMAX] = TString::Format( "%d", roi.xmax );
		run[RS_ROI_YMIN] = TString::Format( "%d", roi.ymin );
		run[RS_ROI_YMAX] = TString::Format( "%d", roi.ymax );

		run[RS_GROSS_PROTONS] =
			TString::Format( "%d", data->GetEntries( roi.AsTCut() ) );
	}
}


void RunSummary::Update( char const * dirname )
{
	for ( int i = 1; i < NumRuns(); ++i )
	{
		vector<string> run = GetRun( i );
		rs::UpdateC11( run, gSystem->PrependPathName( dirname, "Decay Curves" ) );
		rs::UpdateProtons( run, gSystem->PrependPathName( dirname, "Proton Telescope" ) );
		SetRun( i, run );
	}
}

} // namespace rs
