#ifndef QUADTREE_H
#define QUADTREE_H


/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
This class is our implementation of a quadtree.
******************************************************************************/
template<class datatype>
class quadtree
{
	private:

	/**************************************************************************
	@author Jeremy Gamet and John Colton

	@par Description:
	This struct holds the data in our quadtree.
	**************************************************************************/
	struct point
	{
		point();
		point( const size_t X, const size_t Y, const datatype & D );
		datatype getData() const;

		virtual bool contains( const size_t x, const size_t y ) { return false; }
		virtual size_t insert( const point * const P ) { return false; }
		virtual size_t insert( const size_t X, const size_t Y, const datatype & D ) { return false; }

		size_t x = 0, y = 0; // The location of this point.
		datatype data; // The data at this point.
	};

	/**************************************************************************
	@author Jeremy Gamet and John Colton

	@par Description:
	This struct handles the division of points.
	**************************************************************************/
	struct quad : point
	{
		quad( const size_t X, const size_t Y, const size_t W, const size_t H );
		~quad();

		bool contains( const size_t x, const size_t y );
		size_t insert( const point * const P );
		size_t insert( const size_t X, const size_t Y, const datatype & D );

		// Top Left, Top Right, Bottom Left, Bottom Right
		point * tl = nullptr;
		point * tr = nullptr;
		point * bl = nullptr;
		point * br = nullptr;

		// The point defining the top left of this quad.
		size_t x = 0, y = 0;

		// The horizontal and vertical ranges of points this quad contains.
		size_t width, height;

		// The data in this quad.
		datatype data;
	};

	quad * root = nullptr;

	// The horizontal and vertical ranges of points this quadtree contains.
	size_t width, height;

	// The number of quad and point structs currently in the tree.
	struct { size_t quads, points; } count = { 0, 0 };


	public:

	quadtree( const size_t W, const size_t H );
	quadtree( const quadtree<datatype> &obj );
	~quadtree();
	bool insert( const size_t X, const size_t Y, const datatype & D );
	void quadtree<datatype>::subdiv( const datatype ** const image, const datatype & eps );
	void clear();
	size_t numQuads();
	size_t numPoints();
	size_t size();
};

/******************************************************************************
@author John Colton

@par Description:
The empty point struct constructor.
******************************************************************************/
template<class datatype>
quadtree<datatype>::point::point() {}

/******************************************************************************
@author John Colton

@par Description:
The other point struct constructor.

@param[in] X - The x location of this point.
@param[in] Y - The y location of this point.
@parac[in] D - The data at this point.
******************************************************************************/
template<class datatype>
quadtree<datatype>::point::point( const size_t X, const size_t Y, const datatype & D )
	: x( X ), y( Y ), data( D ) {}

/******************************************************************************
@author John Colton

@par Description:
This point struct function returns the data at this point.

@returns datatype - The data at this point.
******************************************************************************/
template<class datatype>
datatype quadtree<datatype>::point::getData() const { return data; }


/******************************************************************************
@author John Colton

@par Description:
The quad struct constructor.

@param[in] X - The x location of the top left of this quadrant.
@param[in] Y - The y location of the top left of this quadrant.
@parac[in] W - The width of this quadrant.
@parac[in] H - The height of this quadrant.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quad::quad( const size_t X, const size_t Y, const size_t W, const size_t H )
	: x( X ), y( Y ), width( W ), height( H ) {}

/******************************************************************************
@author Jeremy Gamet

@par Description:
The copy constructor for this quadtree class.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quadtree(const quadtree<datatype> &obj) 
{
	quadtree<datatype> * ptr = root;
}

/******************************************************************************
@author John Colton

@par Description:
The quad struct destructor.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quad::~quad()
{
	delete tl;
	delete tr;
	delete bl;
	delete br;
}

/******************************************************************************
@author John Colton

@par Description:
Tells whether a point lies within this quadrant or not.

@param[in] X - The x location of the point to check.
@param[in] Y - The y location of the point to check.

@returns true - The point is in this quadrant.
@returns false - The point is not in this quadrant.
******************************************************************************/
template<class datatype>
bool quadtree<datatype>::quad::contains( const size_t X, const size_t Y )
{
	return ( x <= X && X <= ( x + width ) && y <= Y && Y <= ( y + height ) );
}

/******************************************************************************
@author John Colton

@par Description:
Adds a point to this quadrant.

@param[in] P - The point to add.

@returns  #>0 - The number of quadrants that had to be created to insert this
                point.
@returns  0   - No new quadrants were created, only a single point.
@returns -1   - No points or quads were created to insert this point. This
                happens when the point being inserted is already in the tree.
				In this case, the data at that point is overwritten with the
				new data given.
******************************************************************************/
template<class datatype>
size_t quadtree<datatype>::quad::insert( const point * const P )
{
	return insert( P->x, P->y, P->data );
}

