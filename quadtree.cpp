#ifndef QUADTREE_H
#define QUADTREE_H


/**************************************************************************//**
@author Jeremy Gamet and John Colton

@par Description:
This class is our implementation of a quadtree.
******************************************************************************/
template<class datatype>
class quadtree
{
	private:

	/**********************************************************************//**
	@author Jeremy Gamet and John Colton

	@par Description:
	This struct holds the data in our quadtree.
	**************************************************************************/
	struct point
	{
		point();
		point( const size_t X, const size_t Y, const datatype & D );

		datatype getData( const size_t X, const size_t Y ) const;
		bool contains( const size_t x, const size_t y ) const;

		virtual size_t insert( const point * const P ) { return 0; }
		virtual size_t insert( const size_t X, const size_t Y, const datatype & D ) { return 0; }
		virtual size_t optimize( size_t & numQuads, size_t & numPoints ) { return 0; }

		size_t x = 0, y = 0; // The location of this point.
		datatype data; // The data at this point.
	};

	/**********************************************************************//**
	@author Jeremy Gamet and John Colton

	@par Description:
	This struct handles the division of points. The x and y variables from the
	point struct are reused as the location of the top left corner of this
	struct, and the data variable is reused as the default data for every point
	in the rectangle defined by this struct.
	**************************************************************************/
	struct quad : point
	{
		quad( const size_t X, const size_t Y, const size_t W, const size_t H, const datatype & D );
		~quad();

		datatype getData( const size_t X, const size_t Y ) const;
		bool contains( const size_t x, const size_t y ) const;

		size_t insert( const point * const P );
		size_t insert( const size_t X, const size_t Y, const datatype & D );
		size_t optimize( size_t & numQuads, size_t & numPoints );

		// Top Left, Top Right, Bottom Left, Bottom Right
		point * tl = nullptr;
		point * tr = nullptr;
		point * bl = nullptr;
		point * br = nullptr;

		// The horizontal and vertical ranges of points this quad contains.
		size_t width, height;
	};

	quad * root = nullptr;

	// The horizontal and vertical ranges of points this quadtree contains.
	size_t width, height;

	// The number of quad and point structs currently in the tree.
	struct { size_t quads, points; } count = { 0, 0 };


	public:

	quadtree( const size_t W, const size_t H );
	~quadtree();
	bool insert( const size_t X, const size_t Y, const datatype & D );
	datatype getData( const size_t X, const size_t Y ) const;
	void optimize();
	void clear();
	size_t numQuads();
	size_t numPoints();
	size_t size();
};


/**************************************************************************//**
@author John Colton

@par Description:
The empty point struct constructor.
******************************************************************************/
template<class datatype>
quadtree<datatype>::point::point() {}

/**************************************************************************//**
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

/**************************************************************************//**
@author John Colton

@par Description:
This point struct function returns the data at this point.

@returns datatype - The data at this point.
******************************************************************************/
template<class datatype>
datatype quadtree<datatype>::point::getData( const size_t X, const size_t Y ) const { return data; }

/**************************************************************************//**
@author John Colton

@par Description:
Tells whether a given point is at the same point as this point.

@param[in] X - The x location of the point to check.
@param[in] Y - The y location of the point to check.

@returns true  - The point is at this point.
@returns false - The point is not at this point.
******************************************************************************/
template<class datatype>
bool quadtree<datatype>::point::contains( const size_t X, const size_t Y ) const
{
	return ( x == X && y == Y );
}


