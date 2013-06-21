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
 * @f[N_p=\frac{N_{p,fg}}{t_{clock,fg}t_{live,fg}}-\frac{N_{p,bg}}{t_{clock,bg}t_{live,bg}}@f]
 * @f[\delta_{N_p}^2=\left(\frac{N_{p,fg}}{t_{clock,fg}t_{live,fg}}\right)^2
 * \left(\left(\frac{\delta_{N_{p,fg}}}{N_{p,fg}}\right)^2+
 *       \left(\frac{\delta_{t_{clock,fg}}}{t_{clock,fg}}\right)^2+
 *       \left(\frac{\delta_{t_{live,fg}}}{t_{live,fg}}\right)^2\right)+
 * \left(\frac{N_{p,bg}}{t_{clock,bg}t_{live,bg}}\right)^2
 * \left(\left(\frac{\delta_{N_{p,bg}}}{N_{p,bg}}\right)^2+
 *       \left(\frac{\delta_{t_{clock,bg}}}{t_{clock,bg}}\right)^2+
 *       \left(\frac{\delta_{t_{live,bg}}}{t_{live,bg}}\right)^2\right)@f]
 *
 * @param fg_protons Number of protons in foreground, @f$N_{p,fg}@f$ (protons)
 * @param fg_clock_time Clock time of foreground, @f$t_{clock,fg}@f$ (s)
 * @param fg_live_frac Fraction of live time in foreground, @f$t_{live,fg}@f$

 * @param bg_protons Number of protons in background, @f$N_{p,bg}@f$ (protons)
 * @param bg_clock_time Clock time of background, @f$t_{clock,bg}@f$ (s)
 * @param bg_live_frac Fraction of live time in background, @f$t_{live,bg}@f$
 *
 * @return The proton flux, @f$N_p@f$ (@f$\frac{\text{protons}}{\text{s}}@f$)
 */
