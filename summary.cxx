/** @file summary.cxx
 * Copyright (C) 2013 Houghton College
 *
 * Update the Run_Summary.csv file.
 */

#include "decay.H"
#include "csv.H"
#include "proton.hxx"
#include <vector>

namespace c12 {

/** 
 * Traverse the filesystem to locate the specified run data.
 *
 * @param dir A TSystemDirectory object in which to search. This must be a flat
 * directory as this method cannot properly traverse a directory tree.
 * @param run_number The run number to search for.
 * @param target The type of target to search for.
 *
 * @return The TSystemFile object which was found, or the default TSystemFile.
 */
TSystemFile * LocateFile( 
		TSystemDirectory * dir,
		char const * run_number,
		char const * target )
{
	// ensure run number exists and is zero padded
	std::string run = run_number;
	if ( run == "" ) 
		return NULL;
	for ( int n = 0; n < 4 - run.length(); ++n )
		run = "0" + run;

	// find the corresponding file
	TList * files = dir->GetListOfFiles();
	for ( TIter & i = TIter( files ).Begin(); i != TIter::End(); ++i )
	{
		TString * name = new TString( (*i)->GetName() );
		if ( name->Contains( TPRegexp( "\.[cC][sS][vV]$" ) ) 
			&& name->Contains( run.c_str() )
			&& name->Contains( target, TString::kIgnoreCase ) )
			return (TSystemFile *)*i;
	}
	return NULL;
}

/**
 * Run summary column names
 */
enum CsvField { 
        CSV_RUN_NUMBER = 0, 	///< Run number
	CSV_DATE, 		///< Date of run
	CSV_START_TIME, 	///< Start time
	CSV_STOP_TIME, 		///< Stop time
        CSV_NEUTRON_ENERGY,	///< Energy of neutrons (MeV)
       	CSV_DEUTERON_ENERGY, 	///< Energy of deuterons (MeV)
	CSV_CH2_NUMBER,		///< ID/description of CH2 target
        CSV_CH2_DIAMETER,	///< Diameter of CH2 target (inches)
       	CSV_CH2_THICKNESS, 	///< Thickness of CH2 target (mm)
	CSV_GRAPHITE_NUMBER,	///< ID/description of Graphite target
        CSV_CLOCK_TIME, 	///< Clock time (sec)
	CSV_LIVE_TIME,		///< Live time (sec)
       	CSV_ADC1_LIVE_TIME,	///< ADC1 live time (sec)
        CSV_ADC2_LIVE_TIME,	///< ADC2 live time (sec)
       	CSV_ADC3_LIVE_TIME, 	///< ADC3 live time (sec)
	CSV_ADC4_LIVE_TIME,	///< ADC4 live time (sec)
        CSV_ADC5_LIVE_TIME,	///< ADC5 live time (sec)
       	CSV_TOTAL_COUNTS,	///< Total counts (a_4 x a_5)
       	CSV_GROSS_COUNTS,	///< Gross counts (a_4 x a_5)
        CSV_NET_COUNTS,		///< Net counts (a_4 x a_5)
       	CSV_DE_DEAD,		///< Dead time of delta-E detector (%)	
       	CSV_E_DEAD,		///< Dead time of E detector (%)
       	CSV_TOTAL_LIVE,		///< Total live time of proton telescope (%) 
        CSV_INTERIM_TIME,	///< Interim/transit time (sec)
       	CSV_BEAM_CURRENT, 	///< Beam current (&mu;A)
	CSV_TOTAL_CHARGE,	///< Integrated beam current (&mu;C)
        CSV_MQ4_A,
       	CSV_MQ4_B, 
	CSV_TOTAL_NEUTRONS,	///< Total neutrons (counts/BCI)
       	CSV_GROSS_NEUTRONS,	///< Gross neutrons (counts/BCI)
        CSV_NET_NEUTRONS,	///< Net neutrons (counts/BCI)
	CSV_ROI_XMIN,		///< Min X value for region of interest
	CSV_ROI_XMAX,		///< Max X value for region of interest
	CSV_ROI_YMIN,		///< Min Y value for region of interest
	CSV_ROI_YMAX,		///< Max Y value for region of interest
	CSV_GROSS_PROTONS,	///< Gross protons detected (counts)
       	CSV_NET_PROTONS,	///< Net protons detected (counts)
        CSV_CH2_C11_COUNT,	///< Calculated C11 decays in CH2 (counts)
       	CSV_CH2_C11_ERROR,	///< Error in CSV_CH2_C11_COUNT
       	CSV_GRAPHITE_C11_COUNT,	///< Calculated C11 decays in graphite (counts)
        CSV_GRAPHITE_C11_ERROR,	///< Error in CSV_GRAPHITE_C11_COUNT
       	CSV_PROTONS_COUNT,	///< Protons detected (counts/&mu;C)
	CSV_PROTONS_ERROR,	///< Error in CSV_PROTONS_COUNT
	CSV_PROTONS_PER_NEUTRON,///< Protons detected (counts/neutron)
        CSV_DISTANCES,		///< Distance Section Header
       	CSV_CH2_DISTANCE,	///< Distance to front of CH2 (cm)
       	CSV_GRAPHITE_DISTANCE,	///< Distance to front of graphite (cm)
        CSV_TELESCOPE_DISTANCE,	///< Distance to front of telescope (cm)
       	CSV_DE_DISTANCE,	///< Distance to center of dE detector (cm)
       	CSV_E_DISTANCE,		///< Distance to center of E detector (cm)
        CSV_NOTES,		///< Notes field
	CSV_NUM_FIELDS 
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
 * @param target The target to update
 */
void UpdateC11( std::vector<std::string>& row, char const * dirname, 
		C11Target target )
{
	TSystemDirectory * dir = new TSystemDirectory();
	dir->SetDirectory( dirname );

	std::string run_number = row[CSV_RUN_NUMBER];
	Double_t trans_time = atoi( row[CSV_INTERIM_TIME].c_str() ) / 60.0;
	Double_t efficiency = 0.12;
	Double_t count = 0, error = 0;

	TSystemFile * file;
        if ( target == C11_TARGET_PUCK )
		file = LocateFile( dir, run_number.c_str(), "puck" );
	else if ( target == C11_TARGET_PLASTIC )
		file = LocateFile( dir, run_number.c_str(), "plastic" );

	if ( file )
	{
		std::cout << "\n * Calculating decay curve from \""
			  << gSystem->ConcatFileName( 
					  file->GetTitle(), file->GetName() )
			  << "\"...\n";

		TGraphErrors * ge = CreateDecayCurve( file );
		TFitResultPtr fr = FitDecayCurve( ge );
		count = GetCountDecay( fr, trans_time, efficiency );
		error = GetErrorDecay( fr, trans_time, efficiency );
	}

	if ( target == C11_TARGET_PUCK )
	{
		row[CSV_GRAPHITE_C11_COUNT] = TString::Format( "%f", count );
		row[CSV_GRAPHITE_C11_ERROR] = TString::Format( "%f", error );
	}
	else if ( target == C11_TARGET_PLASTIC )
	{
		row[CSV_CH2_C11_COUNT] = TString::Format( "%f", count );
		row[CSV_CH2_C11_ERROR] = TString::Format( "%f", error );
	}
}

/**
 * Calculate the total number of protons and update the csv file.
 * 
 * @param row The row of the csv file to update
 * @param dirname The directory containing proton telescope data
 */
void UpdateProtons( std::vector<std::string>& row, char const * dirname )
{
	std::string run_number = row[CSV_RUN_NUMBER];
	for ( int i = 0; i < 4 - run_number.length(); ++i )
		run_number = "0" + run_number;

	char const * csv_filename =
		gSystem->ConcatFileName( dirname,
			       ("Run" + run_number + "_1x2.csv").c_str() );
	char const * mpa_filename =
		gSystem->ConcatFileName( dirname, 
				("Run" + run_number + ".mpa").c_str() );

	// NOTE: TSystem::AccessPathName returns *FALSE* if the file
	// *CAN* be accessed! 
	// http://root.cern.ch/root/html/TSystem.html#TSystem:AccessPathName
	if ( gSystem->AccessPathName( csv_filename )
			|| gSystem->AccessPathName( mpa_filename ) )
		return;

	TNtuple * data = proton::CSVGetData( csv_filename );
	Region roi = proton::MPAGetROI( mpa_filename );

	row[CSV_ROI_XMIN] = TString::Format( "%d", roi.xmin );
	row[CSV_ROI_YMIN] = TString::Format( "%d", roi.ymin );
	row[CSV_ROI_XMAX] = TString::Format( "%d", roi.xmax );
	row[CSV_ROI_YMAX] = TString::Format( "%d", roi.ymax );
	row[CSV_GROSS_PROTONS] = 
		TString::Format( "%d", data->GetEntries( roi.AsTCut() ) );
}

/**
 * Update the Run_Summary.csv file.
 *
 * @param dirname The directory in which to look for the run data.
 */
void UpdateRunSummary( char const * dirname )
{
	TSystemDirectory * dir = new TSystemDirectory();
	dir->SetDirectory( dirname );

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
		UpdateC11( csv_row, gSystem->ConcatFileName( 
					dirname, "Decay Curves" ), 
				C11_TARGET_PUCK );
		UpdateC11( csv_row, gSystem->ConcatFileName( 
					dirname, "Decay Curves" ), 
				C11_TARGET_PLASTIC );
		UpdateProtons( csv_row, gSystem->ConcatFileName( 
					dirname, "Proton Telescope" ) );
		ofs << CSVFormatRow( csv_row ) << std::endl;
	}

	ifs.close();
	ofs.close();

	gSystem->CopyFile( tempname, filename, kTRUE );
	gSystem->Unlink( tempname );
}


} //namespace c12
