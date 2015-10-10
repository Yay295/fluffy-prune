#pragma once

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

		const bool isPoint = true; // This point is a point.
		size_t x = 0, y = 0; // The location of this point.
		datatype data; // THe data at this point.
	};

	/**************************************************************************
	@author Jeremy Gamet and John Colton

	@par Description:
	This struct handles the division of points.
	**************************************************************************/
	struct quad : point
	{
		quad( const size_t X, const size_t Y, const size_t W, const size_t H );

		bool contains( const size_t x, const size_t y );
		void insert( const point * const P );
		void insert( const size_t X, const size_t Y, const datatype & D );

		// Top Left, Top Right, Bottom Left, Bottom Right
		point * tl = nullptr;
		point * tr = nullptr;
		point * bl = nullptr;
		point * br = nullptr;

		// This quad is not a point.
		const bool isPoint = false;

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


	public:

	quadtree( const size_t W, const size_t H );
	~quadtree();
	bool insert( const size_t X, const size_t Y, const datatype & D );
	void subdiv();
	void clear();
};


#endif /* QUADTREE_H */