UncertainD ProtonFlux( UncertainD fg_protons, UncertainD fg_clock_time, UncertainD fg_live_frac,
		       UncertainD bg_protons, UncertainD bg_clock_time, UncertainD bg_live_frac )
{
	double fg = fg_protons.val / (fg_clock_time.val * fg_live_frac.val);
	double bg = bg_protons.val / (bg_clock_time.val * bg_live_frac.val);
	double d_fg2 = pow( fg, 2 ) * ( pow( fg_protons.unc / fg_protons.val, 2 ) +
			                pow( fg_clock_time.unc / fg_clock_time.val, 2 ) +
			                pow( fg_live_frac.unc / fg_live_frac.val, 2 ) );
	double d_bg2 = pow( bg, 2 ) * ( pow( bg_protons.unc / bg_protons.val, 2 ) +
			                pow( bg_clock_time.unc / bg_clock_time.val, 2 ) +
			                pow( bg_live_frac.unc / bg_live_frac.val, 2 ) );
	
	UncertainD protons;
	protons.val = fg - bg;
	protons.unc = sqrt( d_fg2 + d_bg2 );
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
UncertainD CalcNPCrossSection( UncertainD energy )
{
	// Data from http://nn-online.org/
	// Lab frame
	double energies[] = {  20,  22,  24,  26,  28 };	// (MeV)
	double xsects[]   = { 153, 139, 128, 119, 111 };	// (mbarn/sr)

	ROOT::Math::Interpolator interp( 5 );
	interp.SetData( 5, energies, xsects );

	UncertainD xsect;
	xsect.val = interp.Eval( energy.val );
	xsect.unc = 0;
	return xsect;
}



/**
 * Calculate a solid angle from a distance and area.
 *
 * @f[\Omega=\frac{A}{d^2}@f]
 * @f[\delta_\Omega^2=\left(\frac{A}{d^2}\right)
 * \left(\left(\frac{\delta_A}{A}\right)^2+
 *       \left(\frac{2\delta_d}{d}\right)^2\right)@f]
 *
 * @param area The area of the target, @f$A@f$ (@f$\text{cm}^2@f$)
 * @param distance The distance to the target, @f$d@f$ (cm)
 *
 * @return The solid angle, @f$\Omega@f$ (sr)
 */
UncertainD SolidAngle( UncertainD area, UncertainD distance )
{
	UncertainD sang;
	sang.val = area.val / pow( distance.val, 2 );
	sang.unc = sang.val * ( pow( area.unc / area.val, 2 ) + pow( 2 * distance.unc / distance.val, 2 ) );
	return sang;
}



/**
 * Calculate the number thickness of H in @f$\text{CH}_2@f$.
 *
 * @f[N_{H,CH_2}=2\,x_{CH_2}\frac{\rho_{CH_2}}{m_{CH_2}}@f]
 * 
 * @param thickness The thickness of the @f$\text{CH}_2@f$, @f$x_{CH_2}@f$ (cm)
 *
 * @return The number thickness of H, @f$N_{H,CH_2}@f$ (@f$\frac{\text{H nuclei}}{\text{barn}}@f$)
 */
UncertainD CalcNumThickH_CH2( UncertainD thickness )
{
	static const double mass_H = 1.007825;	// u
	static const double mass_C = 12;	// u
	static const double mass = 2 * mass_H * mass_C;
	static const double density = 0.89;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	UncertainD nH;
	nH.val = 2 * thickness.val * density / (mass * 1.6605389);
	nH.unc = 2 * thickness.unc * density / (mass * 1.6605389);
	return nH;
}

/**
 * Calculate the number thickness of C in @f$\text{CH}_2@f$.
 *
 * @f[N_{C,CH_2}=x_{CH_2}\frac{\rho_{CH_2}}{m_{CH_2}}@f]
 * 
 * @param thickness The thickness of the @f$\text{CH}_2@f$, @f$x_{CH_2}@f$ (cm)
 *
 * @return The number thickness of C, @f$N_{C,CH_2}@f$ (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 */
UncertainD CalcNumThickC_CH2( UncertainD thickness )
{
	static const double mass_H = 1.007825;	// u
	static const double mass_C = 12;	// u
	static const double mass = 2 * mass_H * mass_C;
	static const double density = 0.89;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	UncertainD nC;
	nC.val = thickness.val * density / (mass * 1.6605389);
	nC.unc = thickness.unc * density / (mass * 1.6605389);
	return nC;
}

/**
 * Calculate the number thickness of C in @f${}^{12}\text{C}@f$.
 *
 * @f[N_{C,C12}=x_{C12}\frac{\rho_{C12}}{m_{C12}}@f]
 *
 * @param thickness The thickness of the @f${}^{12}\text{C}@f$, @f$x_{C12}@f$ (cm)
 *
 * @return The number thickness of C, @f$N_{C,C12}@f$ (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 */
UncertainD CalcNumThickC_C12( UncertainD thickness )
{
	static const double mass = 12;		// u
	static const double density = 2.276;	// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	UncertainD nC;
	nC.val = thickness.val * density / (mass * 1.6605389);
	nC.unc = thickness.unc * density / (mass * 1.6605389);
	return nC;
}



/**
 * Calculate the neutron flux, @f$N_{n}@f$.
 *
 * @f[N_n=\frac{N_p}{\sigma_{np}N_{H,CH_2}\Omega_{CH_2}\Omega_{det}}@f]
 * @f[\delta_{N_n}^2=\left(\frac{N_p}{\sigma_{np}N_{H,CH_2}\Omega{CH_2}\Omega_{det}}\right)^2
 * \left(\left(\frac{\delta_{N_p}}{N_p}\right)^2+
 *       \left(\frac{\delta_{\sigma_{np}}}{\sigma_{np}}\right)^2+
 *       \left(\frac{\delta_{N_{H,CH_2}}}{N_{H,CH_2}}\right)^2+
 *       \left(\frac{\delta_{\Omega_{CH_2}}}{\Omega_{CH_2}}\right)^2+
 *       \left(\frac{\delta_{\Omega_{det}}}{\Omega_{det}}\right)^2\right)@f]
 *
 * @param protons The proton flux, @f$N_p@f$ (@f$\frac{\text{protons}}{\text{s}}@f$)
 * @param sigma_np The cross section of the @f$(n,p)@f$ reaction, @f$\sigma_{np}@f$ 
 * (@f$\frac{\text{mbarn}}{\text{sr}}@f$)
 * @param ch2_nH The number thickness of H in the CH2 target, @f$N_{H,CH_2}@f$ 
 * (@f$\frac{\text{H nuclei}}{\text{barn}}@f$)
 * @param ch2_sang The solid angle of the CH2 target, @f$\Omega_{CH_2}@f$ (sr)
 * @param det_sang The solid angle of the proton detector, @f$\Omega_{det}@f$ (sr)
 *
 * @return The neutron flux, @f$N_{n}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 */
UncertainD NeutronFlux( UncertainD protons, UncertainD sigma_np, 
                        UncertainD ch2_nH, UncertainD ch2_sang, UncertainD det_sang )
{
	// 1 mbarn = 1e-3 barn
	UncertainD neutrons;
	neutrons.val = protons.val / (sigma_np.val * ch2_nH.val * ch2_sang.val * det_sang.val * 1e-3);
	neutrons.unc = neutrons.val * sqrt( pow( protons.unc / protons.val, 2 ) +
                                            pow( sigma_np.unc / sigma_np.val, 2 ) +
                                            pow( ch2_nH.unc / ch2_nH.val, 2 ) +
					    pow( ch2_sang.unc / ch2_sang.val, 2 ) +
					    pow( det_sang.unc / det_sang.val, 2 ) );
	return neutrons;
}

/**
 * Calculate the @f${}^{12}\text{C}(n,2n){}^{11}\text{C}@f$ cross section, 
 * @f$\sigma_{n2n}@f$, where @f$\lambda = \frac{\ln(2)}{20.334~\text{min}}@f$
 * is the @f${}^{11}\text{C}@f$ decay constant.
 *
 * @f[\sigma_{n2n}=\frac{N_{C11}\lambda}{N_{C,tar}\Omega_{tar}N_n(1-e^{-\lambda t_{act}})}@f]
 * @f[\delta_{\sigma_{n2n}}^2=
 * \left(\frac{N_{C11}\lambda}{N_{C,tar}\Omega_{tar}N_n(1-e^{-\lambda t_{act}})}\right)^2
 * \left(\left(\frac{\delta_{N_{C11}}}{N_{C11}}\right)^2+
 *       \left(\frac{\delta_{N_{C,tar}}}{N_{C,tar}}\right)^2+
 *	 \left(\frac{\delta_{\Omega_{tar}}}{\Omega_{tar}}\right)^2+
 *	 \left(\frac{\delta_{N_n}}{N_n}\right)^2+
 *	 \left(\frac{\delta_\lambda}{\lambda}-
 *             \frac{\delta_\lambda t_{act}e^{-\lambda t_{act}}}
 *                  {1-e^{-\lambda t_{act}}}\right)^2+
 *       \left(\frac{\delta_{t_{act}}\lambda e^{-\lambda t_{act}}}
 *                  {1-e^{-\lambda t_{act}}}\right)^2\right)@f]
 * 
 * @param target The target for which the cross section should be calculated
 * @param neutrons The neutron flux, @f$N_{flux}@f$ 
 * (@f$\frac{\text{neutrons}}{\text{s}\cdot\text{sr}}@f$)
 * @param time The total activation time, @f$t_{act}@f$ (s)
 * (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
 * @return The cross section, @f$\sigma_{n2n}@f$ (mb)
 */
UncertainD N2NCrossSection( UncertainD tar_decays, UncertainD tar_nC, UncertainD tar_sang,
                            UncertainD neutrons, UncertainD time )
{	
	// 1 min = 60 s
	UncertainD decay;
	decay.val = TMath::Log( 2 ) / (20.334 * 60);	// (1/s)
	decay.unc = 0;

	// 1 mbarn = 1e-3 barn
	UncertainD xsect;
	double elt = TMath::Exp( -decay.val * time.val );
	xsect.val = (tar_decays.val * decay.val) /
		    (tar_nC.val * tar_sang.val * neutrons.val * 1e-3 * (1 - elt));
	xsect.unc = xsect.val * sqrt( pow( tar_decays.unc / tar_decays.val, 2 ) +
				      pow( tar_nC.unc / tar_nC.val, 2 ) +
				      pow( tar_sang.unc / tar_sang.val, 2 ) +
				      pow( neutrons.unc / neutrons.val, 2 ) +
				      pow( decay.unc * (1 / decay.val - time.val * elt / (1 - elt)), 2 ) +
				      pow( decay.val * time.unc * elt / (1 - elt), 2 ) );
	return xsect;
}

} // namespace calculate

