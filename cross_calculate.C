{
gROOT->ProcessLine(".L n2n/cross_section.cxx");
gROOT->ProcessLine(".L n2n/csv_file.cxx");

cs::CrossSection * cross = new cs::CrossSection();
cross->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
cross->Calculate();
cross->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Cross_Sections.csv" );
delete cross;
}