/******************************************************************************
@author John Colton

@par Description:
Adds a point to this quadrant.

@param[in] X - The x location of the point being added.
@param[in] Y - The y location of the point being added.
@parac[in] D - The data of the point being added.

@returns  #>0 - The number of quadrants that had to be created to insert this
                point.
@returns  0   - No new quadrants were created, only a single point.
@returns -1   - No points or quads were created to insert this point. This
                happens when the point being inserted is already in the tree.
				In this case, the data at that point is overwritten with the
				new data given.
******************************************************************************/
template<class datatype>
size_t quadtree<datatype>::quad::insert( const size_t X, const size_t Y, const datatype & D )
{
	// Top
	if ( Y - y < height / 2 )
	{
		// Left
		if ( X - x < width / 2 )
		{
			// If the quadrant containing the point being inserted is
			// unallocated, allocate it as a point.
			if ( tl == nullptr ) tl = new point( X, Y, D );

			// If a point has been allocated to the pointer reserved for this
			// quadrant, replace that point with a quad and insert both that
			// point and the new point into that quad.
			else if ( !dynamic_cast<quad*>(tl) )
			{
				// If it's actually the same point, just overwrite its data.
				if ( tl->x == X && tl->y == Y )
				{
					tl->data = D;

					return -1;
				}

				const point * const temp = tl;
				tl = new quad( x, y, width / 2, height / 2 );
				tl->data = data;

				tl->insert( temp ); // This will return 0.
				return 1 + tl->insert( X, Y, D );
			}

			// Insert the point being inserted into the quad struct defining
			// the area the point to be inserted lies within.
			else if ( D != tl->data ) return tl->insert( X, Y, D );
		}

		// Right
		else
		{
			if ( tr == nullptr ) tr = new point( X, Y, D );

			else if ( !dynamic_cast<quad*>( tr ) )
			{
				if ( tr->x == X && tr->y == Y )
				{
					tr->data = D;

					return -1;
				}

				const point * const temp = tr;
				tr = new quad( x + width / 2, y, width - width / 2, height / 2 );
				tr->data = data;

				tr->insert( temp );
				return 1 + tr->insert( X, Y, D );
			}

			else if ( D != tr->data ) return tr->insert( X, Y, D );
		}
	}

	// Bottom
	else
	{
		// Left
		if ( X - x < width / 2 )
		{
			if ( bl == nullptr ) bl = new point( X, Y, D );

			else if ( !dynamic_cast<quad*>( bl ) )
			{
				if ( bl->x == X && bl->y == Y )
				{
					bl->data = D;

					return -1;
				}

				const point * const temp = bl;
				bl = new quad( x, y + height / 2, width, height - height / 2 );
				bl->data = data;

				bl->insert( temp );
				return 1 + bl->insert( X, Y, D );
			}

			else if ( D != bl->data ) return bl->insert( X, Y, D );
		}

		// Right
		else
		{
			if ( br == nullptr ) br = new point( X, Y, D );

			else if ( !dynamic_cast<quad*>( br ) )
			{
				if ( br->x == X && br->y == Y )
				{
					br->data = D;

					return -1;
				}

				const point * const temp = br;
				br = new quad( x + width / 2, y + height / 2, width - width / 2, height - height / 2 );
				br->data = data;

				br->insert( temp );
				return 1 + br->insert( X, Y, D );
			}

			else if ( D != br->data ) return br->insert( X, Y, D );
		}
	}

	// If only a point was added.
	return 0;
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
******************************************************************************/
template <class datatype>
quadtree<datatype>::~quadtree()
{
	delete root;
}

/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
Inserts a point into the quadtree.

@param[in] X - The x location of the point being added.
@param[in] Y - The y location of the point being added.
@parac[in] D - The data of the point being added.

@returns true - If the item was inserted.
@returns false - If the item was not inserted.
******************************************************************************/
template <class datatype>
bool quadtree<datatype>::insert( const size_t X, const size_t Y, const datatype & D )
{
	// If the item being inserted is within the bounds of the quadtree.
	if ( X < width && Y < height )
	{
		// If the quadtree is not empty and the item being inserted is not the
		// same as the root of the tree.
		if ( root != nullptr && root->data != D )
		{
			// Returns the number of quads created for this point to be inserted.
			const size_t temp = root->insert( X, Y, D );

			// -1 == point was not inserted:
			// case 1 = point has same data as quad it exists within
			// case 2 = point has already been inserted, only its data was changed
			if ( temp != -1 )
			{
				count.quads += temp;

				++count.points;
			}
		}

		// If the quadtree is empty.
		else if ( root == nullptr )
		{
			root = new quad( 0, 0, width, height );

			root->data = D;

			++count.quads;
		}

		return true;
	}

	else return false;
}

/******************************************************************************
@author Jeremy Gamet and John Colton

@par Description:
Clears the quadtree.
******************************************************************************/
template <class datatype>
void quadtree<datatype>::clear()
{
	count = { 0, 0 };
	
	delete root;
}

/******************************************************************************
@author John Colton

@par Description:
Returns the number of quads in the quadtree.

@returns size_t - The number of quads in the quadtree.
******************************************************************************/
template <class datatype>
size_t quadtree<datatype>::numQuads()
{
	return count.quads;
}

/******************************************************************************
@author John Colton

@par Description:
Returns the number of points in the quadtree.

@returns size_t - The number of points in the quadtree.
******************************************************************************/
template <class datatype>
size_t quadtree<datatype>::numPoints()
{
	return count.points;
}

/******************************************************************************
@author John Colton

@par Description:
Returns the size of the quadtree in bytes.

@returns size_t - The size of the quadtree in bytes.
******************************************************************************/
template <class datatype>
size_t quadtree<datatype>::size()
{
	return ( sizeof( quadtree<datatype> ) ) +
		( count.quads * sizeof( quadtree<datatype>::quad ) ) +
		( count.points * sizeof( quadtree<datatype>::point ) );
}


#endif /* QUADTREE_H */
