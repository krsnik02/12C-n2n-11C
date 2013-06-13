/** @file csv_file.hxx
 * Copyright (C) 2013 Houghton College
 */

#include <vector>

namespace csv {

/**
 * Provides access to CSV formatted data.
 */
struct CSVFile
{
	public:
		/**
		 * Load a file containing csv foramtted data.
		 * @param filename The file to load.
		 */
		void Load( char const * filename );

		/**
		 * Save a file containing csv formatted data.
		 * @param filename The file to save to.
		 */
		void Save( char const * filename ) const;

		/**
		 * Retrieve a row from the file.
		 * @param row_number The row to retrieve.
		 * @return The requested row.
		 */
		vector<string> GetRow( int row_number ) const;

		/**
		 * Overwrite a row in the file.
		 * @param row_number The row to overwrite.
		 * @param row The values to write.
		 */
		void SetRow( int row_number, vector<string> const & row );

		/**
		 * Get the number of rows in the file.
		 */
		int NumRows() const;

	private:
		vector<string> data_;

		/**
		 * Parse a string containing a csv formatted row.
		 * @param row_str The string to parse.
		 * @return A vector of values in the string.
		 */
		static vector<string> ParseRow( string row_str );

		/**
		 * Format values into a csv formatted row.
		 * @param row_vec The values to format.
		 * @return A string containing those values.
		 */
		static string FormatRow( vector<string> row_vec );
};

} // namespace csv
