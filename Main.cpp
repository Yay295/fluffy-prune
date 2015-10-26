/**************************************************************************//**
@file

@mainpage Program 2: Quadtree Encoding

@section course_section Course Information

@author Jeremy Gamet and John Colton

@date Monday October 26, 2015

@par Professor: Paul Hinker

@par Course: CSC 300 - Fall 2015

@section program_section Program Information

@details This program reads an image from a .bmp file, stores it in a quadtree,
         and displays the original image and the quadtree image side-by-side.
		 The quadtree image may be compressed by a given quality factor.

		 This program supports images of any size.

@section compile_section Compiling and Usage

@par Compiling Instructions:

@par Usage:

@section todo_bugs_changelog Todo, Bugs, and Changelog

@todo Fix Bug(s)

@bug quadtree::optimize() is wrong, somewhere.
@bug checkval() and subdiv() have errors.

@par Changelog:
	@verbatim
	Date               Change
	----------------   --------------------------------------------------------
	Prehistory         Created main.cpp and quadtree.cpp.
	October 21, 2015   Added lots of documentation.
	October 22, 2015   Added optimize() functions to quadtree and quad.
	                   Improved quad::insert(point*) to handle nullptr's.

					   Moved quadtree::subdiv() to Main.cpp as checkval() and
					   subdiv().
	October 23, 2015   Finished quad::optimize(). Improved point::contains().
	                   Added quad::getData() and quadtree::getData().

					   Fixed bugs in quad::optimize(), quad::contains(), and
					   quad::insert(). Changed quad constructor to also take
					   some data. Changed point and quad structs to use the
					   points' variables instead of having separate variables
					   for each.

					   Added code to load a BMP file and loadImage() function.
					   Fixed Doxygen tags. Finished drawImages(). Worked on
					   loadImage().
	October 26, 2015   Removed Apple specific code and functions checkval() and
	                   subdiv(). Moved QUAD_IMAGE global quatree pointer to
					   loadImage() as a non-pointer quadtree. Fixed bugs
					   causing only 1/3 of the compressed image being shown and
					   bug causing some pure black/white pixels in the
					   compressed image. Found that there is a bug in
					   quadtree::optimize(), though I don't yet know where.

					   Added code to get the quality value from the command
					   line if it is set and make sure it is within limits.

					   Added getDimensions() to the quadtree class and its
					   member structs (quad and point). Wrote drawLines().
					   Exhumed checkval() and subdiv(). Made + and - keys add
					   and subtract 5 to/from the quality level.
	@endverbatim
******************************************************************************/

#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include "loadBMP.cpp"
#include "quadtree.cpp"


// A pixel struct for holding pixel data.
struct pixel
{
	char r, g, b;

	bool operator == ( const pixel & rhs ) const
	{
		return ( r == rhs.r && g == rhs.g && b == rhs.b );
	}

	bool operator != ( const pixel & rhs ) const
	{
		return ( r != rhs.r || g != rhs.g || b != rhs.b );
	}
};


bool DRAW_LINES = false;
double QUALITY = 100;
size_t IMAGE_WIDTH, IMAGE_HEIGHT;
char * ORIGINAL_IMAGE, * COMPRESSED_IMAGE;
std::vector<size_t> DIMENSIONS;


/**************************************************************************//**
@author John Colton

@par Description:
This function prints the required command line arguments to the console.
*****************************************************************************/
void printUsageInstructions()
{
	std::cout <<
		"This program requires one or two arguments be passed to it.\n"
		"ex: <program name> <argument 1> [argument 2]\n\n"

		"Argument 1 :\n"
		"The first argument must be a .bmp image somewhere on your computer. This image\n"
		"is the image that will be used by the program.\n\n"

		"Argument 2 ( optional ) :\n"
		"The second argument must be a number (integer or real) between 0 and 100\n"
		"specifying the quality factor used by the program. A value of 100 is lossless\n"
		"quality, and a value of 0 will compress the image to a single color. If this\n"
		"argument is not specified, it will be set to 100.\n\n"
		
		"Press 'Enter' to exit the program.";
}

