/** @file summary.cxx
 * Copyright (C) 2013 Houghton College
 *
 * Update the Run_Summary.csv file.
 */

#include "csv.H"
#include "decay.hxx"
#include "proton.hxx"
#include <vector>

namespace c12 {

/**
 * Run summary column names
 */
enum RunSummary { 
        RS_RUN_NUMBER, 		///< Run number
	RS_DATE, 		///< Date of run
	RS_START_TIME, 		///< Start time
	RS_STOP_TIME, 		///< Stop time
        RS_NEUTRON_ENERGY,	///< Energy of neutrons (MeV)
       	RS_DEUTERON_ENERGY, 	///< Energy of deuterons (MeV)
	RS_CH2_NUMBER,		///< ID/description of CH2 target
        RS_CH2_DIAMETER,	///< Diameter of CH2 target (inches)
       	RS_CH2_THICKNESS, 	///< Thickness of CH2 target (mm)
	RS_GRAPHITE_NUMBER,	///< ID/description of Graphite target
        RS_CLOCK_TIME, 		///< Clock time (sec)
	RS_LIVE_TIME,		///< Live time (sec)
       	RS_ADC1_LIVE_TIME,	///< ADC1 live time (sec)
        RS_ADC2_LIVE_TIME,	///< ADC2 live time (sec)
       	RS_ADC3_LIVE_TIME, 	///< ADC3 live time (sec)
	RS_ADC4_LIVE_TIME,	///< ADC4 live time (sec)
        RS_ADC5_LIVE_TIME,	///< ADC5 live time (sec)
       	RS_TOTAL_COUNTS,	///< Total counts (a_4 x a_5)
       	RS_GROSS_COUNTS,	///< Gross counts (a_4 x a_5)
        RS_NET_COUNTS,		///< Net counts (a_4 x a_5)
       	RS_DE_DEAD,		///< Dead time of delta-E detector (%)	
       	RS_E_DEAD,		///< Dead time of E detector (%)
       	RS_TOTAL_LIVE,		///< Total live time of proton telescope (%) 
        RS_INTERIM_TIME,	///< Interim/transit time (sec)
       	RS_BEAM_CURRENT, 	///< Beam current (&mu;A)
	RS_TOTAL_CHARGE,	///< Integrated beam current (&mu;C)
        RS_MQ4_A,
       	RS_MQ4_B, 
	RS_TOTAL_NEUTRONS,	///< Total neutrons (counts/BCI)
       	RS_GROSS_NEUTRONS,	///< Gross neutrons (counts/BCI)
        RS_NET_NEUTRONS,	///< Net neutrons (counts/BCI)
	RS_ROI_XMIN,		///< Min X value for region of interest
	RS_ROI_XMAX,		///< Max X value for region of interest
	RS_ROI_YMIN,		///< Min Y value for region of interest
	RS_ROI_YMAX,		///< Max Y value for region of interest
	RS_GROSS_PROTONS,	///< Gross protons detected (counts)
       	RS_NET_PROTONS,		///< Net protons detected (counts)
        RS_C11_PLASTIC_COUNT,	///< Calculated C11 decays in CH2 (counts)
       	RS_C11_PLASTIC_ERROR,	///< Error in CSV_CH2_C11_COUNT
       	RS_C11_PUCK_COUNT,	///< Calculated C11 decays in graphite (counts)
        RS_C11_PUCK_ERROR,	///< Error in CSV_GRAPHITE_C11_COUNT
       	RS_PROTONS_COUNT,	///< Protons detected (counts/&mu;C)
	RS_PROTONS_ERROR,	///< Error in CSV_PROTONS_COUNT
	RS_PROTONS_PER_NEUTRON,	///< Protons detected (counts/neutron)
        RS_DISTANCES,		///< Distance Section Header
       	RS_CH2_DISTANCE,	///< Distance to front of CH2 (cm)
       	RS_GRAPHITE_DISTANCE,	///< Distance to front of graphite (cm)
        RS_TELESCOPE_DISTANCE,	///< Distance to front of telescope (cm)
       	RS_DE_DISTANCE,		///< Distance to center of dE detector (cm)
       	RS_E_DISTANCE,		///< Distance to center of E detector (cm)
        RS_NOTES,		///< Notes field
	RS_NUM_COLUMNS
};

/**
 * Cross Section column names
 */
enum CrossSection {
	CS_FG_RUN_NUMBER,	///< Run number of foreground run
	CS_BG_RUN_NUMBER,	///< Run number of background run
	CS_ENERGY,		///< Neutron energy for both runs (MeV)
	CS_FG_PROTONS,		///< Number of protons in foreground run (#)
	CS_BG_PROTONS,		///< Number of protons in background run (#)
	CS_FG_LIVE_TIME,	///< Live time of foreground run (s)
	CS_BG_LIVE_TIME,	///< Live time of background run (s)
	CS_NUM_COLUMNS
};

/**
 * Types of traget for counting.
 */
enum C11Target {
	C11_TARGET_PUCK,	///< The graphite puck
	C11_TARGET_PLASTIC,	///< The plastic target
	C11_NUM_TARGETS
};

/**
 * Calculate the total number of C11  and update the csv file.
 *
 * @param row The row of the csv file to update
 * @param dirname The directory containing decay curves
 */
void UpdateC11( std::vector<std::string>& row, char const * dirname )
{
	std::string run_number = row[RS_RUN_NUMBER];
	for ( int i = 0; i < 4 - run_number.length(); ++i )
		run_number = "0" + run_number;

	char const * puck_filename = 
		gSystem->ConcatFileName( dirname,
				("Run" + run_number + "_puck.csv").c_str() );
	char const * plast_filename = 
		gSystem->ConcatFileName( dirname,
				("Run" + run_number + "_plastic.csv").c_str() );

	std::cout << "\n * Calculating decay curve...\n";

	Double_t trans_time = atoi( row[RS_INTERIM_TIME].c_str() ) / 60.0;
	Double_t efficiency = 0.12;
	Double_t puck_count = 0, puck_error = 0;
	Double_t plast_count = 0, plast_error = 0;

	// NOTE: TSystem::AccessPathName returns *FALSE* if the file
	// *CAN* be accessed! 
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	std::cout << "\t - " << puck_filename << "\n";
	if ( !gSystem->AccessPathName( puck_filename ) )
	{
		TGraphErrors * ge = decay::CSVGetData( puck_filename );
		TFitResultPtr fr = decay::Fit( ge );
		puck_count = decay::GetCount( fr, trans_time, efficiency );
		puck_error = decay::GetError( fr, trans_time, efficiency );
	}

	std::cout << "\t - " << plast_filename << "\n";
	if ( !gSystem->AccessPathName( plast_filename ) )
	{
		TGraphErrors * ge = decay::CSVGetData( plast_filename );
		TFitResultPtr fr = decay::Fit( ge );
		plast_count = decay::GetCount( fr, trans_time, efficiency );
		plast_error = decay::GetError( fr, trans_time, efficiency );
	}

	row[RS_C11_PUCK_COUNT] = TString::Format( "%f", puck_count );
	row[RS_C11_PUCK_ERROR] = TString::Format( "%f", puck_error );
	row[RS_C11_PLASTIC_COUNT] = TString::Format( "%f", plast_count );
	row[RS_C11_PLASTIC_ERROR] = TString::Format( "%f", plast_error );
}

/**
 * Calculate the total number of protons and update the csv file.
 * 
 * @param row The row of the csv file to update
 * @param dirname The directory containing proton telescope data
 */
void UpdateProtons( std::vector<std::string>& row, char const * dirname )
{
	std::string run_number = row[RS_RUN_NUMBER];
	for ( int i = 0; i < 4 - run_number.length(); ++i )
		run_number = "0" + run_number;

	char const * csv_filename =
		gSystem->ConcatFileName( dirname,
			       ("Run" + run_number + "_1x2.csv").c_str() );
	char const * mpa_filename =
		gSystem->ConcatFileName( dirname, 
				("Run" + run_number + ".mpa").c_str() );

	std::cout << "\n * Calculating proton count...\n";

	int roi_xmin = 0, roi_ymin = 0;
	int roi_xmax = 0, roi_ymax = 0;
        int gross_p = 0;

	// NOTE: TSystem::AccessPathName returns *FALSE* if the file
	// *CAN* be accessed! 
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	std::cout << "\t - " << csv_filename << "\n"
		  << "\t - " << mpa_filename << "\n";
	if ( !gSystem->AccessPathName( csv_filename )
			&& !gSystem->AccessPathName( mpa_filename ) )
	{
		TNtuple * data = proton::CSVGetData( csv_filename );
		Region roi = proton::MPAGetROI( mpa_filename );

		roi_xmin = roi.xmin;
		roi_xmax = roi.xmax;
		roi_ymin = roi.ymin;
		roi_ymax = roi.ymax;
		gross_p = data->GetEntries( roi.AsTCut() );
	}

	row[RS_ROI_XMIN] = TString::Format( "%d", roi_xmin );
	row[RS_ROI_YMIN] = TString::Format( "%d", roi_ymin );
	row[RS_ROI_XMAX] = TString::Format( "%d", roi_xmax );
	row[RS_ROI_YMAX] = TString::Format( "%d", roi_ymax );
	row[RS_GROSS_PROTONS] = TString::Format( "%d", gross_p );
}

/**
 * Update the Run_Summary.csv file.
 *
 * @param dirname The directory in which to look for the run data.
 */
void UpdateRunSummary( char const * dirname )
{
	char const * filename = 
		gSystem->ConcatFileName( dirname, "Run_Summary.csv" );
	char const * tempname = 
		gSystem->ConcatFileName( gSystem->TempDirectory(),
			       	"Run_Summary.csv" );
	std::cout << "\n * Updating file \"" << filename << "\"...\n\n";

	std::ifstream ifs( filename, std::ios::in );
	std::ofstream ofs( tempname, std::ios::out );

	// Skip over lines 1-3 in file
	std::string line;
	for ( int i = 1; i < 4; ++i )
	{
		std::getline( ifs, line );
		ofs << line << std::endl;
	}

	// Process the rest of the file
	while ( std::getline( ifs, line ) )
	{
		std::vector<std::string> csv_row = CSVParseRow( line );
		UpdateC11( csv_row, gSystem->ConcatFileName( dirname, "Decay Curves" ) );
		UpdateProtons( csv_row, gSystem->ConcatFileName( dirname, "Proton Telescope" ) );
		ofs << CSVFormatRow( csv_row ) << std::endl;
	}

	ifs.close();
	ofs.close();

	gSystem->CopyFile( tempname, filename, true );
	gSystem->Unlink( tempname );
}

/**
 * Update Cross_Sections.csv file.
 *
 * @param dirname The directory in which to look for run data.
 */
void UpdateCrossSections( char const * dirname )
{
	char const * filename =
		gSystem->ConcatFileName( dirname, "Cross_Sections.csv" );
	char const * tempname =
		gSystem->ConcatFileName( gSystem->TempDirectory(), 
				"Cross_Sections.csv" );
	char const * rs_filename =
		gSystem->ConcatFileName( dirname, "Run_Summary.csv" );

	std::cout << filename << std::endl;

	std::ifstream ifs( filename, std::ios::in );
	std::ofstream ofs( tempname, std::ios::out );
	std::ifstream rs_ifs( rs_filename, std::ios::in );

	// Skip over header
	std::string line;
	for ( int i = 1; i < 4; ++i )
	{
		std::getline( ifs, line );
		ofs << line << std::endl;
	}

	// Process rest of file
	while ( std::getline( ifs, line ) )
	{
		std::vector<std::string> row = CSVParseRow( line );
		int run_fg = atoi( row[CS_FG_RUN_NUMBER].c_str() );
		int run_bg = atoi( row[CS_BG_RUN_NUMBER].c_str() );
		std::cout << run_fg << " " << run_bg << std::endl;

		// Get the rows for the foreground and background from the run summary
		std::string energy, fg_protons, bg_protons, fg_live, bg_live;

		rs_ifs.seekg( 0, rs_ifs.beg );
		std::string rs_line;
		for ( int i = 1; i < 4; ++i )
		{
			std::getline( rs_ifs, rs_line );
		}

		while ( std::getline( rs_ifs, rs_line ) )
		{
			std::vector<std::string> rs_row = CSVParseRow( rs_line );
			int run = atoi( rs_row[RS_RUN_NUMBER].c_str() );

			if ( run == run_fg )
			{
				energy = rs_row[RS_NEUTRON_ENERGY];
				fg_protons = rs_row[RS_GROSS_PROTONS];
				fg_live = rs_row[RS_LIVE_TIME];
			}

			if ( run == run_bg )
			{
				bg_protons = rs_row[RS_GROSS_PROTONS];
				bg_live = rs_row[RS_LIVE_TIME];
			}
		}

		row[CS_ENERGY] = energy;
		row[CS_FG_PROTONS] = fg_protons;
		row[CS_BG_PROTONS] = bg_protons;
		row[CS_FG_LIVE_TIME] = fg_live;
		row[CS_BG_LIVE_TIME] = bg_live;

		std::cout << CSVFormatRow( row ) << std::endl;
	}
}


} //namespace c12
