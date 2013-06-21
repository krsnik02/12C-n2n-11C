/**
 * @file n2n/CrossSection.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "CrossSection.hxx"
#include "Target.hxx"

namespace n2n {
namespace calculate {

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
UncertainD CalcProtonFlux( int fg_protons, double fg_clock, double fg_live, 
			   int bg_protons, double bg_clock, double bg_live )
{
	UncertainD protons;
	protons.val = (fg_protons / (fg_clock * fg_live)) - (bg_protons / (bg_clock * bg_live));
	protons.unc = sqrt( pow( sqrt( fg_protons ) / (fg_clock * fg_live), 2 ) +
			pow( sqrt( bg_protons ) / (bg_clock * bg_live), 2 ) );
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
UncertainD CalcN2NCrossSection( Target * target, UncertainD neutrons, double time )
{	
	// 1 min = 60 s
	double decay = TMath::Log( 2 ) / (20.334 * 60);	// (1/s)

	// 1 mbarn = 1e-3 barn
	double denom = target->ThicknessC() * neutrons.val * target->SolidAngle() * 1e-3
		* (1 - TMath::Exp( -decay * time ));

	UncertainD xsect;
	xsect.val = target->decay.val * decay / denom;
	xsect.unc = xsect.val * sqrt( pow( neutrons.unc / neutrons.val, 2 ) +
			pow( target->decay.unc / target->decay.val, 2 ) );
	return xsect;
}

} // namespace calculate

void CrossSection::Calculate()
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );

		// CH2 target
		Target * ch2 = new CH2Target();
		ch2->area      = n2n::ReadUncertainD( row, CS_CH2_AREA,      CS_CH2_AREA_UNC );
		ch2->distance  = n2n::ReadUncertainD( row, CS_CH2_DISTANCE,  CS_CH2_DISTANCE_UNC );
		ch2->thickness = n2n::ReadUncertainD( row, CS_CH2_THICKNESS, CS_CH2_THICKNESS_UNC );
		ch2->decay     = n2n::ReadUncertainD( row, CS_CH2_DECAY,     CS_CH2_DECAY_UNC );

		// C12 target
		Target * c12 = new C12Target();
		c12->area      = n2n::ReadUncertainD( row, CS_C12_AREA,      CS_C12_AREA_UNC );
		c12->distance  = n2n::ReadUncertainD( row, CS_C12_DISTANCE,  CS_C12_DISTANCE_UNC );
		c12->thickness = n2n::ReadUncertainD( row, CS_C12_THICKNESS, CS_C12_THICKNESS_UNC );
		c12->decay     = n2n::ReadUncertainD( row, CS_C12_DECAY,     CS_C12_DECAY_UNC );

		// Calculate the proton flux
		int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );
		int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );

		double fg_clock = atof( row[CS_FG_CLOCK_TIME].c_str() );
		double fg_live = atof( row[CS_FG_LIVE_FRAC].c_str() );
		double bg_clock = atof( row[CS_BG_CLOCK_TIME].c_str() );
		double bg_live = atof( row[CS_BG_LIVE_FRAC].c_str() );

		UncertainD protons = calculate::CalcProtonFlux( 
			fg_protons, fg_clock, fg_live, bg_protons, bg_clock, bg_live );
		n2n::WriteUncertainD( protons, row, CS_PROTON_FLUX, CS_PROTON_FLUX_UNC );

		// Calculate the neutron flux
		double area_det  = atof( row[CS_DET_AREA].c_str() );
		double dist_det  = atof( row[CS_DET_DISTANCE].c_str() );
		double sang_det = calculate::CalcSolidAngle( area_det, dist_det );

		double energy  = atof( row[CS_NEUTRON_ENERGY].c_str() );
		double sigma_np = calculate::CalcNPCrossSection( energy );

		UncertainD neutrons = calculate::CalcNeutronFlux( 
			protons, sigma_np, ch2->ThicknessH(), ch2->SolidAngle(), sang_det );
		n2n::WriteUncertainD( neutrons, row, CS_NEUTRON_FLUX, CS_NEUTRON_FLUX_UNC );

		// Calculate cross sections
		UncertainD sigma_n2n_ch2 = calculate::CalcN2NCrossSection( ch2, neutrons, fg_clock );
		UncertainD sigma_n2n_c12 = calculate::CalcN2NCrossSection( c12, neutrons, fg_clock );
		n2n::WriteUncertainD( sigma_n2n_ch2, row, CS_CH2_XSECT, CS_CH2_XSECT_UNC );
		n2n::WriteUncertainD( sigma_n2n_c12, row, CS_C12_XSECT, CS_C12_XSECT_UNC );

		SetRow( i, row );
	}
}

} // namespace n2n
