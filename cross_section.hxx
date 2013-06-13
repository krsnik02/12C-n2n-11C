/** @file cross_section.hxx
 * Copyright (C) 2013 Houghton College
 */

#include "csv_file.hxx"
#include "run_summary.hxx"

namespace cs {

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


struct CrossSection : public csv::CSVFile
{
	public:
		/**
		 * Calculate the cross section for each row in the file
		 * @param summary The run summary.
		 */
		void Update( rs::RunSummary const * const summary );
};

} // namespace cs