/**************************************************************************//**
@author John Colton

@par Description:
The quad struct constructor.

@param[in] X - The x location of the top left of this quadrant.
@param[in] Y - The y location of the top left of this quadrant.
@parac[in] W - The width of this quadrant.
@parac[in] H - The height of this quadrant.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quad::quad( const size_t X, const size_t Y, const size_t W, const size_t H, const datatype & D )
	: point( X, Y, D ), width( W ), height( H ) {}

/**************************************************************************//**
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

/**************************************************************************//**
@author John Colton

@par Description:
Returns the data at a point.

@param[in] X - The x location of the point to check.
@param[in] Y - The y location of the point to check.

@returns datatype - The data at this point.
******************************************************************************/
template<class datatype>
datatype quadtree<datatype>::quad::getData( const size_t X, const size_t Y ) const
{
	quad * temp;

	// If the top left quadrant pointer points to a quad struct that contains
	// the point we are looking for, return the data at the point.
	if ( temp = dynamic_cast<quad*>( tl ) )
	{
		if ( temp->contains( X, Y ) )
			return temp->getData( X, Y );
	}

	// Else if the top left quadrant pointer points to a point struct that is
	// at the point we are looking for, return its data.
	else if ( tl != nullptr && tl->contains( X, Y ) ) return tl->data;

	if ( temp = dynamic_cast<quad*>( tr ) )
	{
		if ( temp->contains( X, Y ) )
			return temp->getData( X, Y );
	}

	else if ( tr != nullptr && tr->contains( X, Y ) ) return tr->data;

	if ( temp = dynamic_cast<quad*>( bl ) )
	{
		if ( temp->contains( X, Y ) )
			return temp->getData( X, Y );
	}

	else if ( bl != nullptr && bl->contains( X, Y ) ) return bl->data;

	if ( temp = dynamic_cast<quad*>( br ) )
	{
		if ( temp->contains( X, Y ) )
			return temp->getData( X, Y );
	}

	else if ( br != nullptr && br->contains( X, Y ) ) return br->data;

	// The point is not in any of the set quadrants of this quad, so it must
	// have the same data as this quads default data.
	return data;
}

/**************************************************************************//**
@author John Colton

@par Description:
Tells whether a point lies within this quadrant or not.

@param[in] X - The x location of the point to check.
@param[in] Y - The y location of the point to check.

@returns true - The point is in this quadrant.
@returns false - The point is not in this quadrant.
******************************************************************************/
template<class datatype>
bool quadtree<datatype>::quad::contains( const size_t X, const size_t Y ) const
{
	return ( X >= x && X < ( x + width ) && Y >= y && Y < ( y + height ) );
}

/**************************************************************************//**
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
				new data given. Alternatively, if the pointer is a nullptr.
******************************************************************************/
template<class datatype>
size_t quadtree<datatype>::quad::insert( const point * const P )
{
	return ( P == nullptr ? -1 : insert( P->x, P->y, P->data ) );
}

