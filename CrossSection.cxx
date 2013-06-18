/**
 * @file n2n/CrossSection.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "CrossSection.hxx"
#include "Error.hxx"

namespace cs {

void UpdateSummary( vector<string> & row, n2n::RunSummary const * const summary )
{
	int fg_run_number = atoi( row[CS_FG_RUN_NUMBER].c_str() );
	int bg_run_number = atoi( row[CS_BG_RUN_NUMBER].c_str() );
	vector<string> fg = summary->GetRun( fg_run_number );
	vector<string> bg = summary->GetRun( bg_run_number );

	assert( row.size() == CS_NUM_COLUMNS );
	assert( fg.size() == n2n::RS_NUM_COLUMNS );
	assert( bg.size() == n2n::RS_NUM_COLUMNS );

	// Run data
	row[CS_NEUTRON_ENERGY] 	= fg[n2n::RS_NEUTRON_ENERGY];
	row[CS_CLOCK_TIME]	= fg[n2n::RS_CLOCK_TIME];
	row[CS_FG_LIVE_TIME] 	= fg[n2n::RS_LIVE_TIME];
	row[CS_BG_LIVE_TIME] 	= bg[n2n::RS_LIVE_TIME];

	// Geometry
	row[CS_DET_AREA]	= "0.7133";	// cm^2
	row[CS_DET_DISTANCE]	= "12.07";	// cm
	row[CS_CH2_AREA]	= "5.067075";	// cm^2
	row[CS_CH2_DISTANCE]	= "6.46";	// cm
	row[CS_CH2_THICKNESS]	= "0.164";	// cm
	row[CS_C12_AREA]	= "43.20869";	// cm^2
	row[CS_C12_DISTANCE]	= "14.52";	// cm
	row[CS_C12_THICKNESS]	= "0.889";	// cm

	// Calculated values
	row[CS_FG_PROTONS] 	= fg[n2n::RS_GROSS_PROTONS];
	row[CS_BG_PROTONS] 	= bg[n2n::RS_GROSS_PROTONS];
	row[CS_C11_C12]		= fg[n2n::RS_C11_PUCK];
	row[CS_C11_C12_ERR]	= fg[n2n::RS_C11_PUCK_ERR];
	row[CS_C11_CH2]		= fg[n2n::RS_C11_PLASTIC];
	row[CS_C11_CH2_ERR]	= fg[n2n::RS_C11_PLASTIC_ERR];
}

void CrossSection::LoadSummary( n2n::RunSummary const * const summary )
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );
		cs::UpdateSummary( row, summary );
		SetRow( i, row );
	}
}



/**
 * Calculate the proton flux, @f$N_p@f$.
 *
 * @f[N_p=\frac{N_{p,fg}}{t_{live,fg}}-\frac{N_{p,bg}}{t_{live,bg}}@f]
 * @f[\delta_{N_p}=\sqrt{\left(\frac{\sqrt{N_{p,fg}}}{t_{live,fg}}\right)^2+
 * \left(\frac{\sqrt{N_{p,bg}}}{t_{live,bg}}\right)^2}@f]
 *
 * @param fg_protons Number of protons in foreground, @f$N_{p,fg}@f$ (protons)
 * @param bg_protons Number of protons in background, @f$N_{p,bg}@f$ (protons)
 * @param fg_live Live time of foreground, @f$t_{live,fg}@f$ (s)
 * @param bg_live Live time of background, @f$t_{live,bg}@f$ (s)
 * @return The proton flux, @f$N_p@f$ (@f$\frac{\text{protons}}{\text{s}}@f$)
 */
n2n::Error<double> CalcProtonFlux( int fg_protons, int bg_protons, double fg_live, double bg_live )
{
	n2n::Error<double> protons;
	protons.value = (fg_protons / fg_live) - (bg_protons / bg_live);
	protons.error = sqrt( pow( sqrt( fg_protons ) / fg_live, 2 ) +
			pow( sqrt( bg_protons ) / bg_live, 2 ) );
	return protons;
}

/**
 * Calculate the @f$(n,p)@f$ cross section, @f$\sigma_{np}(T)@f$, at the given energy 
 * by interpolating between known cross sections.
 *
 * @param energy The kinetic energy, @f$T@f$ (MeV)
 * @return The cross section, @f$\sigma_{np}(T)@f$ 
 * (@f$\frac{\text{mbarn}}{\text{sr}}@f$)
 */
double CalcNPCrossSection( double energy )
{
	// Data from http://nn-online.org/
	// Lab frame
	double energies[] = {  20,  22,  24,  26,  28 };	// (MeV)
	double xsects[]   = { 153, 139, 128, 119, 111 };	// (mbarn/sr)

	ROOT::Math::Interpolator interp( 5 );
	interp.SetData( 5, energies, xsects );
	return interp.Eval( energy );
}

/**
 * Determine the number thickness of the @f$\text{CH}_2@f$ target, @f$N_{H,CH_2}@f$.
 *
 * @f[N_{H,CH_2}=2\frac{\rho_{CH_2}}{m_{CH_2}}t_{CH_2}@f]
 *
 * @param thickness the thickness of the @f$\text{CH}_2@f$, @f$t_{CH_2}@f$ (cm)
 * @return the number thickness of hydrogen, @f$N_{H,CH_2}@f$ 
 * (@f$\frac{\text{H nuclei}}{\text{barn}}@f$)
 */
