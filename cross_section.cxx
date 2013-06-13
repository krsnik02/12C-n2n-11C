/** @file cross_section.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "cross_section.hxx"

namespace cs {

void UpdateSummary( vector<string> & row, rs::RunSummary const * const summary )
{
	int fg_run_number = atoi( row[CS_FG_RUN_NUMBER].c_str() );
	int bg_run_number = atoi( row[CS_BG_RUN_NUMBER].c_str() );
	vector<string> fg = summary->GetRun( fg_run_number );
	vector<string> bg = summary->GetRun( bg_run_number );

	assert( row.size() == CS_NUM_COLUMNS );
	assert( fg.size() == rs::RS_NUM_COLUMNS );
	assert( bg.size() == rs::RS_NUM_COLUMNS );

	row[CS_FG_PROTONS] 	= fg[rs::RS_GROSS_PROTONS];
	row[CS_BG_PROTONS] 	= bg[rs::RS_GROSS_PROTONS];
	row[CS_FG_LIVE_TIME] 	= fg[rs::RS_LIVE_TIME];
	row[CS_BG_LIVE_TIME] 	= bg[rs::RS_LIVE_TIME];
	row[CS_NEUTRON_ENERGY] 	= fg[rs::RS_NEUTRON_ENERGY];
}

void CrossSection::Update( rs::RunSummary const * const summary )
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );
		cs::UpdateSummary( row, summary );
		SetRow( i, row );
	}
}

} // namespace cs