/**************************************************************************//**
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
				if ( tl->contains( X, Y ) )
				{
					tl->data = D;

					return -1;
				}

				const point * const temp = tl;
				tl = new quad( x, y, width / 2, height / 2, data );

				tl->insert( temp ); // This will return 0.
				delete temp; // A new point was created from temp's data, so we can delete this one.
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
				if ( tr->contains( X, Y ) )
				{
					tr->data = D;

					return -1;
				}

				const point * const temp = tr;
				tr = new quad( x + width / 2, y, width - width / 2, height / 2, data );

				tr->insert( temp );
				delete temp;
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
				if ( bl->contains( X, Y ) )
				{
					bl->data = D;

					return -1;
				}

				const point * const temp = bl;
				bl = new quad( x, y + height / 2, width / 2, height - height / 2, data );

				bl->insert( temp );
				delete temp;
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
				if ( br->contains( X, Y ) )
				{
					br->data = D;

					return -1;
				}

				const point * const temp = br;
				br = new quad( x + width / 2, y + height / 2, width - width / 2, height - height / 2, data );

				br->insert( temp );
				delete temp;
				return 1 + br->insert( X, Y, D );
			}

			else if ( D != br->data ) return br->insert( X, Y, D );
		}
	}

	// If only a point was added.
	return 0;
}

/**************************************************************************//**
@author John Colton

@par Description:
This function attempts to optimize the quadtree storage. It works by getting
the unique items in this quad (including the quads' default item) and counting
how many times they each occur. If one of them occurs more often than the
quads' default item, that item becomes the default and the quads' sub-quadrants
are modified accordingly.

@returns size_t - The number of items in this quadrant that match the default
                  item of this quadrant. After optimization, this should be
				  more than half of them.
******************************************************************************/
template <class datatype>
size_t quadtree<datatype>::quad::optimize( size_t & numQuads, size_t & numPoints )
{
	// The number of items in each quadrant that are the same as their
	// quadrants' base item.
	struct { size_t tl, tr, bl, br; } matchCount = { 0, 0, 0, 0 };

	// If the top left quad pointer points to something, get the number of
	// items contained by the thing it points to that have the same data as the
	// default data of the thing the top left pointer points to.
	if ( tl != nullptr )
	{
		// If the top left quad pointer points to a quad, optimize it.
		if ( dynamic_cast<quad*>( tl ) ) matchCount.tl = tl->optimize( numQuads, numPoints );

		// Otherwise it points to a point.
		else ++matchCount.tl;
	}

	if ( tr != nullptr )
	{
		if ( dynamic_cast<quad*>( tr ) ) matchCount.tr = tr->optimize( numQuads, numPoints );
		else ++matchCount.tr;
	}

	if ( bl != nullptr )
	{
		if ( dynamic_cast<quad*>( bl ) ) matchCount.bl = bl->optimize( numQuads, numPoints );
		else ++matchCount.bl;
	}

	if ( br != nullptr )
	{
		if ( dynamic_cast<quad*>( br ) ) matchCount.br = br->optimize( numQuads, numPoints );
		else ++matchCount.br;
	}


	// The number of points in this quadrant whose data is defined by the
	// default data of the quad.
	const size_t numDefault = ( width * height ) - matchCount.tl - matchCount.tr - matchCount.bl - matchCount.br;

	// The five possible unique items defined by: this quads data, and the data
	// in each of this quads' quadrants.
	datatype * items[5];
	// The number of items in this quadrant that are the same as their
	// respective data in 'items'.
	size_t iCount[5] = { numDefault };

	// If at least one of the points in this quadrant is defined by the default
	// data of this quad, set the default data of this quad as the first unique
	// item in the unique items list.
	if ( numDefault ) items[0] = &data;

	// If the top left quadrant pointer is not a nullptr, check if its data is
	// in the unique data list ('items'). If it is, add the number of items in
	// this quadrant that have the same data as this quadrants default data to
	// the respective count ('iCount') for that unique data. Otherwise add the
	// data to the list.
	if ( tl != nullptr )
	{
		// Iterate through the items in the item list.
		for ( size_t item = 0; item < 5; ++item )
		{
			// If the iCount[item] is >0, there is an item at items[item].
			if ( iCount[item] )
			{
				// If the item at items[item] is the same as the item in this
				// quadrant, add its count to this items' count.
				if ( *items[item] == tl->data )
				{
					iCount[item] += matchCount.tl;

					break;
				}
			}

			// There is not an item at items[item].
			else
			{
				// Put this item at items[item].
				items[item] = &tl->data;

				// Set its item count.
				iCount[item] = matchCount.tl;

				break;
			}
		}
	}

	if ( tr != nullptr )
	{
		for ( size_t item = 0; item < 5; ++item )
		{
			if ( iCount[item] )
			{
				if ( *items[item] == tr->data )
				{
					iCount[item] += matchCount.tr;
					break;
				}
			}

			else
			{
				items[item] = &tr->data;
				iCount[item] = matchCount.tr;
				break;
			}
		}
	}

	if ( bl != nullptr )
	{
		for ( size_t item = 0; item < 5; ++item )
		{
			if ( iCount[item] )
			{
				if ( *items[item] == bl->data )
				{
					iCount[item] += matchCount.bl;
					break;
				}
			}

			else
			{
				items[item] = &bl->data;
				iCount[item] = matchCount.bl;
				break;
			}
		}
	}

	if ( br != nullptr )
	{
		for ( size_t item = 0; item < 5; ++item )
		{
			if ( iCount[item] )
			{
				if ( *items[item] == br->data )
				{
					iCount[item] += matchCount.br;
					break;
				}
			}

			else
			{
				items[item] = &br->data;
				iCount[item] = matchCount.br;
				break;
			}
		}
	}


	// The item with the greatest similar items in this quadrant.
	datatype best = *items[0];
	// The number of items similar to 'best'.
	size_t max = iCount[0];

	// Determine which item in this quadrant has the most similar items.
	for ( size_t item = 1; item < 5; ++item )
	{
		if ( iCount[item] > max )
		{
			max = iCount[item];

			best = *items[item];
		}
	}


	// If the best choice for the default item is not the default item, make it
	// the default item and adjust the sub-quadrants appropriately (not in that
	// order).
	if ( best != data )
	{
		// If the subquadrants of this quad cannot be subdivided any further.
		const bool isSmallest = ( width < 5 && height < 5 );


		// If the top left quadrant pointer is a nullptr or a point struct.
		if ( !dynamic_cast<quad*>( tl ) )
		{
			// If this quad cannot be subdivided any further.
			if ( isSmallest )
			{
				// If there is a point in the top left quadrant and its data is
				// the same as the new default data for this quad.
				if ( tl != nullptr && tl->data == best )
				{
					// Delete the point.
					delete tl;
					
					// Set the top left quadrant pointer to nullptr.
					tl = nullptr;

					// Decrement the quadtree point count.
					--numPoints;
				}
			}

			// If this quad can be subdivided further.
			else
			{
				// Store the top left quadrant pointer in a temp pointer.
				const point * const temp = tl;

				// Create a new quad for the top left quadrant and give that
				// quadrant the data that was previously the default data of
				// this quad.
				tl = new quad( x, y, width / 2, height / 2, data );

				// Insert the point previously pointed to by the top left
				// quadrant pointer of this quad into the new quad. Nothing
				// will happen if it is a nullptr.
				tl->insert( temp );

				// Inserting temp into a quad created a new point from temp's
				// data, so we can now delete temp.
				delete temp;

				// Increment the quadtree quad count.
				++numQuads;
			}
		}

		// If the data in the quad struct pointed to by the top left quadrant
		// pointer is the same as the new default data of this quad.
		else if ( tl->data == best )
		{
			// Create a temp quad pointer so the quads' sub-quadrants can be
			// accessed. They can't be accesed without a cast because the
			// compiler thinks tl points to a point struct.
			const quad * const temp = static_cast<quad*>( tl );

			// If the quad pointed to by the top left quadrant pointer has no
			// sub-quadrants, delete it.
			if ( temp->tl == nullptr && temp->tr == nullptr && temp->bl == nullptr && temp->br == nullptr )
			{
				delete temp;

				tl = nullptr;

				--numQuads;
			}

			// If the quad pointed to by the top left quadrant pointer has only
			// one sub-quadrant, and that sub-quadrant is a point, delete it
			// and replace it with that point.
			else if ( temp->tl != nullptr && temp->tr != nullptr && temp->bl != nullptr && temp->br != nullptr )
			{
				// If the sub-quadrant of the quad pointed to by the top left
				// quadrant pointer is a point struct.
				bool isPoint = true;

				// If the top left quadrant pointer of the quad pointed to by
				// the top left quadrant pointer is a point struct, replace the
				// top left quadrant pointer with that point.
				if ( temp->tl != nullptr && !dynamic_cast<quad*>( temp->tl ) )
					tl = new point( temp->tl->x, temp->tl->y, temp->tl->data );

				else if ( temp->tr != nullptr && !dynamic_cast<quad*>( temp->tr ) )
					tl = new point( temp->tr->x, temp->tr->y, temp->tr->data );

				else if ( temp->bl != nullptr && !dynamic_cast<quad*>( temp->bl ) )
					tl = new point( temp->bl->x, temp->bl->y, temp->bl->data );

				else if ( temp->br != nullptr && !dynamic_cast<quad*>( temp->br ) )
					tl = new point( temp->br->x, temp->br->y, temp->br->data );

				// If the single quadrant pointer in the quad pointed to by the
				// top left quad pointer is not a point (it is a quad).
				else isPoint = false;

				// If the top left quadrant pointer was replaced by a point.
				if ( isPoint )
				{
					// Delete the quad (and the point it contains).
					delete temp;

					// Decrement the quadtree quad count.
					--numQuads;
				}
			}
		}

		if ( !dynamic_cast<quad*>( tr ) )
		{
			if ( isSmallest )
			{
				if ( tr != nullptr && tr->data == best )
				{
					delete tr;
					tr = nullptr;
					--numPoints;
				}
			}

			else
			{
				const point * const temp = tr;
				tr = new quad( x, y, width / 2, height / 2, data );
				tr->insert( temp );
				delete temp;
				++numQuads;
			}
		}

		else if ( tr->data == best )
		{
			const quad * const temp = static_cast<quad*>( tr );

			if ( temp->tl == nullptr && temp->tr == nullptr && temp->bl == nullptr && temp->br == nullptr )
			{
				delete temp;
				tr = nullptr;
				--numQuads;
			}

			else if ( temp->tl != nullptr && temp->tr != nullptr && temp->bl != nullptr && temp->br != nullptr )
			{
				bool isPoint = true;

				if ( temp->tl != nullptr && !dynamic_cast<quad*>( temp->tl ) )
					tr = new point( temp->tl->x, temp->tl->y, temp->tl->data );

				else if ( temp->tr != nullptr && !dynamic_cast<quad*>( temp->tr ) )
					tr = new point( temp->tr->x, temp->tr->y, temp->tr->data );

				else if ( temp->bl != nullptr && !dynamic_cast<quad*>( temp->bl ) )
					tr = new point( temp->bl->x, temp->bl->y, temp->bl->data );

				else if ( temp->br != nullptr && !dynamic_cast<quad*>( temp->br ) )
					tr = new point( temp->br->x, temp->br->y, temp->br->data );

				else isPoint = false;

				if ( isPoint )
				{
					delete temp;
					--numQuads;
				}
			}
		}

		if ( !dynamic_cast<quad*>( bl ) )
		{
			if ( isSmallest )
			{
				if ( bl != nullptr && bl->data == best )
				{
					delete bl;
					bl = nullptr;
					--numPoints;
				}
			}

			else
			{
				const point * const temp = bl;
				bl = new quad( x, y, width / 2, height / 2, data );
				bl->insert( temp );
				delete temp;
				++numQuads;
			}
		}

		else if ( bl->data == best )
		{
			const quad * const temp = static_cast<quad*>( bl );

			if ( temp->tl == nullptr && temp->tr == nullptr && temp->bl == nullptr && temp->br == nullptr )
			{
				delete temp;
				bl = nullptr;
				--numQuads;
			}

			else if ( temp->tl != nullptr && temp->tr != nullptr && temp->bl != nullptr && temp->br != nullptr )
			{
				bool isPoint = true;

				if ( temp->tl != nullptr && !dynamic_cast<quad*>( temp->tl ) )
					bl = new point( temp->tl->x, temp->tl->y, temp->tl->data );

				else if ( temp->tr != nullptr && !dynamic_cast<quad*>( temp->tr ) )
					bl = new point( temp->tr->x, temp->tr->y, temp->tr->data );

				else if ( temp->bl != nullptr && !dynamic_cast<quad*>( temp->bl ) )
					bl = new point( temp->bl->x, temp->bl->y, temp->bl->data );

				else if ( temp->br != nullptr && !dynamic_cast<quad*>( temp->br ) )
					bl = new point( temp->br->x, temp->br->y, temp->br->data );

				else isPoint = false;

				if ( isPoint )
				{
					delete temp;
					--numQuads;
				}
			}
		}

		if ( !dynamic_cast<quad*>( br ) )
		{
			if ( isSmallest )
			{
				if ( br != nullptr && br->data == best )
				{
					delete br;
					br = nullptr;
					--numPoints;
				}
			}

			else
			{
				const point * const temp = br;
				br = new quad( x, y, width / 2, height / 2, data );
				br->insert( temp );
				delete temp;
				++numQuads;
			}
		}

		else if ( br->data == best )
		{
			const quad * const temp = static_cast<quad*>( br );

			if ( temp->tl == nullptr && temp->tr == nullptr && temp->bl == nullptr && temp->br == nullptr )
			{
				delete temp;
				br = nullptr;
				--numQuads;
			}

			else if ( temp->tl != nullptr && temp->tr != nullptr && temp->bl != nullptr && temp->br != nullptr )
			{
				bool isPoint = true;

				if ( temp->tl != nullptr && !dynamic_cast<quad*>( temp->tl ) )
					br = new point( temp->tl->x, temp->tl->y, temp->tl->data );

				else if ( temp->tr != nullptr && !dynamic_cast<quad*>( temp->tr ) )
					br = new point( temp->tr->x, temp->tr->y, temp->tr->data );

				else if ( temp->bl != nullptr && !dynamic_cast<quad*>( temp->bl ) )
					br = new point( temp->bl->x, temp->bl->y, temp->bl->data );

				else if ( temp->br != nullptr && !dynamic_cast<quad*>( temp->br ) )
					br = new point( temp->br->x, temp->br->y, temp->br->data );

				else isPoint = false;

				if ( isPoint )
				{
					delete temp;
					--numQuads;
				}
			}
		}


		// Replace the current default with the new default.
		data = best;
	}


	// Return the number of items that match the default item of this quad.
	return max;
}