double CalcThickness_H_CH2( double thickness )
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass_CH2 = 2 * mass_H + mass_C;
	double density_CH2 = 0.89;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	// 1 u/cm^2 = 1.6605389 g/barn
	return 2 * thickness * density_CH2 / (mass_CH2 * 1.6605389);
}

/**
 * Determine the number thickness of the @f$\text{CH}_2@f$ target, @f$N_{C,CH_2}@f$.
 *
 * @f[N_{C,CH_2}=\frac{\rho_{CH_2}}{m_{CH_2}}t_{CH_2}@f]
 *
 * @param thickness the thickness of the @f$\text{CH}_2@f$, @f$t_{CH_2}@f$ (cm)
 * @return the number thickness of carbon, @f$N_{C,CH_2}@f$ 
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 */
double CalcThickness_C_CH2( double thickness )
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass_CH2 = 2 * mass_H + mass_C;
	double density_CH2 = 0.89;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	// 1 u/cm^2 = 1.6605389 g/barn
	return thickness * density_CH2 / (mass_CH2 * 1.6605389);
}

/**
 * Determine the number thickness of the @f${}^{12}\text{C}@f$ target, @f$N_{C,C12}@f$.
 *
 * @f[N_{C,C12}=\frac{\rho_{C12}}{m_{C12}}t_{C12}@f]
 *
 * @param thickness the thickness of the @f${}^{12}\text{C}@f$, @f$t_{C12}@f$ (cm)
 * @return the number thickness of carbon, @f$N_{C,C12}@f$
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 */
double CalcThickness_C_C12( double thickness )
{
	double mass_C = 12;		// u
	double density_C = 2.276;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	// 1 u/cm^2 = 1.6605389 g/barn
	return thickness * density_C / (mass_C * 1.6605389); 
}

/**
 * Calculate a solid angle from a distance and area.
 *
 * @f[\Omega=\frac{A}{d^2}@f]
 *
 * @param area The area of the target, @f$A@f$ (@f$\text{cm}^2@f$)
 * @param dist The distance to the target, @f$d@f$ (cm)
 * @return The solid angle, @f$\Omega@f$ (sr)
 */
double CalcSolidAngle( double area, double dist )
{
	return area / pow( dist, 2 );
}

/**
 * Calculate the neutron flux, @f$N_{flux}@f$.
 *
 * @f[N_{flux}=\frac{N_p}{\sigma_{np}(T)N_{H,CH_2}\Omega_{det}\Omega_{CH_2}}@f]
 * @f[\delta_{N_{flux}}=\frac{\delta_{N_p}}{\sigma_{np}(T)N_{H,CH_2}\Omega_{det}\Omega_{CH_2}}@f]
 *
 * @param protons The proton flux, @f$N_p@f$ (@f$\frac{\text{protons}}{\text{s}}@f$)
 * @param sigma_np The cross section of the @f$(n,p)@f$ reaction, @f$\sigma_{np}@f$ 
 * (@f$\frac{\text{mbarn}}{\text{sr}}@f$)
 * @param nH_ch2 The number thickness of hydrogen, @f$N_{H,CH_2}@f$ 
 * (@f$\frac{\text{H nuclei}}{\text{barn}}@f$)
 * @param sang_det The solid angle of the proton detector, @f$\Omega_{det}@f$ (sr)
 * @param sang_ch2 The solid angle of the ch2 target, @f$\Omega_{CH_2}@f$ (sr)
 * @return The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 */
n2n::Error<double> CalcNeutronFlux( n2n::Error<double> protons, double sigma_np, double nH_ch2,
		double sang_det, double sang_ch2 )
{
	// 1 mbarn = 1e-3 barn
	double denom = sigma_np * nH_ch2 * sang_det * sang_ch2 * 1e-3;

	n2n::Error<double> flux;
	flux.value = protons.value / denom;
	flux.error = protons.error / denom;
	return flux;
}

/**
 * Calculate the @f${}^{12}\text{C}(n,2n){}^{11}\text{C}@f$ cross section, 
 * @f$\sigma_{n2n}@f$, where @f$\lambda_{C11} = \frac{\ln(2)}{20.334~\text{min}}@f$
 * is the @f${}^{11}\text{C}@f$ decay constant.
 *
 * @f[\sigma_{n2n}=\frac{N_{C11}}{\text{efficiency}}\frac{\lambda_{C11}}
 * {N_{C,tar}N_{flux}\Omega_{tar}(1-e^{-\lambda_{C11}t_{act}})}@f]
 * @f[\delta_{\sigma_{n2n}}=\frac{N_{C11}}{\text{efficiency}}\frac{\lambda_{C11}}
 * {N_{C,tar}N_{flux}\Omega_{tar}(1-e^{-\lambda_{C11}t_{act}})}
 * \sqrt{\left(\frac{\delta_{N_{flux}}}{N_{flux}}\right)^2+
 * \left(\frac{\delta_{N_{C11}}}{N_{C11}}\right)^2}@f]
 * 
 * @param c11 The number of @f${}^{11}\text{C}@f$ nuclei, @f$N_{C11}@f$ 
 * (@f${}^{11}\text{C}@f$ nuclei)
 * @param flux The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 * @param efficiency An efficiency correction factor
 * @param time The total activation time, @f$t_{act}@f$ (s)
 * @param thickness The number thickness of carbon in target, @f$N_{C,tar}@f$ 
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 * @param sang The solid angle of target, @f$\Omega_{tar}@f$ (sr)
 * @return The cross section, @f$\sigma_{n2n}@f$ (mb)
 */
