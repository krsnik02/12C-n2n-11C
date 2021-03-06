/** 
 * @file n2n/cross_calculate.C
 * Copyright (C) 2013 Houghton College
 *
 * Calculate cross sections from values in the Cross_Sections.csv file.
 *
 * @code
 * .x n2n/cross_calculate.C
 * @endcode
 */

/// @cond
{
gROOT->ProcessLine(".L n2n/CrossSection_calculate.cxx");
gROOT->ProcessLine(".L n2n/Uncertain.cxx");
gROOT->ProcessLine(".L n2n/CSVFile.cxx");

n2n::CrossSection * cross = new n2n::CrossSection();
cross->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
cross->Calculate();
cross->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
delete cross;
}
/// @endcond