/**************************************************************************//**
@author Jeremy Gamet

@par Description:
Tests a range of values for deviation from a calculated mean.
Note: This function is called by subdiv().

@param[in] image - 2D matrix containing pixel values to be checked.
@param[in] x1    - Top left x coord of area to be checked.
@param[in] x2    - Bottom right x coord of area to be checked.
@param[in] y1    - Top left y coord of area to be checked.
@param[in] y2    - Bottom right y coord of area to be checked.
@param[in] eps   - An epsilon value to determine deviation from a mean value in
                   a range of pixel values.
******************************************************************************/
template<class datatype>
datatype checkval( const datatype ** const image,
	const size_t x1, const size_t x2, const size_t y1,
	const size_t y2, const datatype & eps )
{
	size_t i, j, y2 - y1, width = x2 - x1;
	datatype curr, sum, min, max, avg = 0;

	for ( i = x1; i < x2; ++i )
	{
		for ( j = y1; j < y2; ++j )
		{
			curr = image[i][j];
			sum += curr;
			if ( max < curr )
				max = curr;
			if ( min > curr )
				min = curr;
			if ( i == height - 1 && j == width - 1 )
				avg = sum / ( height * width );
			if ( max - min > 0 && max - min > 2 * eps ) // worst case reached
				return 0;
		}
	}

	if ( max - min < 2 * eps )
		return avg;
	else
		return (datatype) -1;
}

/**************************************************************************//**
@author Jeremy Gamet

@par Description:
Inserts an image into the quadtree.
Note: This function calls on checkval().

@param[in,out] tree - The structure being inserted into.
@param[in] image    - The image being inserted.
@param[in] x1       - Top left x coord of area to be inserted.
@param[in] x2       - Bottom right x coord of area to be inserted.
@param[in] y1       - Top left y coord of area to be inserted.
@param[in] y2       - Bottom right y coord of area to be inserted.
@param[in] eps      - An epsilon value to determine deviation from a mean value
                      in a range of pixel values.
******************************************************************************/
template<class datatype>
void subdiv( quadtree<datatype> & tree, const datatype ** const image,
	const size_t x1, const size_t x2, const size_t y1, const size_t y2,
	const datatype & eps )
{
	const datatype avg = checkval( image, x1, x2, y1, y2, eps );

	if ( avg != (datatype) -1 ) // if pass checkval pass the average value as data
		tree.insert( x1, y1, avg );

	else // if fails the checkval 
	{
		subdiv( tree, image, x1, x2 / 2, y1, y2 / 2, eps ); // tl
		subdiv( tree, image, x2 / 2, x2, y1, y2 / 2, eps ); // tr
		subdiv( tree, image, x1, x2 / 2, y2 / 2, y2, eps ); // bl
		subdiv( tree, image, x2 / 2, x2, y2 / 2, y2, eps ); // br
	}
}

