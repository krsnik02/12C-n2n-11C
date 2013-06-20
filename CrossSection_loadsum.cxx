/**
 * @file n2n/CrossSection_loadsum.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "CrossSection.hxx"

namespace n2n {
namespace loadsum {

void UpdateRunData( vector<string> & row, vector<string> const & fg, vector<string> const & bg )
{
	row[CS_NEUTRON_ENERGY] 	= fg[RS_NEUTRON_ENERGY];
	row[CS_NEUTRON_ENERGY_UNC] = "0";
	row[CS_FG_CLOCK_TIME]	= fg[RS_CLOCK_TIME];
	row[CS_FG_CLOCK_TIME_UNC] = "0";
	row[CS_FG_LIVE_FRAC] = fg[RS_TOTAL_LIVE];
	row[CS_FG_LIVE_FRAC_UNC] = "0";
	row[CS_BG_CLOCK_TIME]	= bg[RS_CLOCK_TIME];
	row[CS_BG_CLOCK_TIME_UNC] = "0";
	row[CS_BG_LIVE_FRAC] = bg[RS_TOTAL_LIVE];
	row[CS_BG_LIVE_FRAC_UNC] = "0";
}

void UpdateGeometry( vector<string> & row )
{
	row[CS_DET_AREA]	= "0.7133";	// cm^2
	row[CS_DET_AREA_UNC]	= "0";
	row[CS_DET_DISTANCE]	= "12.07";	// cm
	row[CS_DET_DISTANCE_UNC] = "0";

	row[CS_CH2_AREA]	= "5.067075";	// cm^2
	row[CS_CH2_AREA_UNC]	= "0";
	row[CS_CH2_DISTANCE]	= "6.46";	// cm
	row[CS_CH2_DISTANCE_UNC] = "0";
	row[CS_CH2_THICKNESS]	= "0.164";	// cm
	row[CS_CH2_THICKNESS_UNC] = "0";

	row[CS_C12_AREA]	= "43.20869";	// cm^2
	row[CS_C12_AREA_UNC]	= "0";
	row[CS_C12_DISTANCE]	= "14.52";	// cm
	row[CS_C12_DISTANCE_UNC] = "0";
	row[CS_C12_THICKNESS]	= "0.889";	// cm
	row[CS_C12_THICKNESS_UNC] = "0";
}

void UpdateCalcValues( vector<string> & row, vector<string> const & fg, vector<string> const & bg )
{
	row[CS_FG_PROTONS] 	= fg[RS_PROTONS];
	row[CS_FG_PROTONS_UNC]	= TString::Format( "%f", sqrt( atof( fg[RS_PROTONS].c_str() ) ) );
	row[CS_BG_PROTONS] 	= bg[RS_PROTONS];
	row[CS_BG_PROTONS_UNC]	= TString::Format( "%f", sqrt( atof( bg[RS_PROTONS].c_str() ) ) );
	row[CS_CH2_DECAY]	= fg[RS_CH2_DECAY];
	row[CS_CH2_DECAY_UNC]	= fg[RS_CH2_DECAY_ERR];
	row[CS_C12_DECAY]	= fg[RS_C12_DECAY];
	row[CS_C12_DECAY_UNC]	= fg[RS_C12_DECAY_ERR];
}

void UpdateSummary( vector<string> & row, RunSummary const * const summary )
{
	int fg_run_number = atoi( row[CS_FG_RUN_NUMBER].c_str() );
	int bg_run_number = atoi( row[CS_BG_RUN_NUMBER].c_str() );
	vector<string> fg = summary->GetRun( fg_run_number );
	vector<string> bg = summary->GetRun( bg_run_number );

	assert( row.size() == CS_NUM_COLUMNS );
	assert( fg.size() == RS_NUM_COLUMNS );
	assert( bg.size() == RS_NUM_COLUMNS );

	loadsum::UpdateRunData( row, fg, bg );
	loadsum::UpdateGeometry( row );
	loadsum::UpdateCalcValues( row, fg, bg );
}

} // namespace loadsum

void CrossSection::LoadSummary( RunSummary const * const summary )
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );
		loadsum::UpdateSummary( row, summary );
		SetRow( i, row );
	}
}

} // namespace n2n
