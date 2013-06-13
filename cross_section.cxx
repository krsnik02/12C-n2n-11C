/** @file cross_section.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "cross_section.hxx"

namespace cs {

template <typename T>
struct Error
{
	T value, error;
};

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

/**
 * Calculate the proton flux, @f$N_p@f$.
 * @f[N_p=\frac{N_{p,fg}}{t_{live,fg}}-\frac{N_{p,bg}}{t_{live,bg}}@f]
 * @f[\delta_{N_p}=\sqrt{\left(\frac{\sqrt{N_{p,fg}}}{t_{live,fg}}\right)^2+
 * \left(\frac{\sqrt{N_{p,bg}}}{t_{live,bg}}\right)^2}@f]
 *
 * @param fg_protons Number of protons in foreground, @f$N_{p,fg}@f$ (protons)
 * @param bg_protons Number of protons in background, @f$N_{p,bg}@f$ (protons)
 * @param fg_live Live time of foreground, @f$t_{live,fg}@f$ (s)
 * @param bg_live Live time of background, @f$t_{live,bg}@f$ (s)
 * @return The proton flux, @f$N_p@f$ (protons/s)
 */
Error<double> CalcProtons( int fg_protons, int bg_protons, double fg_live, double bg_live )
{
	Error<double> protons;
	protons.value = (fg_protons / fg_live) - (bg_protons / bg_live);
	protons.error = sqrt( pow( sqrt( fg_protons ) / fg_live, 2 ) +
			pow( sqrt( bg_protons ) / bg_live, 2 ) );
	return protons;
}

/**
 * Calculate the @f$(n,p)@f$ cross section, @f$\sigma_{np}@f$, at the given energy 
 * by interpolating between known cross sections.
 *
 * @param energy The kinetic energy, @f$T@f$ (MeV)
 * @return The cross section, @f$\sigma_{np}@f$	(mb/sr)
 */
double CalcNPCrossSection( double energy )
{
	double energies[] = {  20,  22,  24,  26,  28 };	// (MeV)
	double xsects[]   = { 153, 139, 128, 119, 111 };	// (mb/sr)
	ROOT::Math::Interpolator interp( 5 );
	interp.SetData( 5, energies, xsects );
	return interp.Eval( energy );
}

/**
 * Determine the thickness of the @f$\text{CH}_2@f$ target, @f$N_{H,CH_2}@f$ 
 * (@f$\frac{\text{H nuclei}}{\mathrm{cm}^2}@f$).
 * @f[t_{H,CH_2}=2\frac{\rho_{CH_2}}{m_{CH_2}}t_{CH_2}@f]
 *
 * @param thickness the thickness of the @f$\text{CH}_2@f$, @f$t_{CH_2}@f$ (cm)
 * @return the number thickness of hydrogen, @f$t_{H,CH_2}@f$ 
 * (@f$\frac{\text{H nuclei}}{\mathrm{cm}^2}@f$)
 */
double CalcThickness_H_CH2( double thickness )
{
	double mass_H = 1.007825;	// u = g/mol
	double mass_C = 12;		// u = g/mol
	double mass_CH2 = 2 * mass_H + mass_C;
	double density_CH2 = 0.89;	// g/cm^3

	double N_a = 6.0231415e23;	// nuclei/mol
	return 2 * thickness * density_CH2 * N_a / mass_CH2;
}


void CrossSection::Update( rs::RunSummary const * const summary )
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );
		cs::UpdateSummary( row, summary );

		int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );
		int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );
		double fg_live = atof( row[CS_FG_LIVE_TIME].c_str() );
		double bg_live = atof( row[CS_BG_LIVE_TIME].c_str() );
		Error<double> protons = CalcProtons( fg_protons, bg_protons, fg_live, bg_live );
		row[CS_PROTONS] = TString::Format( "%f", protons.value );
		row[CS_PROTONS_ERR] = TString::Format( "%f", protons.error );

		double energy = atoi( row[CS_NEUTRON_ENERGY].c_str() );
		double sigma_np = CalcNPCrossSection( energy );
		row[CS_NP_CROSS_SECTION] = TString::Format( "%f", sigma_np );

		double tt_CH2 = atof( row[CS_CH2_THICKNESS].c_str() );
		double tH_CH2 = CalcThickness_H_CH2( tt_CH2 );
		row[CS_CH2_NUM_H] = TString::Format( "%f", tH_CH2 );

		SetRow( i, row );
	}
}

} // namespace cs