/**************************************************************************//**
@author John Colton

@par Description:
This function loads the given bmp image into ORIGINAL_IMAGE, and then
compresses it into QUAD_IMAGE. It then reads the compressed image from
QUAD_IMAGE into COMPRESSED_IMAGE for easier glut display.

@param[in] filename - The name of the bmp file to read.

@returns true  - The image was successfully loaded.
@returns false - The image was not loaded.
*****************************************************************************/
bool loadImage( const char * const filename )
{
	if ( filename == nullptr || loadBMP( filename, IMAGE_HEIGHT, IMAGE_WIDTH, ORIGINAL_IMAGE ) )
	{
		quadtree<pixel> quadimage( IMAGE_WIDTH, IMAGE_HEIGHT );
		
		for ( size_t y = 0; y < IMAGE_HEIGHT; ++y )
		{
			for ( size_t x = 0; x < IMAGE_WIDTH; ++x )
			{
				const pixel temp = { ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x],
					                 ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x+1],
					                 ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x+2] };

				// Posterized
				const pixel test = { char( ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x] * QUALITY / 100 + 0.5 ) * ( 100 / QUALITY ),
					                 char( ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x+1] * QUALITY / 100 + 0.5 ) * ( 100 / QUALITY ),
					                 char( ORIGINAL_IMAGE[3*IMAGE_WIDTH*y+3*x+2] * QUALITY / 100 + 0.5 ) * ( 100 / QUALITY ) };

				quadimage.insert( x, y, test );
			}
		}


		std::cout << "Quality Setting: " << QUALITY << "\n\n";

		std::cout << "Size of BMP image: "
				  << 3 * IMAGE_WIDTH * IMAGE_HEIGHT * sizeof( ORIGINAL_IMAGE ) << "\n\n";

		std::cout << "Before optimize() is called\n---------------------------------------\n"
			<< "quadtree size in bytes:       " << quadimage.size() << '\n'
			<< "number of quads in the tree:  " << quadimage.numQuads() << '\n'
			<< "number of points in the tree: " << quadimage.numPoints() << "\n\n";

		quadimage.optimize();

		std::cout << "After optimize() is called\n--------------------------------------\n"
			<< "quadtree size in bytes:       " << quadimage.size() << '\n'
			<< "number of quads in the tree:  " << quadimage.numQuads() << '\n'
			<< "number of points in the tree: " << quadimage.numPoints() << "\n\n";


		COMPRESSED_IMAGE = new char[IMAGE_WIDTH*IMAGE_HEIGHT*3];

		for ( size_t y = 0; y < IMAGE_HEIGHT; ++y )
		{
			for ( size_t x = 0; x < IMAGE_WIDTH; ++x )
			{
				const pixel temp = quadimage.getData( x, y );
				COMPRESSED_IMAGE[3*IMAGE_WIDTH*y+3*x] = temp.r;
				COMPRESSED_IMAGE[3*IMAGE_WIDTH*y+3*x+1] = temp.g;
				COMPRESSED_IMAGE[3*IMAGE_WIDTH*y+3*x+2] = temp.b;
			}
		}


		DIMENSIONS.clear();
		quadimage.getDimensions( DIMENSIONS );


		/*/ Print Incorrect Pixels
		for ( size_t i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT * 3; ++i )
		{
			if ( ORIGINAL_IMAGE[i] != COMPRESSED_IMAGE[i] )
			{
				COMPRESSED_IMAGE[i] = 0;
				//std::cout << i / ( 512 * 3 ) << ' ' << ( i % 512 ) / 3 << " - " << int( ORIGINAL_IMAGE[i] ) << ' ' << int( COMPRESSED_IMAGE[i] ) << '\n';
				//break;
			}

			else COMPRESSED_IMAGE[i] = 255;
		}//*/


		return true;
	}

	else return false;
}

/**************************************************************************//**
@author John Colton

@par Description:
This function draws the original and quadtree compressed images to the screen.
*****************************************************************************/
void drawImages()
{
	glRasterPos2i( 0, 0 );

	glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, ORIGINAL_IMAGE );

	glRasterPos2i( IMAGE_WIDTH + 10, 0 );

	glDrawPixels( IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, COMPRESSED_IMAGE );
}

/**************************************************************************//**
@author John Colton

@par Description:
This function draws lines on top of the quadtree compressed image around the
location of each block as stored in the quadtree.
******************************************************************************/
void drawLines()
{
	const size_t size = DIMENSIONS.size();

	for ( size_t i = 0; i < size; i += 4 )
	{
		const size_t x = DIMENSIONS[i] + IMAGE_WIDTH + 10,
			         y = DIMENSIONS[i+1],
			     width = DIMENSIONS[i+2],
			    height = DIMENSIONS[i+3];

		// Draw Top Border
		glBegin( GL_LINES );
		glVertex2d( x, y );
		glVertex2d( x + width, y );
		glEnd();

		// Draw Bottom Border
		glBegin( GL_LINES );
		glVertex2d( x, y + height );
		glVertex2d( x + width, y + height );
		glEnd();

		// Draw Left Border
		glBegin( GL_LINES );
		glVertex2d( x, y );
		glVertex2d( x, y + height );
		glEnd();

		// Draw Right Border
		glBegin( GL_LINES );
		glVertex2d( x + width, y );
		glVertex2d( x + width, y + height );
		glEnd();
	}
}

