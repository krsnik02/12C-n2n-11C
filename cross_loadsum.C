{
gROOT->ProcessLine(".L n2n/cross_section.cxx");
gROOT->ProcessLine(".L n2n/run_summary.cxx");
gROOT->ProcessLine(".L n2n/csv_file.cxx");

cs::CrossSection * cross = new cs::CrossSection();
rs::RunSummary * sum = new rs::RunSummary();
cross->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
sum->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
cross->LoadSummary( sum );
cross->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
delete sum;
delete cross;
}
