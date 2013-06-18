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
	CS_NEUTRON_ENERGY,	///< Neutron energy (MeV)
	CS_CLOCK_TIME,		///< Total clock time (s)
	CS_FG_LIVE_TIME,	///< Live time of foreground run (s)
	CS_BG_LIVE_TIME,	///< Live time of background run (s)
	CS_EMPTY1,
	CS_DET_AREA,		///< Area of detector (cm^2)
	CS_DET_DISTANCE,	///< Distance of detector (cm)
	CS_CH2_AREA,		///< Area of CH2 target (cm^2)
	CS_CH2_DISTANCE,	///< Distance of CH2 target (cm)
	CS_CH2_THICKNESS,	///< Thickness of CH2 target (cm)
	CS_C12_AREA,		///< Area of C12 target (cm^2)
	CS_C12_DISTANCE,	///< Distance of C12 target (cm^2)
	CS_C12_THICKNESS,	///< Thickness of C12 target (cm)
	CS_EMPTY2,
	CS_FG_PROTONS,		///< Number of protons in foreground run (protons)
	CS_BG_PROTONS,		///< Number of protons in background run (protons)
	CS_C11_C12,		///< Number of activated C11 nuclei in C12 target (C11 nuclei)
	CS_C11_C12_ERR,		///< Uncertainty in CS_C11_C12
	CS_C11_CH2,		///< Number of activated C11 nuclei in CH2 target (C11 nuclei)
	CS_C11_CH2_ERR,		///< Uncertainty in CS_C11_CH2
	CS_EMPTY3,
	CS_PROTON_FLUX,		///< Proton flux (protons/s)
	CS_PROTON_FLUX_ERR,	///< Uncertainty in CS_PROTON_FLUX
	CS_NEUTRON_FLUX,	///< Neutron flux (neutrons/s)
	CS_NEUTRON_FLUX_ERR,	///< Uncertainty in CS_NEUTRON_FLUX
	CS_XSECT_N2N_CH2,	///< (n,2n) cross section in CH2 (mbarn)
	CS_XSECT_N2N_CH2_ERR,	///< Uncertainty in CS_XSECT_N2N_CH2
	CS_XSECT_N2N_C12,	///< (n,2n) cross section in C12 (mbarn)
	CS_XSECT_N2N_C12_ERR,	///< Uncertainty in CS_XSECT_N2N_C12
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
