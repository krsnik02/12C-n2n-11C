/** 
 * @file n2n/cross_loadsum.C
 * Copyright (C) 2013 Houghton College
 *
 * Copy values from the Run_Summary.csv file into the Cross_Sections.csv file.
 *
 * @code
 * .x n2n/cross_loadsum.C
 * @endcode
 */

/// @cond
{
gROOT->ProcessLine(".L n2n/CrossSection_loadsum.cxx");
gROOT->ProcessLine(".L n2n/RunSummary.cxx");
gROOT->ProcessLine(".L n2n/CSVFile.cxx");

n2n::CrossSection * cross = new n2n::CrossSection();
cross->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );

n2n::RunSummary * sum = new n2n::RunSummary();
sum->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
cross->LoadSummary( sum );
delete sum;

cross->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
delete cross;
}
/// @endcond
