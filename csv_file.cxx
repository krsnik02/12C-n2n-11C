/** @file csv_file.cxx
 * Copyright (C) 2013 Houghton College
 */

#include "csv_file.hxx"

namespace csv {

void CSVFile::Load( char const * filename )
{
	data_.clear();

	ifstream is( filename );
	string line;
	while ( getline( is, line ) )
	{
		data_.push_back( line );
	}
}

void CSVFile::Save( char const * filename ) const
{
	ofstream os( filename );
	for ( vector<string>::iterator i = data_.begin();
			i != data_.end(); ++i )
	{
		os << *i << endl;
	}
}

vector<string> CSVFile::GetRow( int row_number ) const
{
	return ParseRow( data_[row_number] );
}

void CSVFile::SetRow( int row_number, vector<string> const & row )
{
	data_[row_number] = FormatRow( row );
}

int CSVFile::NumRows() const
{
	return data_.size();
}


vector<string> CSVFile::ParseRow( string row_str )
{
	vector<string> row_vec;

	bool quoted = false;
	string elem = "";
	for ( int i = 0; i < row_str.length(); ++i )
	{
		if ( !quoted )
		{
			if ( row_str[i] == ',' )
				row_vec.push_back( elem ), elem = "";
			else if ( row_str[i] == '"' )
				quoted = true;
			else
				elem += row_str[i];
		}
		else
		{
			if ( row_str[i] == '"' && row_str[i+1] == '"' )
				elem += '"', ++i;
			else if ( row_str[i] == '"' )
				quoted = false;
			else
				elem += row_str[i];
		}
	}
	row_vec.push_back( elem );

	return row_vec;
}

string CSVFile::FormatRow( vector<string> row_vec )
{
	string row_str = "";

	for ( int i = 0; i < row_vec.size(); ++i )
	{
		if ( row_vec[i].find( '"' ) != string::npos )
		{
			row_str += '"';
			for ( int j = 0; j < row_vec[i].length(); ++j )
			{
				if ( row_vec[i][j] == '"' )
					row_str += "\"\"";
				else
					row_str += row_vec[i][j];
			}
			row_str += '"';
		}
		else if ( row_vec[i].find( ',' ) != string::npos )
		{
			row_str += '"';
			row_str += row_vec[i];
			row_str += '"';
		}
		else
			row_str += row_vec[i];

		if ( i != row_vec.size() - 1 )
			row_str += ',';
	}

	return row_str;
}

} // namespace csv
