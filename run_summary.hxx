/** @file run_summary.hxx
 * Copyright (C) 2013 Houghton College
 */

#include "csv_file.hxx"

namespace rs {

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


struct RunSummary : public csv::CSVFile
{
	public:
		/**
		 * Retrieve a run by number.
		 * @param run_number The run to retrieve.
		 * @return The requested run.
		 */
		vector<string> GetRun( int run_number ) const;

		/**
		 * Save a run by number.
		 * @param run_number The run to save.
		 * @param row The values to write.
		 */
		void SetRun( int run_number, vector<string> const & row );

		/**
		 * Get the number of runs in the file
		 */
		int NumRuns() const;

		/**
		 * Calculate the number of C11 nuclei and protons for each run.
		 * @param dirname The directory containing all relevant data files.
		 */
		void Update( char const * dirname );
};

} // namespace rs
