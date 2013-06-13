/** @file cross_section.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "cross_section.hxx"

namespace cs {

void UpdateSummary( vector<string> & row, rs::RunSummary const * const summary )
{
	int fg_run_number = atoi( row[CS_FG_RUN_NUMBER].c_str() );
	int bg_run_number = atoi( row[CS_BG_RUN_NUMBER].c_str() );
	std::cout << "REALLY " << fg_run_number << endl;
	vector<string> fg = summary->GetRun( fg_run_number );

	row[CS_NEUTRON_ENERGY] 	= fg[rs::RS_NEUTRON_ENERGY];
	row[CS_BEAM_CHARGE] 	= fg[rs::RS_TOTAL_CHARGE];
	row[CS_FG_PROTONS] 	= fg[rs::RS_GROSS_PROTONS];
	row[CS_FG_LIVE_TIME] 	= fg[rs::RS_LIVE_TIME];
	row[CS_ELAPSED_TIME] 	= fg[rs::RS_CLOCK_TIME];

	row[CS_C11_PLASTIC] 	= fg[rs::RS_C11_PLASTIC];
	row[CS_C11_PLASTIC_ERR] = fg[rs::RS_C11_PLASTIC_ERR];
	row[CS_C11_PUCK]	= fg[rs::RS_C11_PUCK];
	row[CS_C11_PUCK_ERR] 	= fg[rs::RS_C11_PUCK_ERR];

	std::cout << "YEAH " << bg_run_number << endl;
	vector<string> bg = summary->GetRun( bg_run_number );
	row[CS_BG_PROTONS] 	= bg[rs::RS_GROSS_PROTONS];
	row[CS_BG_LIVE_TIME] 	= bg[rs::RS_LIVE_TIME];

	std::cout << "OKAY THEN" << endl;
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