/**************************************************************************//**
@author Jeremy Gamet and John Colton

@par Description:
Constructor for the quadtree class.
******************************************************************************/
template<class datatype>
quadtree<datatype>::quadtree( const size_t W, const size_t H ) : width( W ), height( H ) {}

/**************************************************************************//**
@author Jeremy Gamet and John Colton

@par Description:
Destructor for the quadtree class.
******************************************************************************/
template <class datatype>
quadtree<datatype>::~quadtree()
{
	delete root;
}

/**************************************************************************//**
@author Jeremy Gamet and John Colton

@par Description:
Inserts a point into the quadtree.

@param[in] X - The x location of the point being added.
@param[in] Y - The y location of the point being added.
@parac[in] D - The data of the point being added.

@returns true  - If the item was inserted.
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
			root = new quad( 0, 0, width, height, D );

			++count.quads;
		}

		return true;
	}

	else return false;
}

/**************************************************************************//**
@author John Colton

@par Description:
Returns the data at a point. Will fail if the point to get it outside the range
of this quadtree or if this quadtree is empty.

@param[in] X - The x location of the point to check.
@param[in] Y - The y location of the point to check.

@returns datatype - The data at this point.
******************************************************************************/
template<class datatype>
datatype quadtree<datatype>::getData( const size_t X, const size_t Y ) const
{
	return root->getData( X, Y );
}

/**************************************************************************//**
@author John Colton

@par Description:
This function attempts to optimize the quadtree storage.
******************************************************************************/
template <class datatype>
void quadtree<datatype>::optimize()
{
	if ( root != nullptr ) root->optimize( count.quads, count.points );
}

/**************************************************************************//**
@author Jeremy Gamet and John Colton

@par Description:
Clears the quadtree.
******************************************************************************/
template <class datatype>
void quadtree<datatype>::clear()
{
	count = { 0, 0 };
	
	delete root;

	root = nullptr;
}

/**************************************************************************//**
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

/**************************************************************************//**
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

/**************************************************************************//**
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
