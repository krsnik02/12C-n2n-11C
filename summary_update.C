/** @file summary_update.C
 * Copyright (C) 2013 Houghton College
 *
 * Update the Run_Summary.csv file.
 *
 * @code
 * .x n2n/summary_update.C
 * @endcode
 */

/// @cond
{
gROOT->ProcessLine(".L n2n/RunSummary.cxx");
gROOT->ProcessLine(".L n2n/CSVFile.cxx");
gROOT->ProcessLine(".L n2n/proton.cxx");
gROOT->ProcessLine(".L n2n/decay.cxx");

n2n::RunSummary * sum = new n2n::RunSummary();
sum->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
sum->Update( "C:\\2012_12C(n,2n) Data\\ROOT Data" );
sum->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
delete sum;
}
/// @endcond
