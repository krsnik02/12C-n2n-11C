/** @file csv.H
 * Copyright (C) 2013 Houghton College
 */

#include <vector>

namespace c12 {

std::vector<std::string> CSVParseRow( std::string csv_row );
std::string CSVFormatRow( std::vector<std::string> row );

} // namespace c12
