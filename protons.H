/** @file protons.H
 * Copyright (C) 2013 Houghton College
 */

namespace c12 {

TNtuple * CreateProtonDistribution( TSystemFile * file );
TH2F * SelectProtonROI( TNtuple * ntup, Float_t xmin, Float_t xmax, 
		        Float_t ymin, Float_t ymax );

} // namespace c12