void CrossSection::Calculate()
{
	for ( int i = 3; i < NumRows(); ++i )
	{
		vector<string> row = GetRow( i );
		clog << row[CS_FG_RUN_NUMBER] << "\t" << row[CS_BG_RUN_NUMBER] << endl;

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

		// Detector
		UncertainD det_area = n2n::ReadUncertainD( row, CS_DET_AREA, CS_DET_AREA_UNC );
		UncertainD det_distance = n2n::ReadUncertainD( row, CS_DET_DISTANCE, CS_DET_DISTANCE_UNC );

		// Calculate the proton flux
		UncertainD fg_protons = n2n::ReadUncertainD( row, CS_FG_PROTONS, CS_FG_PROTONS_UNC );
		UncertainD bg_protons = n2n::ReadUncertainD( row, CS_BG_PROTONS, CS_BG_PROTONS_UNC );
		UncertainD fg_clock_time = n2n::ReadUncertainD( row, CS_FG_CLOCK_TIME, CS_FG_CLOCK_TIME_UNC );
		UncertainD fg_live_frac  = n2n::ReadUncertainD( row, CS_FG_LIVE_FRAC,  CS_FG_LIVE_FRAC_UNC );
		UncertainD bg_clock_time = n2n::ReadUncertainD( row, CS_BG_CLOCK_TIME, CS_BG_CLOCK_TIME_UNC );
		UncertainD bg_live_frac  = n2n::ReadUncertainD( row, CS_BG_LIVE_FRAC,  CS_BG_LIVE_FRAC_UNC );
		UncertainD protons = calculate::ProtonFlux( 
			fg_protons, fg_clock_time, fg_live_frac, bg_protons, bg_clock_time, bg_live_frac );
		n2n::WriteUncertainD( protons, &row, CS_PROTON_FLUX, CS_PROTON_FLUX_UNC );

		// Calculate the neutron flux
		UncertainD energy = n2n::ReadUncertainD( row, CS_NEUTRON_ENERGY, CS_NEUTRON_ENERGY_UNC );
		UncertainD sigma_np = calculate::CalcNPCrossSection( energy );
		UncertainD det_sang = calculate::SolidAngle( det_area, det_distance );
		UncertainD ch2_sang = calculate::SolidAngle( ch2_area, ch2_distance );
		UncertainD ch2_nH = calculate::CalcNumThickH_CH2( ch2_thickness );
		UncertainD neutrons = calculate::NeutronFlux( 
			protons, sigma_np, ch2_nH, ch2_sang, det_sang );
		n2n::WriteUncertainD( neutrons, &row, CS_NEUTRON_FLUX, CS_NEUTRON_FLUX_UNC );

		// Calculate cross sections
		UncertainD ch2_nC = calculate::CalcNumThickC_CH2( ch2_thickness );
		UncertainD ch2_sigma_n2n = calculate::N2NCrossSection( 
			ch2_decay, ch2_nC, ch2_sang, neutrons, fg_clock_time );
		n2n::WriteUncertainD( ch2_sigma_n2n, &row, CS_CH2_XSECT, CS_CH2_XSECT_UNC );

		UncertainD c12_sang = calculate::SolidAngle( c12_area, c12_distance );
		UncertainD c12_nC = calculate::CalcNumThickC_C12( c12_thickness );
		UncertainD c12_sigma_n2n = calculate::N2NCrossSection( 
			c12_decay, c12_nC, c12_sang, neutrons, fg_clock_time );
		n2n::WriteUncertainD( c12_sigma_n2n, &row, CS_C12_XSECT, CS_C12_XSECT_UNC );

		SetRow( i, row );
	}
}

} // namespace n2n
