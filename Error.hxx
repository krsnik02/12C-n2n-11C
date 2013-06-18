/**
 * @file n2n/Error.hxx
 * Copyright (C) 2013 Houghton College
 */

#ifndef N2N_ERROR_INCL_
#define N2N_ERROR_INCL_

namespace n2n {

/**
 * A value and its uncertainty.
 */
template <typename T>
struct Error
{
	T value;	///< The value
	T error;	///< The uncertainty in the value
};

} // namespace n2n

#endif
