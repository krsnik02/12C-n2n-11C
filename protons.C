/** @file protons.C
 * Copyright (C) 2013 Houghton College
 *
 * Contains functions for determining proton counts
 */

#include "protons.H"
#include <vector>

namespace c12 {

/**
 * Parse a tab-separated file containing data from a E-dE detector into
 * a TGraph2DErrors object.
 *
 * @param file the TSystemFile to read. This input file should be the
 * output of the MPA4 software set to output CSV and contain data from
 * a E-dE detector.
 *
 * @return A TGraph2D object containing the proton distribution.
 */
TNtuple * CreateProtonDistribution( TSystemFile * file )
{
	// Open the file for reading
	std::ifstream ifs( gSystem->ConcatFileName(
				file->GetTitle(), file->GetName() ),
			   std::ios::in );

	// Skip to the [DATA] section of the file
	std::string line;
	while ( std::getline( ifs, line ) )
	{
		if ( line == "[DATA]" )
			break;
	}

	// Read the data from the file.
	TNtuple * ntup = new TNtuple( "", "", "x:y:counts" );
	float x, y, count;
	while ( ifs >> x >> y >> count )
	{
		ntup->Fill( x, y, count );
	}
	return ntup;
}

/**
 * Select the region of interest from a proton distribution.
 *
 * @param ntup The proton distribution TNtuple returned by
 *   @ref CreateProtonDistribution
 * @param xmin The smallest x-value in the region
 * @param xmax The largest x-value in the region
 * @param ymin The smallest y-value in the region
 * @param ymax The largest y-value in the region
 *
 * @return A TH2F containing the region of interest
 */
TH2F * SelectProtonROI( TNtuple * ntup, Float_t xmin, Float_t xmax,
		        Float_t ymin, Float_t ymax )
{
	TCut cutx = TString::Format( "(x>%f)&&(x<%f)", xmin, xmax );
	TCut cuty = TString::Format( "(y>%f)&&(y<%f)", ymin, ymax );
	TCut roi = cutx && cuty;

	ntup->Draw( "y:x >>hroi", "counts" * roi, "goff" );
	return (TH2F*)gROOT->FindObject( "hroi" );
}

} // namespace c12