/**************************************************************************//**
@author John Colton

@par Description:
This function calls the various functions required to draw/redraw the screen.
*****************************************************************************/
void display()
{
	// Clear Window
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();
	glOrtho( 0, glutGet( GLUT_WINDOW_WIDTH ), 0, glutGet( GLUT_WINDOW_HEIGHT ), -1, 1 );

	// Draw Images
	drawImages();

	// Draw Quadtree Lines
	if ( DRAW_LINES ) drawLines();

	glFinish();
}

/**************************************************************************//**
@author John Colton

@par Description:
This function controls how the program reacts to different key presses. The
x and y variables are not used.

@param[in]	key - An unsigned char indicating which key was pressed.
@param[in]	x - The x coordinate of the mouse cursor when the key was pressed.
@param[in]	y - The y coordinate of the mouse cursor when the key was pressed.
******************************************************************************/
void onkeypress( const unsigned char key, const int x, const int y )
{
	// If the spacebar is pressed.
	if ( key == ' ' )
	{
		// Toggle quadtree quad border line visibility.
		DRAW_LINES = !DRAW_LINES;

		// Redraw screen.
		display();
	}

	else if ( key == '+' || key == '-' )
	{
		QUALITY /= 20;
		QUALITY *= 20;

		if ( key == '+' ) QUALITY += 5;
		else if ( key == '-' ) QUALITY -= 5;

		if ( QUALITY < 0 ) QUALITY = 0;
		if ( QUALITY > 100 ) QUALITY = 100;

		loadImage( nullptr );

		display();
	}
}

void test()
{
	quadtree<int> test( 500, 500 );

	test.insert( 0, 0, 0 );
	test.insert( 0, 0, 1 );
	test.insert( 0, 1, 1 );
	test.insert( 0, 2, 1 );
	test.insert( 1, 0, 1 );
	test.insert( 1, 1, 1 );
	test.insert( 1, 2, 1 );
	test.insert( 2, 0, 1 );
	test.insert( 2, 1, 1 );
	test.insert( 2, 2, 1 );

	std::cout << "quadtree size in bytes:       " << test.size() << '\n'
			  << "number of quads in the tree:  " << test.numQuads() << '\n'
			  << "number of points in the tree: " << test.numPoints() << "\n\n";

	test.optimize();

	std::cout << "quadtree size in bytes:       " << test.size() << '\n'
			  << "number of quads in the tree:  " << test.numQuads() << '\n'
			  << "number of points in the tree: " << test.numPoints() << "\n\n";
}

/**************************************************************************//**
@author John Colton

@par Description:
This is the beginning of the program. All necessary initialization
functions are called here.
******************************************************************************/
int main( int argc, char * argv[] )
{
	if ( argc < 2 || argc > 3 )
	{
		printUsageInstructions();

		std::cin.ignore();

		return 0;
	}

	if ( argc == 3 ) // If a quality value has been specified.
	{
		QUALITY = atof( argv[2] );

		if ( QUALITY < 0.1 ) QUALITY = 0.1;

		else if ( QUALITY > 100 ) QUALITY = 100;
	}

	if ( !loadImage( argv[1] ) ) return 0;

	// Glut Init Functions
	glutInitWindowSize( 600, 600 );
	glutInitWindowPosition( 0, 0 );
	glutInitDisplayMode( GLUT_RGB );
	glutInit( &argc, argv );

	glutCreateWindow( "Quadtree Image Encoding" );

	// GL Init Functions
	glClearColor( 0, 0, 0, 0 ); // Black
	glMatrixMode( GL_PROJECTION );

	// Glut Event Functions
	glutDisplayFunc( display );
	glutKeyboardFunc( onkeypress );

	glutMainLoop();

	delete[] COMPRESSED_IMAGE;
}
