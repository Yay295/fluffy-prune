/******************************************************************************
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

@bug Probably

@par Changelog:
	@verbatim
	Date               Change
	----------------   --------------------------------------------------------
	October 21, 2015   Added lots of documentation.
	October 22, 2015   Added optimize() functions to quadtree and quad.
	                   Improved quad::insert(point*) to handle nullptr's.
	October 23, 2015   Finished quad::optimize(). Improved point::contains().
	                   Added quad::getData() and quadtree::getData.
	@endverbatim
******************************************************************************/

// Need to implement operator != for our datatype. != operator will check the
// global QUALITY to determine if they're 'not equal'.

#include <GL/freeglut.h>
#endif
#include <iostream>
#include "quadtree.cpp"

bool DRAW_LINES = false;
double QUALITY = 100;
quadtree<int> QUAD_IMAGE ( 500, 500 );


/******************************************************************************
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

/******************************************************************************
@author ???

@par Description:
This function draws the original and quadtree compressed images to the screen.
*****************************************************************************/
void drawImages()
{
}

/******************************************************************************
@author ???

@par Description:
This function draws lines on top of the quadtree compressed image around the
location of each block as stored in the quadtree.
******************************************************************************/
void drawLines()
{
}

/******************************************************************************
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

/******************************************************************************
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


/******************************************************************************
@author Jeremy Gamet

@par Description:
Inserts an image into the quadtree.
Note: This function calls on checkval().

@param[in,out] tree - The structure being inserted into.
@param[in] image - The image being inserted.
@param[in] x1 - Top left x coord of area to be inserted.
@param[in] x2 - Bottom right x coord of area to be inserted.
@param[in] y1 - Top left y coord of area to be inserted.
@param[in] y2 - Bottom right y coord of area to be inserted.
@param[in] eps   - An epsilon value to determine deviation from
				    a mean value in a range of pixel values.

******************************************************************************/
template<class datatype>
void subdiv( quadtree<datatype> & tree, const datatype ** const image, 
const size_t x1, const size_t x2, const size_t y1, const size_t y2, 
const datatype & eps )
{
	datatype avg = checkval(image, x1, x2, y1, y2, eps);

	if ( avg ) //if pass checkval pass the average value as data
	{
		tree.insert(x1,y1,avg);
		return;
	}

	else //if fails the checkval 
	{
		subdiv(tree, image, x1, x2/2, y1, y2/2, eps); //tl
		subdiv(tree, image, x2/2, x2, y1, y2/2, eps); //tr
		subdiv(tree, image, x1, x2/2, y2/2, y2, eps); //bl
		subdiv(tree, image, x2/2, x2, y2, y2, eps); //br
	}
}

/******************************************************************************
@author Jeremy Gamet

@par Description:
Tests a range of values for deviation from a calculated mean. 
Note: This function is called by subdiv().

@param[in] image - 2D matrix containing pixel values to be checked.
@param[in] x1 - Top left x coord of area to be checked.
@param[in] x2 - Bottom right x coord of area to be checked.
@param[in] y1 - Top left y coord of area to be checked.
@param[in] y2 - Bottom right y coord of area to be checked.
@param[in] eps   - An epsilon value to determine deviation from
				    a mean value in a range of pixel values.

******************************************************************************/
template<class datatype>
datatype checkval(const datatype ** const image, 
const size_t x1, const size_t x2, const size_t y1,
const size_t y2, const datatype & eps)
{
	size_t i, j, height, width = 0;
	datatype curr, sum, min, max, avg = 0;
	height = y2-y1;
	width = x2-x1;

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
			if (max-min > 0 && max-min > 2 * eps) //worst case reached
				return 0;
		}
	}

	if (max-min < 2*eps)
		return avg;
	else 
		return 0;
}


/******************************************************************************
@author John Colton

@par Description:
This is the beginning of the program. All necessary initialization
functions are called here.
******************************************************************************/
int main( int argc, char * argv[] )
{
	QUAD_IMAGE.insert( 0, 0, 0 );
	QUAD_IMAGE.insert( 0, 0, 1 );
	QUAD_IMAGE.insert( 0, 1, 1 );
	QUAD_IMAGE.insert( 1, 0, 1 );
	QUAD_IMAGE.insert( 1, 1, 1 );

	QUAD_IMAGE.optimize();

	std::cout << "quadtree size in bytes:       " << QUAD_IMAGE.size() << '\n'
			  << "number of quads in the tree:  " << QUAD_IMAGE.numQuads() << '\n'
			  << "number of points in the tree: " << QUAD_IMAGE.numPoints() << "\n\n";

	if ( argc < 2 || argc > 3 )
	{
		printUsageInstructions();

		std::cin.ignore();

		return 0;
	}

	// Glut Init Functions
	glutInitWindowSize( 600, 600 );
	glutInitWindowPosition( 0, 0 );
	glutInitDisplayMode( GLUT_ALPHA );
	glutInit( &argc, argv );

	glutCreateWindow( "Quadtree Image Encoding" );

	// GL Init Functions
	glClearColor( 1, 1, 1, 1 );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	glMatrixMode( GL_PROJECTION );

	// Glut Event Functions
	glutDisplayFunc( display );
	glutKeyboardFunc( onkeypress );

	glutMainLoop();
}
