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
        RS_C11_PLASTIC,		///< Calculated C11 decays in CH2 (counts)
       	RS_C11_PLASTIC_ERR,	///< Error in RS_C11_PLASTIC
       	RS_C11_PUCK,		///< Calculated C11 decays in graphite (counts)
        RS_C11_PUCK_ERR,	///< Error in RS_C11_PUCK
       	RS_PROTONS_COUNT,	///< Protons detected (counts/&mu;C)
	RS_PROTONS_ERROR,	///< Error in RS_PROTONS
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
	CS_NEUTRON_ENERGY,	///< Neutron energy for both runs (MeV)
	CS_BEAM_CHARGE,		///< Total beam charge (&mu;C) 
	CS_ELAPSED_TIME,	///< Total run time (s)
	CS_FG_LIVE_TIME,	///< Live time of foreground run (s)
	CS_BG_LIVE_TIME,	///< Live time of background run (s)
	CS_C11_PLASTIC,		///< Calculated decays in CH2 (counts)
	CS_C11_PLASTIC_ERR,
	CS_C11_PUCK,		///< Calculated decays in graphite (counts)
	CS_C11_PUCK_ERR,	
	CS_FG_PROTONS,		///< Number of protons in foreground run (#)
	CS_BG_PROTONS,		///< Number of protons in background run (#)
	CS_PROTONS,		///< Net protons (background subtracted) (#/s)
	CS_PROTONS_ERR,
	CS_NEUTRON_FLUX,	///< Measured neutron flux (1/s sr)
	CS_NEUTRON_FLUX_ERR,
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


void UpdateFG( std::vector<std::string> & row, char const * filename )
{
	std::string fg_run = row[CS_FG_RUN_NUMBER];

	std::ifstream ifs( filename, std::ios::in );

	// Skip over header
	std::string line;
	for ( int i = 1; i < 4; ++i )
	{
		std::getline( ifs, line );
	}

	std::vector<std::string> fg;
	while ( std::getline( ifs, line ) )
	{
		fg = CSVParseRow( line );
		if ( fg_run == fg[RS_RUN_NUMBER] )
			break;
	}

	row[CS_NEUTRON_ENERGY] = fg[RS_NEUTRON_ENERGY];
	row[CS_BEAM_CHARGE] = fg[RS_TOTAL_CHARGE];
	row[CS_FG_PROTONS] = fg[RS_GROSS_PROTONS];
	row[CS_FG_LIVE_TIME] = fg[RS_LIVE_TIME];
	row[CS_ELAPSED_TIME] = fg[RS_CLOCK_TIME];

	row[CS_C11_PLASTIC] = fg[RS_C11_PLASTIC];
	row[CS_C11_PLASTIC_ERR] = fg[RS_C11_PLASTIC_ERR];
	row[CS_C11_PUCK] = fg[RS_C11_PUCK];
	row[CS_C11_PUCK_ERR] = fg[RS_C11_PUCK_ERR];
}

void UpdateBG( std::vector<std::string> & row, char const * filename )
{
	std::string bg_run = row[CS_BG_RUN_NUMBER];

	std::ifstream ifs( filename, std::ios::in );

	// Skip over header
	std::string line;
	for ( int i = 1; i < 4; ++i )
	{
		std::getline( ifs, line );
	}

	std::vector<std::string> bg;
	while ( std::getline( ifs, line ) )
	{
		bg = CSVParseRow( line );
		if ( bg_run == bg[RS_RUN_NUMBER] )
			break;
	}

	row[CS_BG_PROTONS] = bg[RS_GROSS_PROTONS];
	row[CS_BG_LIVE_TIME] = bg[RS_LIVE_TIME];
}





/**
 * Determine the cross section of a (n,p) reaction based on a 
 * cspline interpolation between known cross sections.
 *
 * @param energy the kinetic energy of the reaction (MeV)
 *
 * @return the cross section at that energy (mb/sr)
 */
double GetCrossSection_np( double energy )
{
	double energies[] = { 20, 22, 24, 26, 28 }; 	// (MeV)
	double x_sects[] = { 153, 139, 128, 119, 111 }; // (mb/sr)

	ROOT::Math::Interpolator interp( 5 );
	interp.SetData( 5, energies, x_sects );
	return interp.Eval( energy );
}

/** 
 * Get the number of hydrogen thickness in a specified thickness of CH2.
 * @f$N_{H,CH_2}(\mathrm{thickness})=\frac{2\rho_{CH2}}{m_{CH2}}\cdot\mathrm{thickness}@f$
 *
 * @param thickness the thickness of the CH2 (cm)
 *
 * @return the number thickness of hydrogen in that amount of CH2 (protons/cm^2)
 */
