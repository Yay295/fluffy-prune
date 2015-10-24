#include <iostream>
#include <fstream>
#include "loadBMP.h"

const size_t MAX_DIMENSIONS = 100000;

bool loadBMP( const char * const filename, size_t & rows, size_t & columns, char * & image )
{
	std::ifstream in( filename, std::ios_base::binary );

	if ( !in )
	{
		std::cerr << "Unable to open file: " << filename << '\n';

		return false;
	}

	// process BMP file header
	bool fileFormatOK = false;

	if ( in.get() == 'B' && in.get() == 'M' )
	{
		in.ignore( 16 ); // Skip 4 fields we don't care about
		columns = readLong( in );
		rows = readLong( in );
		in.ignore( 2 ); // Skip one field
		short bitsPerPixel = readShort( in );
		in.ignore( 24 ); // Skip 6 more fields

		// check for valid image metadata
		if ( bitsPerPixel == 24 && rows <= MAX_DIMENSIONS && columns <= MAX_DIMENSIONS && !in.eof() )
			fileFormatOK = true;
	}

	if ( !fileFormatOK )
	{
		std::cerr << "Not a valid 24-bit bitmap file: " << filename << '\n';

		return false;
	}

	// allocate memory
	image = new char[rows*bytesPerRow( columns )];

	if ( image == nullptr )
	{
		std::cerr << "Unable to allocate memory for a " << rows << " x "
				  << columns  << " bitmap: " << filename << '\n';

		return false;
	}

	// read image data
	char * cPtr = image;
	for ( size_t i = 0; i < rows; ++i )
	{
		size_t j;

		for ( j = 0; j < columns; ++j )
		{
			*( cPtr + 2 ) = char( in.get() ); // Blue color value
			*( cPtr + 1 ) = char( in.get() ); // Green color value
			*cPtr = char( in.get() ); // Red color value
			cPtr += 3;
		}

		// j = number of bytes already read
		for ( j = 3 * columns; j < bytesPerRow( columns ); ++j )
		{
			in.ignore(); // ignore padding
			*( cPtr++ ) = 0;
		}
	}

	if ( in.eof() )
	{
		std::cerr << "Premature end of file: " << filename << '\n';

		delete[] image;

		return false;
	}

	return true;
}

short readShort( std::istream & in )
{
	const char lowByte = char( in.get() ); // Read the low order byte (little endian form)
	const char hiByte = char( in.get() ); // Read the high order byte

	// Pack together
	short ret = hiByte;
	ret <<= 8;
	ret |= lowByte;
	return ret;
}

int readLong( std::istream & in )
{
	char bytes[4];

	// Read bytes, low order to high order
	bytes[0] = char( in.get() );
	bytes[1] = char( in.get() );
	bytes[2] = char( in.get() );
	bytes[3] = char( in.get() );

	// Pack together
	char ret = bytes[3];
	ret <<= 8;
	ret |= bytes[2];
	ret <<= 8;
	ret |= bytes[1];
	ret <<= 8;
	ret |= bytes[0];
	return ret;
}

size_t bytesPerRow( size_t columns )
{
	const size_t mask = size_t( -4 ); // All bits==1 except the first two.
	return ( 3 * ++columns ) & mask;
}
