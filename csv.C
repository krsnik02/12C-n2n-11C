/** @file csv.C
 * Copyright (C) 2013 Houghton College
 *
 * Contains functions for handling CSV files.
 */

#include "csv.H"

namespace c12 {

/**
 * Parse a row from a CSV file into a std::vector<std::string> object.
 *
 * @param csv_row The row to parse
 * 
 * @return The values in the row as a std::vector<std::string>.
 */
std::vector<std::string> CSVParseRow( std::string csv_row )
{
 	std::vector<std::string> row;
	bool quoted = false;

	std::string elem = "";
	for ( int i = 0; i < csv_row.length(); ++i )
	{
		if ( !quoted )
		{
			if ( csv_row[i] == ',' )
				row.push_back( elem ), elem = "";
			else if ( csv_row[i] == '"' )
				quoted = true;
			else
				elem += csv_row[i];
		}
		else
		{
			if ( csv_row[i] == '"' && csv_row[i+1] == '"' )
				elem += '"', ++i;
			else if ( csv_row[i] == '"' )
				quoted = false;
			else
				elem += csv_row[i];
		}
	}

	return row;
}



/**
 * Formats a std::vector<std::string> object as escaped CSV.
 *
 * @param row The row to format
 *
 * @return A well-formed CSV string
 */
std::string CSVFormatRow( std::vector<std::string> row )
{
	std::string csv_row = "";

	for ( int i = 0; i < row.size(); ++i )
	{
		if ( row[i].find( '"' ) != std::string::npos )
		{
			csv_row += '"';
			for ( int j = 0; j < row[i].length(); ++j )
			{
				if ( row[i][j] == '"' )
					csv_row += "\"\"";
				else
					csv_row += row[i][j];
			}
			csv_row += '"';
		}
		else if ( row[i].find( ',' ) != std::string::npos )
		{
			csv_row += '"';
			csv_row += row[i];
			csv_row += '"';
		}
		else
			csv_row += row[i];

		if ( i != row.size() - 1 )
			csv_row += ',';
	}
	return csv_row;
}

} // namespace c12
