/******************************************************************************* 
 * Copyright (c) 2009, Andreas Karrenbauer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *******************************************************************************/

/**
 * \file ppmimage.h
 * \author Andreas Karrenbauer
 */

#ifndef PPMIMAGE_H
#define PPMIMAGE_H
#include <cassert>
#include <assert.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>

using namespace std;

/**
 * \addtogroup sda
 * @{
 */

/** \struct PPMImage
 * \brief Represent a ppm image
 */
struct PPMImage 
{
	int n;/*!< number of lines*/
	int m;/*!< number of columns*/
	int maxval;/*!< maximum color value of the image*/

	int** rgb;/*!< image*/

	std::string magic;

	/**
	 * \brief Copy constructor
	 */ 
	PPMImage( const PPMImage& image ) : n(image.n), m(image.m), maxval(image.maxval), magic(image.magic) {
		rgb = new int*[m];
		for (int j = 0; j < m; ++j) 
		{
			rgb[j] = new int[n];
		}
		for (int i = 0; i < n; ++i) 
		{
			for (int j = 0; j < m; ++j) 
			{
				rgb[j][i]=image.rgb[j][i];
				assert( rgb[j][i] <= maxval );
			}
		}
		
		
			}

	/**
	 * \brief Constructor from a .ppm file
	 */ 
	template< typename istream >
	PPMImage( istream& image )
	{
		while( image.peek() == '#' || image.peek() == '\n') image.ignore( 1 << 16, '\n' );
		image >> magic;
		while( image.peek() == '#' || image.peek() == '\n') image.ignore( 1 << 16, '\n' );
		image >> m;
		while( image.peek() == '#' || image.peek() == '\n') image.ignore( 1 << 16, '\n' );

		if( magic == "P3" )
			m *= 3;

		image >> n;
		while( image.peek() == '#' || image.peek() == '\n') image.ignore( 1 << 16, '\n' );
		if( magic == "P1" )
		{
			maxval = 1;
		}
		else
		{
			image >> maxval;
			while( image.peek() == '#' || image.peek() == '\n') image.ignore( 1 << 16, '\n' );
		}

		rgb = new int*[m];
		for (int j = 0; j < m; ++j)
		{
			rgb[j] = new int[n];
		}

		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < m; ++j)
			{
				image >> rgb[j][i];
				assert( rgb[j][i] <= maxval );
			}
		}

	}

	//n = number of lines
	//m = number of columns
	PPMImage(char** buffer,int n,int m,int maxval,string magic): n(n), m(m), maxval(maxval), magic(magic){

		rgb = new int*[m];
		for(int j = 0; j < m; j++){
			rgb[j] = new int [n];
		}
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < m; ++j)
			{
				rgb[j][i] = buffer[j][i];
				assert( rgb[j][i] <= maxval );
			}
		}


	}

	/**
	 * \brief Constructor of a black image
	 * \param a : number of lines
	 * \param b : number of columns
	 * \param c : maximum value of the image
	 */ 
	PPMImage(int a, int b, int c)
	{
		n=a; 
		m=b; 
		maxval=c;

		rgb = new int*[m];

		for (int j=0; j<m ; ++j)
		{
			rgb[j] = new int[n];
			for (int i=0; i<n; ++i)
			{
				rgb[j][i] = 0;
			}
		}
	}


	/**
	 * \brief Destructor
	 */
	~PPMImage() 
	{
	  for (int j=0; j<m ; ++j) {
	    if( rgb[j] ) delete[] rgb[j];
	  }
	    if( rgb ) 
			delete[] rgb;
	}


	/**
	 * \brief Return reference of the RGB table
	 * \param i : i-th column
	 * \param j : j-th row
	 * \return RGB table reference
	 */
	int& r( const int& i, const int& j ) 
	{ 
		assert( 0 <= i ); 
		assert( i < n ); 
		return rgb[j][i]; 
	}

	/**
	 * \brief Return const reference of the RGB table
	 * \param i : i-th column
	 * \param j : j-th row
	 * \return constant RGB table reference
	 */

	int r( const int& i, const int& j ) const 
	{
		assert( 0 <= i ); 
		assert( i < n ); 
		return rgb[j][i]; 
	}

	/**
	 * \brief Return row pointer
	 * \param j : Row position in the RGB table
	 * \return Row pointer
	 */
	int* operator[]( const int& j ) 
	{
		return rgb[j]; 
	}
	
};

std::ostream& operator<<( std::ostream& stream, const PPMImage& image ) {
  stream << image.magic << std::endl;
  stream << image.m << " " << image.n << std::endl;
  stream << image.maxval << std::endl;
  for( int i = 0; i < image.n; ++i ) {
    for( int j = 0; j < image.m; ++j ) {
      stream << image.r(i,j) << " ";
    }
    stream << std::endl;
  }
  return stream;
}



/** @} */

#endif
