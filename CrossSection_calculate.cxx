/**
 * @file n2n/CrossSection_calculate.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "CrossSection.hxx"
#include "Uncertain.hxx"

namespace n2n {
namespace calculate {

/**
 * Calculate the proton flux, @f$N_p@f$.
 *
 * @f[N_p=
 *	\frac{N_{p,fg}}{t_{clock,fg}t_{live,fg}}-
 *      \frac{N_{p,bg}}{t_{clock,bg}t_{live,bg}}@f]
 * @f[{\delta_{N_p}}^2=
 * 	\left(\frac{\delta_{N_{p,fg}}}{t_{clock,fg}t_{live,fg}}\right)^2+
 * 	\left(\frac{\delta_{N_{p,bg}}}{t_{clock,bg}t_{live,bg}}\right)^2@f]
 *
 * @param fg_protons Number of protons in foreground, @f$N_{p,fg}@f$ (protons)
 * @param fg_clock Clock time of foreground, @f$t_{clock,fg}@f$ (s)
 * @param fg_live Fraction of live time in foreground, @f$t_{live,fg}@f$
 * @param bg_protons Number of protons in background, @f$N_{p,bg}@f$ (protons)
 * @param bg_clock Clock time of background, @f$t_{clock,bg}@f$ (s)
 * @param bg_live Fraction of live time in background, @f$t_{live,bg}@f$
 *
 * @return The proton flux, @f$N_p@f$ (@f$\frac{\text{protons}}{\text{s}}@f$)
 */
UncertainD ProtonFlux( UncertainD fg_protons, double fg_clock, double fg_live, 
		       UncertainD bg_protons, double bg_clock, double bg_live )
{
	UncertainD protons;
	protons.val = fg_protons.val / (fg_clock * fg_live) - 
                      bg_protons.val / (bg_clock * bg_live);
	protons.unc = sqrt( pow( fg_protons.unc / (fg_clock * fg_live), 2 ) +
			    pow( bg_protons.unc / (bg_clock * bg_live), 2 ) );
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
	return area / (dist * dist);
}

double CalcThicknessH_CH2( double thickness )
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass = 2 * mass_H + mass_C;
	double density = 0.89;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return 2 * thickness * density / (mass * 1.6605389);
}

double CalcThicknessC_CH2( double thickness )
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass = 2 * mass_H + mass_C;
	double density = 0.89;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return thickness * density / (mass * 1.6605389);
}

