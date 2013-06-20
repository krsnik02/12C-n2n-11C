/** 
 * @file n2n/CrossSection.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_CROSSSECTION_INCL_
#define N2N_CROSSSECTION_INCL_

#include "CSVFile.hxx"
#include "RunSummary.hxx"

namespace n2n {

/**
 * Cross Section column names
 */
enum CSFields {
	CS_FG_RUN_NUMBER,	///< Run number of foreground run
	CS_BG_RUN_NUMBER,	///< Run number of background run
	CS_SPACE0,
	CS_NEUTRON_ENERGY,	///< Neutron energy (MeV)
	CS_NEUTRON_ENERGY_UNC,	///< Uncertainty in CS_NEUTRON_ENERGY
	CS_FG_CLOCK_TIME,	///< Total clock time of foreground (s)
	CS_FG_CLOCK_TIME_UNC,	///< Uncertainty in CS_FG_CLOCK_TIME
	CS_FG_LIVE_FRAC,	///< Live time of foreground run (fraction live)
	CS_FG_LIVE_FRAC_UNC,	///< Uncertainty in CS_FG_LIVE_FRAC
	CS_BG_CLOCK_TIME,	///< Total clock time of background (s)
	CS_BG_CLOCK_TIME_UNC,	///< Uncertainty in CS_BG_CLOCK_TIME
	CS_BG_LIVE_FRAC,	///< Live time of background run (fraction live)
	CS_BG_LIVE_FRAC_UNC,	///< Uncertainty in CS_BG_LIVE_FRAC
	CS_SPACE1,
	CS_DET_AREA,		///< Area of detector (cm^2)
	CS_DET_AREA_UNC,	///< Uncertainty in CS_DET_AREA
	CS_DET_DISTANCE,	///< Distance of detector (cm)
	CS_DET_DISTANCE_UNC,	///< Uncertainty in CS_DET_DISTANCE
	CS_SPACE2,
	CS_CH2_AREA,		///< Area of CH2 target (cm^2)
	CS_CH2_AREA_UNC,	///< Uncertainty in CS_CH2_AREA
	CS_CH2_DISTANCE,	///< Distance of CH2 target (cm)
	CS_CH2_DISTANCE_UNC,	///< Uncertainty in CS_CH2_DISTANCE
	CS_CH2_THICKNESS,	///< Thickness of CH2 target (cm)
	CS_CH2_THICKNESS_UNC,	///< Uncertainty in CS_CH2_THICKNESS
	CS_SPACE3,
	CS_C12_AREA,		///< Area of C12 target (cm^2)
	CS_C12_AREA_UNC,	///< Uncertainty in CS_C12_AREA
	CS_C12_DISTANCE,	///< Distance of C12 target (cm^2)
	CS_C12_DISTANCE_UNC,	///< Uncertainty in CS_C12_DISTANCE
	CS_C12_THICKNESS,	///< Thickness of C12 target (cm)
	CS_C12_THICKNESS_UNC,	///< Uncertainty in CS_C12_THICKNESS
	CS_SPACE4,
	CS_FG_PROTONS,		///< Number of protons in foreground run (protons)
	CS_FG_PROTONS_UNC,	///< Uncertainty in CS_FG_PROTONS
	CS_BG_PROTONS,		///< Number of protons in background run (protons)
	CS_BG_PROTONS_UNC,	///< Uncertainty in CS_BG_PROTONS
	CS_C12_DECAY,		///< Number of activated C11 nuclei in C12 target (C11 nuclei)
	CS_C12_DECAY_UNC,	///< Uncertainty in CS_C12_DECAYS
	CS_CH2_DECAY,		///< Number of activated C11 nuclei in CH2 target (C11 nuclei)
	CS_CH2_DECAY_UNC,	///< Uncertainty in CS_CH2_DECAYS
	CS_SPACE5,
	CS_PROTON_FLUX,		///< Proton flux (protons/s)
	CS_PROTON_FLUX_UNC,	///< Uncertainty in CS_PROTON_FLUX
	CS_NEUTRON_FLUX,	///< Neutron flux (neutrons/s)
	CS_NEUTRON_FLUX_UNC,	///< Uncertainty in CS_NEUTRON_FLUX
	CS_CH2_XSECT,		///< (n,2n) cross section in CH2 (mbarn)
	CS_CH2_XSECT_UNC,	///< Uncertainty in CS_CH2_XSECT
	CS_C12_XSECT,		///< (n,2n) cross section in C12 (mbarn)
	CS_C12_XSECT_UNC,	///< Uncertainty in CS_C12_XSECT
	CS_NUM_COLUMNS
};


struct CrossSection : public CSVFile
{
	public:
		/**
		 * Copy values from Run_Summary.csv file into the Cross_Sections.csv file
		 * @param summary The run summary to use
		 */
		void LoadSummary( RunSummary const * const summary );

		/**
		 * Calculate cross sections based on the values in Cross_Sections.csv
		 */
		void Calculate();
};

} // namespace n2n

#endif
