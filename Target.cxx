/**
 * @file n2n/Target.cxx
 */

#include "Target.hxx"

namespace n2n {

double Target::SolidAngle() const
{
	return area / (distance * distance);
}

double CH2Target::ThicknessC() const
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass = 2 * mass_H + mass_C;
	double density = 0.89;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return thickness * density / (mass * 1.6605389);
}

double CH2Target::ThicknessH() const
{
	double mass_H = 1.007825;	// u
	double mass_C = 12;		// u
	double mass = 2 * mass_H + mass_C;
	double density = 0.89;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return 2 * thickness * density / (mass * 1.6605389);
}

double C12Target::ThicknessC() const
{
	double mass = 12;		// u
	double density = 2.276;		// g/cm^3

	// 1 u = 1.6605389e-24 g
	// 1 barn = 1e-24 cm^2
	return thickness * density / (mass * 1.6605389);
}

double C12Target::ThicknessH() const
{
	return 0;
}

} // namespace n2n
