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

@todo Finish Program

@bug Only one third of the compressed image is shown.
@bug There are some pure black/white pixels in the compressed image.

@par Changelog:
	@verbatim
	Date               Change
	----------------   --------------------------------------------------------
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
	@endverbatim
******************************************************************************/

#ifdef __APPLE__
#include <GLUT/glut.h>
typedef unsigned int size_t;
#else
#include <GL/freeglut.h>
#endif

#include <iostream>
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
quadtree<pixel> * QUAD_IMAGE;
char * ORIGINAL_IMAGE, * COMPRESSED_IMAGE;
size_t IMAGE_WIDTH, IMAGE_HEIGHT;


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
	size_t i, j, height, width = 0;
	datatype curr, sum, min, max, avg = 0;
	height = y2 - y1;
	width = x2 - x1;

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
			if ( max - min > 0 && max - min > 2 * eps ) //worst case reached
				return 0;
		}
	}

	if ( max - min < 2 * eps )
		return avg;
	else
		return 0;
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

	if ( avg ) //if pass checkval pass the average value as data
		tree.insert( x1, y1, avg );

	else //if fails the checkval 
	{
		subdiv( tree, image, x1, x2 / 2, y1, y2 / 2, eps ); //tl
		subdiv( tree, image, x2 / 2, x2, y1, y2 / 2, eps ); //tr
		subdiv( tree, image, x1, x2 / 2, y2 / 2, y2, eps ); //bl
		subdiv( tree, image, x2 / 2, x2, y2, y2, eps ); //br
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
	if ( loadBMP( filename, IMAGE_WIDTH, IMAGE_HEIGHT, ORIGINAL_IMAGE ) )
	{
		QUAD_IMAGE = new quadtree<pixel>( IMAGE_WIDTH, IMAGE_HEIGHT );

		for ( size_t y = 0; y < IMAGE_HEIGHT; ++y )
		{
			for ( size_t x = 0; x < IMAGE_WIDTH; ++x )
			{
				const pixel temp = { ORIGINAL_IMAGE[IMAGE_WIDTH*y+3*x],
					                 ORIGINAL_IMAGE[IMAGE_WIDTH*y+3*x+1],
					                 ORIGINAL_IMAGE[IMAGE_WIDTH*y+3*x+2] };

				QUAD_IMAGE->insert( x, y, temp );
			}
		}


		std::cout << "Size of BMP image: "
				  << 3 * IMAGE_WIDTH * IMAGE_HEIGHT * sizeof( ORIGINAL_IMAGE ) << "\n\n";

		std::cout << "Before optimize() is called\n---------------------------------------\n"
			<< "quadtree size in bytes:       " << QUAD_IMAGE->size() << '\n'
			<< "number of quads in the tree:  " << QUAD_IMAGE->numQuads() << '\n'
			<< "number of points in the tree: " << QUAD_IMAGE->numPoints() << "\n\n";

		QUAD_IMAGE->optimize();

		std::cout << "After optimize() is called\n--------------------------------------\n"
			<< "quadtree size in bytes:       " << QUAD_IMAGE->size() << '\n'
			<< "number of quads in the tree:  " << QUAD_IMAGE->numQuads() << '\n'
			<< "number of points in the tree: " << QUAD_IMAGE->numPoints() << "\n\n";


		COMPRESSED_IMAGE = new char[IMAGE_WIDTH*IMAGE_HEIGHT*3];

		for ( size_t y = 0; y < IMAGE_HEIGHT; ++y )
		{
			for ( size_t x = 0; x < IMAGE_WIDTH; ++x )
			{
				const pixel temp = QUAD_IMAGE->getData( x, y );
				COMPRESSED_IMAGE[IMAGE_WIDTH*y+3*x] = temp.r;
				COMPRESSED_IMAGE[IMAGE_WIDTH*y+3*x+1] = temp.g;
				COMPRESSED_IMAGE[IMAGE_WIDTH*y+3*x+2] = temp.b;
			}
		}


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
@author ???

@par Description:
This function draws lines on top of the quadtree compressed image around the
location of each block as stored in the quadtree.
******************************************************************************/
void drawLines()
{
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
	test();

	if ( argc < 2 || argc > 3 )
	{
		printUsageInstructions();

		std::cin.ignore();

		return 0;
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
	
	delete[] QUAD_IMAGE;
	delete[] COMPRESSED_IMAGE;
}
