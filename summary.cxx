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
 * Cross Section column names
 */
enum CrossSection {
	CS_FG_RUN_NUMBER,	///< Run number of foreground run
	CS_BG_RUN_NUMBER,	///< Run number of background run
	CS_FG_PROTONS,		///< Number of protons in foreground run (protons)
	CS_BG_PROTONS,		///< Number of protons in background run (protons)
	CS_FG_LIVE_TIME,	///< Live time of foreground run (s)
	CS_BG_LIVE_TIME,	///< Live time of background run (s)	
	CS_PROTONS,		///< Proton flux (protons/s)
	CS_PROTONS_ERR,		///< Uncertainty in proton flux (protons/s)
	CS_NEUTRON_ENERGY,	///< Neutron energy for both runs (MeV)
	CS_NP_CROSS_SECTION,	///< Cross section of np reaction (mbarn)
	CS_CH2_THICKNESS,	///< Thickness of CH2 target (cm)
	CS_CH2_NUM_H,		///< Number thickness of H in CH2 target (protons/cm^2)
	CS_DET_AREA,		///< Area of detector (cm^2)
	CS_DET_DISTANCE,	///< Distance of detector (cm)
	CS_DET_SOLID_ANGLE,	///< Solid angle of detector (sr)
	CS_CH2_AREA,		///< Area of CH2 target (cm^2)
	CS_CH2_DISTANCE,	///< Distance of CH2 target (cm)
	CS_CH2_SOLID_ANGLE,	///< Solid angle of CH2 target (sr)
	CS_NEUTRON_FLUX,	///< Measured neutron flux (neutrons/s)
	CS_NEUTRON_FLUX_ERR,	///< Error in neutron flux (neutrons/s)
	CS_NUM_COLUMNS
};

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

void CalculateCrossSection( std::vector<std::string> row )
{
	int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );			// protons
	int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );			// protons
	double fg_live = atof( row[CS_FG_LIVE_TIME].c_str() );			// s
	double bg_live = atof( row[CS_BG_LIVE_TIME].c_str() );			// s

	double protons = (fg_protons / fg_live) - (bg_protons / bg_live);	// protons/s
	double protons_err = sqrt( pow( sqrt( fg_protons ) / fg_live, 2 ) +
			pow( sqrt( bg_protons ) / bg_live, 2 ) );		// protons/s

	row[CS_PROTONS] = TString::Format( "%f", protons );
	row[CS_PROTONS_ERR] = TString::Format( "%f", protons_err );

	double energy = atof( row[CS_NEUTRON_ENERGY].c_str() );			// MeV
	double thick_CH2 = 0.164; 						// cm
	double area_E = 0.7133;							// cm^2
	double dist_E = 12;							// cm
	double area_CH2 = 5.06707479;						// cm^2
	double dist_CH2 = 6.5;							// cm

	double cross_section_np = GetCrossSection_np( energy );			// mbarn
	double num_thick_H_CH2 = GetNumberThickness_H_CH2( thick_CH2 );		// protons/cm^2
	double solid_angle_E = GetSolidAngle( area_E, dist_E );			// sr
	double solid_angle_CH2 = GetSolidAngle( area_CH2, dist_CH2 );		// sr

	// 1 mb = 1e-27 cm^2
	double nflux = protons / (cross_section_np * 1e-27
			* num_thick_H_CH2 * solid_angle_E * solid_angle_CH2 );	// neutrons/s
	double nflux_err = protons_err / (cross_section_np * 1e-27
			* num_thick_H_CH2 * solid_angle_E * solid_angle_CH2 );	// neutrons/s

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

		CalculateCrossSection( row );

		ofs << CSVFormatRow( row ) << std::endl;
	}
	
	ifs.close();
	ofs.close();

	gSystem->CopyFile( tempname, filename, true );
	gSystem->Unlink( tempname );
}



} //namespace c12
