{
gROOT->ProcessLine(".L n2n/run_summary.cxx");
gROOT->ProcessLine(".L n2n/csv_file.cxx");
gROOT->ProcessLine(".L n2n/proton_good.cxx");
gROOT->ProcessLine(".L n2n/decay.cxx");

rs::RunSummary * sum = new rs::RunSummary();
sum->Load( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
sum->Update( "C:\\2012_12C(n,2n) Data\\ROOT Data" );
sum->Save( "C:\\2012_12C(n,2n) Data\\ROOT Data\\Run_Summary.csv" );
delete sum;
}
