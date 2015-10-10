#include "quadtree.h"


template<class datatype>
quadtree<datatype>::point::point() {}

template<class datatype>
quadtree<datatype>::point::point( const size_t X, const size_t Y, const datatype & D )
	: x( X ), y( Y ), data( D ) {}

template<class datatype>
datatype quadtree<datatype>::point::getData() const { return data; }


template<class datatype>
quadtree<datatype>::quad::quad( const size_t X, const size_t Y, const size_t W, const size_t H )
	: isPoint( false ), x( X ), y( Y ), width( W ), height( H ) {}

template<class datatype>
bool quadtree<datatype>::quad::contains( const size_t X, const size_t Y )
{
	return ( x <= X && X <= ( x + width ) && y <= Y && Y <= ( y + height ) );
}

template<class datatype>
void quadtree<datatype>::quad::insert( const point * const P )
{
	insert( P.x, P.y, P.data )
}

template<class datatype>
void quadtree<datatype>::quad::insert( const size_t X, const size_t Y, const datatype & D )
{
	// Top
	if ( Y - y < height / 2 )
	{
		// Left
		if ( X - x < width / 2 )
		{
			if ( tl == nullptr ) tl = new point( X, Y, D );

			else if ( tl->isPoint )
			{
				const point * const temp = tl;
				tl = new quad( x, y, width / 2, height / 2 );
				tl->data = data;

				tl->insert( temp );
				tl->insert( X, Y, D );
			}

			else if ( D != tl->data ) tl->insert( X, Y, D );
		}

		// Right
		else
		{
			if ( tr == nullptr ) tr = new point( X, Y, D );

			else if ( tr->isPoint )
			{
				const point * const temp = tr;
				tr = new quad( x + width / 2, y, width - width / 2, height / 2 );
				tr->data = data;

				tr->insert( temp );
				tr->insert( X, Y, D );
			}

			else if ( D != tr->data ) tr->insert( X, Y, D );
		}
	}

	// Bottom
	else
	{
		// Left
		if ( X - x < width / 2 )
		{
			if ( bl == nullptr ) bl = new point( X, Y, D );

			else if ( bl->isPoint )
			{
				const point * const temp = bl;
				bl = new quad( x, y + height / 2, width, height - height / 2 );
				bl->data = data;

				bl->insert( temp );
				bl->insert( X, Y, D );
			}

			else if ( D != bl->data ) bl->insert( X, Y, D );
		}

		// Right
		else
		{
			if ( br == nullptr ) br = new point( X, Y, D );

			else if ( br->isPoint )
			{
				const point * const temp = br;
				br = new quad( x + width / 2, y + height / 2, width - width / 2, height - height / 2 );
				br->data = data;

				br->insert( temp );
				br->insert( X, Y, D );
			}

			else if ( D != br->data ) br->insert( X, Y, D );
		}
	}
}



/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
Constructor for the quadtree class.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quadtree( const size_t W, const size_t H ) : width( W ), height( H ) {}

/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
Destructor for the quadtree class.
*****************************************************************************/
template <class datatype>
quadtree<datatype>::~quadtree()
{
	clear();

	delete root;
}

/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
Insert method for the quadtree class.
******************************************************************************/
template <class datatype>
bool quadtree<datatype>::insert( const size_t X, const size_t Y, const datatype & D )
{
	if ( X < width && Y < height )
	{
		if ( root != nullptr && root->data != D ) root->insert( X, Y, D );

		else
		{
			root = new quad( 0, 0, width, height );

			quad->data = D;
		}

		return true;
	}

	else return false;
}

/******************************************************************************
@author Jeremy Gamet

@par Description:
Insert method for the quadtree class.
*****************************************************************************/
template <class datatype>
void quadtree<datatype>::subdiv()
{
	//TBD
}

/******************************************************************************
@author Jeremy Gamet

@par Description:
Insert method for the quadtree class.
*****************************************************************************/
template <class datatype>
void quadtree<datatype>::clear()
{
	//TBD
}