n2n::Error<double> CalcN2NCrossSection( n2n::Error<double> c11, n2n::Error<double> flux, 
		double efficiency, double time, double thickness, double sang )
{	
	// 1 min = 60 s
	double decay = TMath::Log( 2 ) / (20.334 * 60);	// (1/s)

	// 1 mbarn = 1e-3 barn
	double denom = thickness * flux.value * sang * 1e-3
		* (1 - TMath::Exp( -decay * time ));

	n2n::Error<double> xsect;
	xsect.value = c11.value * decay / (efficiency * denom);
	xsect.error = xsect.value * sqrt( pow( flux.error / flux.value, 2 ) +
			pow( c11.error / c11.value, 2 ) );
	return xsect;
}

void CrossSection::Calculate()
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );

		// Run data
		double clock   = atof( row[CS_CLOCK_TIME].c_str() );
		double fg_live = atof( row[CS_FG_LIVE_TIME].c_str() );
		double bg_live = atof( row[CS_BG_LIVE_TIME].c_str() );
		double energy  = atof( row[CS_NEUTRON_ENERGY].c_str() );

		double sigma_np = cs::CalcNPCrossSection( energy );

		// Geometry
		double area_det  = atof( row[CS_DET_AREA].c_str() );
		double dist_det  = atof( row[CS_DET_DISTANCE].c_str() );
		double area_ch2  = atof( row[CS_CH2_AREA].c_str() );
		double dist_ch2  = atof( row[CS_CH2_DISTANCE].c_str() );
		double thick_ch2 = atof( row[CS_CH2_THICKNESS].c_str() );
		double area_c12  = atof( row[CS_C12_AREA].c_str() );
		double dist_c12  = atof( row[CS_C12_DISTANCE].c_str() );
		double thick_c12 = atof( row[CS_C12_THICKNESS].c_str() );

		double sang_det = cs::CalcSolidAngle( area_det, dist_det );
		double sang_ch2 = cs::CalcSolidAngle( area_ch2, dist_ch2 );
		double nH_ch2   = cs::CalcThickness_H_CH2( thick_ch2 );
		double nC_ch2   = cs::CalcThickness_C_CH2( thick_ch2 );
		double sang_c12 = cs::CalcSolidAngle( area_c12, dist_c12 );
		double nC_c12   = cs::CalcThickness_C_C12( thick_c12 );

		// Calculated data
		int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );
		int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );
		n2n::Error<double> c11_c12;
		c11_c12.value = atof( row[CS_C11_C12].c_str() );
		c11_c12.error = atof( row[CS_C11_C12_ERR].c_str() );
		n2n::Error<double> c11_ch2;
		c11_ch2.value = atof( row[CS_C11_CH2].c_str() );
		c11_ch2.error = atof( row[CS_C11_CH2_ERR].c_str() );

		// Calculate the flux
		n2n::Error<double> protons = 
			cs::CalcProtonFlux( fg_protons, bg_protons, fg_live, bg_live );
		row[CS_PROTON_FLUX]     = TString::Format( "%f", protons.value );
		row[CS_PROTON_FLUX_ERR] = TString::Format( "%f", protons.error );

		n2n::Error<double> neutrons = 
			cs::CalcNeutronFlux( protons, sigma_np, nH_ch2, sang_det, sang_ch2 );
		row[CS_NEUTRON_FLUX]     = TString::Format( "%f", neutrons.value );
		row[CS_NEUTRON_FLUX_ERR] = TString::Format( "%f", neutrons.error );

		// Caclulate cross sections
		n2n::Error<double> sigma_n2n_ch2 =
			cs::CalcN2NCrossSection( c11_ch2, neutrons, 5.83, clock, nC_ch2, sang_ch2 );
		n2n::Error<double> sigma_n2n_c12 =
			cs::CalcN2NCrossSection( c11_c12, neutrons, 1, clock, nC_c12, sang_c12 );
		row[CS_XSECT_N2N_CH2]     = TString::Format( "%f", sigma_n2n_ch2.value );
		row[CS_XSECT_N2N_CH2_ERR] = TString::Format( "%f", sigma_n2n_ch2.error );
		row[CS_XSECT_N2N_C12]     = TString::Format( "%f", sigma_n2n_c12.value );
		row[CS_XSECT_N2N_C12_ERR] = TString::Format( "%f", sigma_n2n_c12.error );

		SetRow( i, row );
	}
}

} // namespace cs