double GetNumberThickness_H_CH2( double thickness )
{
	double mass_H = 1.007825; 		// u
	double mass_C = 12;			// u
	double mass_CH2 = 2 * mass_H + mass_C;	// u
	double density_CH2 = 0.89; 		// g/cm^3
	
	// 1 u = 1.660538782e-24 g
	return 2 * thickness * density_CH2 / (1.660538782e-24 * mass_CH2);	// protons/cm^2
}

/** 
 * Get the number thickness of carbon in a specified thickness of CH2.
 * @f$N_{C,CH_2}(\mathrm{thickness})=\frac{\rho_{CH2}}{m_{CH2}}\cdot\mathrm{thickness}@f$
 *
 * @param thickness the thickness of the CH2 (cm)
 *
 * @return the number thickness of carbon in that amount of CH2 (protons/cm^2)
 */
double GetNumberThickness_C_CH2( double thickness )
{
	double mass_H = 1.007825;		// u
	double mass_C = 12;			// u
	double mass_CH2 = 2 * mass_H + mass_C;	// u
	double density_CH2 = 0.89;		// g/cm^3

	// 1 u = 1.660538782e-24 g
	return thickness * density_CH2 / (1.660538782e-24 * mass_CH2);		// nuclei/cm^2
}

/**
 * Get the solid angle of an object.
 * @f$\Omega(\mathrm{area},\mathrm{distance})=\frac{\mathrm{area}}{\mathrm{distance}^2}@f$
 *
 * @param area the cross sectional area of the target (cm^2)
 * @param distance the distance of the target (cm)
 *
 * @return the solid angle of the target (sr)
 */
double GetSolidAngle( double area, double distance )
{
	return area / pow( distance, 2 );
}

void UpdateNeutronFlux( std::vector<std::string> row )
{
	int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );			// protons
	int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );			// protons
	double fg_live = atof( row[CS_FG_LIVE_TIME].c_str() );			// s
	double bg_live = atof( row[CS_BG_LIVE_TIME].c_str() );			// s

	double protons = (fg_protons / fg_live) - (bg_protons / bg_live);	// protons/s
	double protons_err = sqrt( pow( sqrt( fg_protons ) / fg_live, 2 ) +
			pow( sqrt( bg_protons ) / bg_live, 2 ) );		// protons/s
	
	double energy = atof( row[CS_NEUTRON_ENERGY].c_str() );			// MeV
	double thick_CH2 = 0.164; 						// cm
	double area_E = 0.7133;							// cm^2
	double dist_E = 12;							// cm
	double area_CH2 = 5.06707479;						// cm^2
	double dist_CH2 = 6.5;							// cm

	double cross_section_np = GetCrossSection_np( energy );			// mb/sr
	double num_thick_H_CH2 = GetNumberThickness_H_CH2( thick_CH2 );		// protons/cm^2
	double solid_angle_E = GetSolidAngle( area_E, dist_E );			// sr
	double solid_angle_CH2 = GetSolidAngle( area_CH2, dist_CH2 );		// sr

	// 1 mb = 1e-27 cm^2
	double nflux = protons / (cross_section_np * 1e-27
			* num_thick_H_CH2 * solid_angle_E * solid_angle_CH2 );	// protons/s*sr
	double nflux_err = protons_err / (cross_section_np * 1e-27
			* num_thick_H_CH2 * solid_angle_E * solid_angle_CH2 );	// protons/s*sr

	row[CS_PROTONS] = TString::Format( "%f", protons );
	row[CS_PROTONS_ERR] = TString::Format( "%f", protons_err );
	row[CS_NEUTRON_FLUX] = TString::Format( "%f", nflux );
	row[CS_NEUTRON_FLUX_ERR] = TString::Format( "%f", nflux_err );	
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

	std::ifstream ifs( filename, std::ios::in );
	std::ofstream ofs( tempname, std::ios::out );

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
		UpdateFG( row, gSystem->ConcatFileName( dirname, "Run_Summary.csv" ) );
		UpdateBG( row, gSystem->ConcatFileName( dirname, "Run_Summary.csv" ) );

		UpdateNeutronFlux( row );
		UpdateCrossSection_CH2( row );

		ofs << CSVFormatRow( row ) << std::endl;
	}
	
	ifs.close();
	ofs.close();

	gSystem->CopyFile( tempname, filename, true );
	gSystem->Unlink( tempname );
}



} //namespace c12