double CalcThicknessC_C12( double thickness )
{
	double mass = 12;		// u
	double density = 2.276;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return thickness * density / (mass * 1.6605389);
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
 * @param ch2 The @f$\text{CH}_2@f$ target.
 * @param sang_det The solid angle of the proton detector, @f$\Omega_{det}@f$ (sr)
 * @return The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 */
UncertainD CalcNeutronFlux( UncertainD protons, double sigma_np, double ch2_nH, double ch2_sang, 
			    double det_sang )
{
	// 1 mbarn = 1e-3 barn
	double denom = sigma_np * ch2_nH * ch2_sang * det_sang * 1e-3;

	UncertainD flux;
	flux.val = protons.val / denom;
	flux.unc = protons.unc / denom;
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
 * @param target The target for which the cross section should be calculated
 * @param neutrons The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 * @param efficiency An efficiency correction factor
 * @param time The total activation time, @f$t_{act}@f$ (s)
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 * @return The cross section, @f$\sigma_{n2n}@f$ (mb)
 */
UncertainD CalcN2NCrossSection( UncertainD tar_decay, UncertainD neutrons, double time, 
                                double tar_nC, double tar_sang )
{	
	// 1 min = 60 s
	double decay = TMath::Log( 2 ) / (20.334 * 60);	// (1/s)

	// 1 mbarn = 1e-3 barn
	double denom = tar_nC * neutrons.val * tar_sang * 1e-3
		* (1 - TMath::Exp( -decay * time ));

	UncertainD xsect;
	xsect.val = tar_decay.val * decay / denom;
	xsect.unc = xsect.val * sqrt( pow( neutrons.unc / neutrons.val, 2 ) +
			pow( tar_decay.unc / tar_decay.val, 2 ) );
	return xsect;
}

} // namespace calculate

void CrossSection::Calculate()
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );

		// CH2 target
		UncertainD ch2_area = n2n::ReadUncertainD( row, CS_CH2_AREA, CS_CH2_AREA_UNC );
		UncertainD ch2_distance = n2n::ReadUncertainD( row, CS_CH2_DISTANCE, CS_CH2_DISTANCE_UNC );
		UncertainD ch2_thickness = n2n::ReadUncertainD( row, CS_CH2_THICKNESS, CS_CH2_THICKNESS_UNC );
		UncertainD ch2_decay = n2n::ReadUncertainD( row, CS_CH2_DECAY, CS_CH2_DECAY_UNC );

		// C12 target
		UncertainD c12_area = n2n::ReadUncertainD( row, CS_C12_AREA, CS_C12_AREA_UNC );
		UncertainD c12_distance = n2n::ReadUncertainD( row, CS_C12_DISTANCE, CS_C12_DISTANCE_UNC );
		UncertainD c12_thickness = n2n::ReadUncertainD( row, CS_C12_THICKNESS, CS_C12_THICKNESS_UNC );
		UncertainD c12_decay = n2n::ReadUncertainD( row, CS_C12_DECAY, CS_C12_DECAY_UNC );

		// Calculate the proton flux
		UncertainD fg_protons = n2n::ReadUncertainD( row, CS_FG_PROTONS, CS_FG_PROTONS_UNC );
		UncertainD bg_protons = n2n::ReadUncertainD( row, CS_BG_PROTONS, CS_BG_PROTONS_UNC );

		double fg_clock = atof( row[CS_FG_CLOCK_TIME].c_str() );
		double fg_live = atof( row[CS_FG_LIVE_FRAC].c_str() );
		double bg_clock = atof( row[CS_BG_CLOCK_TIME].c_str() );
		double bg_live = atof( row[CS_BG_LIVE_FRAC].c_str() );

		UncertainD protons = calculate::ProtonFlux( 
			fg_protons, fg_clock, fg_live, bg_protons, bg_clock, bg_live );
		n2n::WriteUncertainD( protons, &row, CS_PROTON_FLUX, CS_PROTON_FLUX_UNC );

		// Calculate the neutron flux
		double area_det  = atof( row[CS_DET_AREA].c_str() );
		double dist_det  = atof( row[CS_DET_DISTANCE].c_str() );
		double sang_det = calculate::CalcSolidAngle( area_det, dist_det );

		double energy  = atof( row[CS_NEUTRON_ENERGY].c_str() );
		double sigma_np = calculate::CalcNPCrossSection( energy );

		double ch2_sang = calculate::CalcSolidAngle( ch2_area.val, ch2_distance.val );
		double ch2_nH = calculate::CalcThicknessH_CH2( ch2_thickness.val );
		double ch2_nC = calculate::CalcThicknessC_CH2( ch2_thickness.val );

		double c12_sang = calculate::CalcSolidAngle( c12_area.val, c12_distance.val );
		double c12_nC = calculate::CalcThicknessC_C12( c12_thickness.val );

		UncertainD neutrons = calculate::CalcNeutronFlux( 
			protons, sigma_np, ch2_nH, ch2_sang, sang_det );
		n2n::WriteUncertainD( neutrons, &row, CS_NEUTRON_FLUX, CS_NEUTRON_FLUX_UNC );

		// Calculate cross sections
		UncertainD sigma_n2n_ch2 = calculate::CalcN2NCrossSection( 
			ch2_decay, neutrons, fg_clock, ch2_nC, ch2_sang );
		UncertainD sigma_n2n_c12 = calculate::CalcN2NCrossSection( 
			c12_decay, neutrons, fg_clock, c12_nC, c12_sang );
		n2n::WriteUncertainD( sigma_n2n_ch2, &row, CS_CH2_XSECT, CS_CH2_XSECT_UNC );
		n2n::WriteUncertainD( sigma_n2n_c12, &row, CS_C12_XSECT, CS_C12_XSECT_UNC );

		SetRow( i, row );
	}
}

} // namespace n2n
