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
Error<double> CalcProtonFlux( int fg_protons, int bg_protons, double fg_live, double bg_live )
{
	Error<double> protons;
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
Error<double> CalcNeutronFlux( Error<double> protons, double sigma_np, Target * ch2, double sang_det )
{
	// 1 mbarn = 1e-3 barn
	double denom = sigma_np * ch2->ThicknessH() * sang_det * ch2->SolidAngle() * 1e-3;

	Error<double> flux;
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
 * @param target The target for which the cross section should be calculated
 * @param neutrons The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 * @param efficiency An efficiency correction factor
 * @param time The total activation time, @f$t_{act}@f$ (s)
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 * @return The cross section, @f$\sigma_{n2n}@f$ (mb)
 */
Error<double> CalcN2NCrossSection( Target * target, Error<double> neutrons, double efficiency, double time )
{	
	// 1 min = 60 s
	double decay = TMath::Log( 2 ) / (20.334 * 60);	// (1/s)

	// 1 mbarn = 1e-3 barn
	double denom = target->ThicknessC() * neutrons.value * target->SolidAngle() * 1e-3
		* (1 - TMath::Exp( -decay * time ));

	Error<double> xsect;
	xsect.value = target->c11.value * decay / (efficiency * denom);
	xsect.error = xsect.value * sqrt( pow( neutrons.error / neutrons.value, 2 ) +
			pow( target->c11.error / target->c11.value, 2 ) );
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
		ch2->area      = atof( row[CS_CH2_AREA].c_str() );
		ch2->distance  = atof( row[CS_CH2_DISTANCE].c_str() );
		ch2->thickness = atof( row[CS_CH2_THICKNESS].c_str() );
		ch2->c11.value = atof( row[CS_CH2_DECAY].c_str() );
		ch2->c11.error = atof( row[CS_CH2_DECAY_UNC].c_str() );

		// C12 target
		Target * c12 = new C12Target();
		c12->area      = atof( row[CS_C12_AREA].c_str() );
		c12->distance  = atof( row[CS_C12_DISTANCE].c_str() );
		c12->thickness = atof( row[CS_C12_THICKNESS].c_str() );
		c12->c11.value = atof( row[CS_C12_DECAY].c_str() );
		c12->c11.error = atof( row[CS_C12_DECAY_UNC].c_str() );

		// Calculate the proton flux
		int fg_protons = atoi( row[CS_FG_PROTONS].c_str() );
		int bg_protons = atoi( row[CS_BG_PROTONS].c_str() );

		double fg_clock = atof( row[CS_FG_CLOCK_TIME].c_str() );
		double fg_live = atof( row[CS_FG_LIVE_FRAC].c_str() );
		double bg_clock = atof( row[CS_BG_CLOCK_TIME].c_str() );
		double bg_live = atof( row[CS_BG_LIVE_FRAC].c_str() );

		Error<double> protons = calculate::CalcProtonFlux( fg_protons, bg_protons, 
						fg_clock * fg_live, bg_clock * bg_live );
		row[CS_PROTON_FLUX]     = TString::Format( "%f", protons.value );
		row[CS_PROTON_FLUX_UNC] = TString::Format( "%f", protons.error );

		// Calculate the neutron flux
		double area_det  = atof( row[CS_DET_AREA].c_str() );
		double dist_det  = atof( row[CS_DET_DISTANCE].c_str() );
		double sang_det = calculate::CalcSolidAngle( area_det, dist_det );

		double energy  = atof( row[CS_NEUTRON_ENERGY].c_str() );
		double sigma_np = calculate::CalcNPCrossSection( energy );

		Error<double> neutrons = calculate::CalcNeutronFlux( protons, sigma_np, ch2, sang_det );
		row[CS_NEUTRON_FLUX]     = TString::Format( "%f", neutrons.value );
		row[CS_NEUTRON_FLUX_UNC] = TString::Format( "%f", neutrons.error );

		// Calculate cross sections
		Error<double> sigma_n2n_ch2 = calculate::CalcN2NCrossSection( ch2, neutrons, 5.83, fg_clock );
		row[CS_CH2_XSECT]     = TString::Format( "%f", sigma_n2n_ch2.value );
		row[CS_CH2_XSECT_UNC] = TString::Format( "%f", sigma_n2n_ch2.error );

		Error<double> sigma_n2n_c12 = calculate::CalcN2NCrossSection( c12, neutrons, 1, fg_clock );
		row[CS_C12_XSECT]     = TString::Format( "%f", sigma_n2n_c12.value );
		row[CS_C12_XSECT_UNC] = TString::Format( "%f", sigma_n2n_c12.error );

		SetRow( i, row );
	}
}

} // namespace n2n