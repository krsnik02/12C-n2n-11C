/** @file decay.H
 * Copyright (C) 2013 Houghton College
 */

namespace c12 {

TGraphErrors * CreateDecayCurve( TSystemFile * file );
TFitResultPtr FitDecayCurve( TGraphErrors * ge );
Double_t GetCountDecay( TFitResultPtr fr, Double_t trans_time,
	         	Double_t efficiency );
Double_t GetErrorDecay( TFitResultPtr fr, Double_t trans_time,
			Double_t efficiency );

} // namespace c12
