/** @file utility.C
 * Copyright (C) 2013 Houghton College
 *
 * Utility function.
 */

namespace c11 {

/** 
 * Traverse the filesystem to locate the specified run data.
 *
 * @param dir A TSystemDirectory object in which to search. This must be a flat
 * directory as this method cannot properly traverse a directory tree.
 * @param run_number The run number to search for.
 * @param target The type of target to search for.
 *
 * @return The TSystemFile object which was found, or the default TSystemFile.
 */
TSystemFile * LocateFile( 
		TSystemDirectory * dir,
		char const * run_number,
		char const * target )
{
	TList * files = dir->GetListOfFiles();
	for ( TIter & i = TIter( files ).Begin(); i != TIter::End(); ++i )
	{
		TString * name = new TString( (*i)->GetName() );
		if ( name->Contains( TPRegexp( "\.[cC][sS][vV]$" ) ) 
			&& name->Contains( run_number )
			&& name->Contains( target, TString::kIgnoreCase ) )
			return (TSystemFile *)*i;
	}
	return NULL;
}

/**
 * Attempt to fit every run in the run summary file, and write the results back.
 *
 * @param run_summary The Run_Summary.csv file.
 * @param dir The directory in which to look for the run data.
 */
void FitAllRuns( TSystemFile * run_summary, TSystemDirectory * dir )
{
}


} //namespace c11
