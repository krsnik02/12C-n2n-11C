/**
 * @file n2n/Target.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_TARGET_INCL_
#define N2N_TARGET_INCL_

#include "Error.hxx"

namespace n2n {

/**
 * A target to be activated.
 */
struct Target
{
	double area;		///< Area of the target, @f$A@f$ (@f$\text{cm}^2@f$)
	double distance;	///< Distance of target from the tritium, @f$d@f$ (@f$\text{cm}@f$)
	double thickness;	///< Thickness of the target, @f$th@f$ (@f$\text{cm}@f$)
	Error<double> c11;	///< Number of activated @f${}^{11}\text{C}@f$ in target, @f$N_{C11}@f$

	virtual ~Target() {}

	/**
	 * Solid angle of target, @f$\Omega@f$ (@f$\text{sr}@f$)
	 * @f[\Omega=\frac{A}{d^2}@f]
	 */
	virtual double SolidAngle() const;

	/**
	 * Thickness of target, @f$N_C@f$ (@f$\frac{\text{C nuclei}}{\text{barn}}@f$)
	 * @f[N_C=\text{C nuclei per molecule}\cdot\frac{th\cdot\rho}{m}@f]
	 * where @f$\rho@f$ is the density and @f$m@f$ is the molecular mass.
	 */
	virtual double ThicknessC() const = 0;

	/** 
	 * Thickness of target, @f$N_H@f$ (@f$\frac{\text{H nuclei}}{\text{barn}}@f$)
	 * @f[N_H=\text{H nuclei per molecule}\cdot\frac{th\cdot\rho}{m}@f]
	 * where @f$\rho@f$ is the density and @f$m@f$ is the molecular mass.
	 */
	virtual double ThicknessH() const = 0;
};

struct CH2Target : Target
{
	double ThicknessC() const;
	double ThicknessH() const;	
};

struct C12Target : Target
{
	double ThicknessC() const;
	double ThicknessH() const;
};

} // namespace n2n

#endif
