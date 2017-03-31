/******************************************************************************* 
 * Copyright (c) 2011, George Nomikos (nomikos88@gmail.com)
 * Copyright (c) 2011, Andreas Karrenbauer (andreas.karrenbauer@uni-konstanz.de)
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

#ifndef GRIDFLOW_H
#define GRIDFLOW_H

#include <vector>
#include <queue>
#include <algorithm>
#include "bucketqueue.h"
#include <iostream>
#include <assert.h>
#include <climits>
#include "temp.h"

#ifdef WIN32
	#include <ctime>
	#include "getopt.h"
#endif

#define	INFINITE 8000000

/** \struct ArcData
 *  \brief Contains information concerning the arcs of a node
 */
template< int N >
struct ArcData {
	int flow;
	int difference[N];
	int forward_length, backward_length;
};

/** \struct Node
 * \brief Contains information about the position of a node and his distance from the source
 */
struct Node {
	int y;
	int x;
	int key;
	Node( const int& i = -1, const int& j = -1, const int& k = 0 ) : y(i), x(j), key(k) {}
	bool operator<( const Node& v ) const {
		return key > v.key;
	}
};

/** \struct NodeData
 * \brief Contains crucial information for each node in order to solve the min cost flow problem
 */
template< int N >
struct NodeData {
	int deficiency;
	int apexcost;
	int potential;
	int distance;
	int flowtoapex;
	Node parent;
	ArcData<N> down;
	ArcData<N> right;
};

/** \struct ApexGrid
 * \brief Represent the grid flow
 */
template< int N , typename Image>
struct ApexGrid {

	Image image;
	const int image_height;
	const int image_width;
	const int maxval;
  
	const int height;
	const int width;
	const int blocksize;
	const unsigned int scanlines;
	
	int state;
	int scan;
	int num_colors;
	int** img;
	unsigned char* range_limit;     //table for range limiting
	
	int** blockmin;
	int** blockmax;
	int* border;
	int* scan_border;
	int** shift_values;
	
	int** offsets;

	NodeData<N>** grid;
	
	Node Apex;
	NodeData<N> apex;
	
	//priority_queue< Node > Q;
	BucketQueue< Node > Q;
	vector< Node > visited;

	int framenumber;
	int framerate;
	bool owner;
	
    /**
     * \brief Constructur
     * \param int h,w: height and width of the image
     * \param int m: maxval
     * \param unsigned int s: scanlines
     **/
	ApexGrid( const int h, const int w, int m, unsigned int s) : image_height(h), image_width(w), maxval(m),height((h+N-1)/N),width((w+N-1)/N), blocksize(N),scanlines(s), Q(2*m+1), framenumber(100000), framerate(100000), owner(true) {
		
		range_limit = range_limit;
		
		blockmin = new int*[height];
		blockmax = new int*[height];
		offsets = new int*[height];
		shift_values = new int*[height];
		
		grid = new NodeData<N>*[height];
		for( int i = 0; i < height; ++i ) {
			
			grid[i] = new NodeData<N>[width];
		}
		
		apex.deficiency = 0;
	  apex.apexcost = 0;
	  apex.potential = 0;
	  apex.distance = INFINITE;
	  Apex.x = -1;
	  Apex.y = -1;
		
		
	}
	
	void init(){
	
		//the whole image
		img = new int*[image_height];
		for(int i = 0; i < image_height; i++){
			img[i] = new int[image_width*num_colors];
		}
		
    //array for calculating differenz between MCU's of two different scans
		border = new int[image_width];
		scan_border = new int[width];
		
		
		for(int i = 0; i < height; i++){
			blockmin[i] = new int[width];
			blockmax[i] = new int[width];
			offsets[i] = new int[width];
			shift_values[i] = new int[width];
		}
		
		for(int bi = 0; bi < height; bi++){
			for(int bj = 0; bj < width; bj++){
				blockmin[bi][bj] = 255;
				blockmax[bi][bj] = 0;
				shift_values[bi][bj] = 0;
			}
		}
	}

	ApexGrid( const Image& img ) : image(img), height((image.n+N-1)/N), width((image.m+N-1)/N), blocksize(N), Q(2*image.maxval+1), framenumber(100000), framerate(100000), owner(true) {
	  blockmin = new int*[height];
	  blockmax = new int*[height];

//	  visited.reserve( height*width );

	  grid = new NodeData<N>*[height];
	  for( int i = 0; i < height; ++i ) {
	    grid[i] = new NodeData<N>[width];
	  }
	  
	  apex.deficiency = 0;
	  apex.apexcost = 0;
	  apex.potential = 0;
	  apex.distance = INFINITE;
	  Apex.x = -1;
	  Apex.y = -1;
	  
	  const int maxval = image.maxval;
	  
	  for( int Bi = 0; Bi < height; ++Bi )
	  {
	    blockmin[Bi] = new int[width];
	    blockmax[Bi] = new int[width];
	  }
	  for( int Bi = 0; Bi < height; ++Bi )
	  {
	    for( int Bj = 0; Bj < width; ++Bj )
	    {
	      blockmax[Bi][Bj] = 0;
	      blockmin[Bi][Bj] = maxval;
	      for( int u = 0; u < N; ++u )
	      {
		for( int v = 0; v < N; ++v )
		{
		  const int i = Bi*N + u;
		  const int j = Bj*N + v;
		  const int r = i < image.n && j < image.m ? image.r(i,j) : 0;
		  if( r < blockmin[Bi][Bj] )
		  {
		    blockmin[Bi][Bj] = r;
		  }
		  if( r > blockmax[Bi][Bj] )
		  {
		    blockmax[Bi][Bj] = r;
		  }
		}
	      }

	      for( int u = 0; u < N; ++u )
	      {
		for( int v = 0; v < N; ++v )
		{
		  const int i = Bi*N + u;
		  const int j = Bj*N + v;
		  if( i < image.n && j < image.m ) {
		    image.r(i,j) -= blockmin[Bi][Bj];
		  }
		}
	      }
	      
	      grid[Bi][Bj].deficiency = 0;
	      grid[Bi][Bj].apexcost = maxval - blockmax[Bi][Bj] + blockmin[Bi][Bj];
	      grid[Bi][Bj].potential = 0;
	      grid[Bi][Bj].distance = INFINITE;
	      
	      Node(Bj,Bi);
	      
	      if( Bj > 0 )
	      {
		for( int u = 0; u < N; ++u )
		{
		  const int i = Bi*N + u;
		  const int j = Bj*N;
		  if( i < image.n && j < image.m ) {
		    grid[Bi][Bj-1].right.difference[u] = image.r(i,j) - image.r(i,j-1);
		  }
		}
		std::sort( grid[Bi][Bj-1].right.difference, grid[Bi][Bj-1].right.difference+N );
	      } else {
		for( int u = 0; u < N; ++u )
		{
		  grid[Bi][width-1].right.difference[u] = 0;
		}
	      }
	      if( Bi > 0 )
	      {
		for( int v = 0; v < N; ++v )
		{
		  const int i = Bi*N;
		  const int j = Bj*N + v;
		  if( i < image.n && j < image.m ) {
		    grid[Bi-1][Bj].down.difference[v] = image.r(i,j) - image.r(i-1,j);
		  }
		}
		std::sort( grid[Bi-1][Bj].down.difference, grid[Bi-1][Bj].down.difference+N );
	      }else {
		for( int u = 0; u < N; ++u )
		{
		  grid[height-1][Bj].down.difference[u] = 0;
		}
	      }
	    }
	  }
// 	  toImage();
// 	  stringstream filename;
// 	  filename << "frame" << framenumber << ".pgm";
// 	  ofstream file( filename.str().c_str() );
// 	  file << image;
// 	  file.close();
// 	  ++framenumber;
// 	  toImage(-1);
	}
	
	void refill_grid(int h, int w, int maxval, int** buffer, int* saved, bool pixelshift){
		
		for( int Bi = 0; Bi < height; ++Bi ){
	    for( int Bj = 0; Bj < width; ++Bj ){
				int t = Bj+Bi*width;
				if(pixelshift){
	      if(saved[t] == 0){
				blockmax[Bi][Bj] = 0;
	      blockmin[Bi][Bj] = maxval;
	      
				for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N; ++v ){
						const int i = Bi*N + u;
						const int j = Bj*N + v;
						if(i < image_height && j < image_width){
							const int r = buffer[i][j];
							if( r < blockmin[Bi][Bj] ){
								blockmin[Bi][Bj] = r;
							}
							if( r > blockmax[Bi][Bj] ){
								blockmax[Bi][Bj] = r;
							}
						}
					}
	      }
				
	      
	      
				
					for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N; ++v ){
						const int i = Bi*N + u;
						const int j = Bj*N + v;
						if( i < image_height && j < image_width ) {

								img[i][j] -= blockmin[Bi][Bj];

						}
					}
	      }
					
					grid[Bi][Bj].deficiency = 0;
					grid[Bi][Bj].apexcost = maxval - blockmax[Bi][Bj] + blockmin[Bi][Bj];
					grid[Bi][Bj].potential = 0;
					grid[Bi][Bj].distance = INFINITE;
				}
				else{
					
					for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N; ++v ){
						const int i = Bi*N + u;
						const int j = Bj*N + v;
						if( i < image_height && j < image_width ) {

								img[i][j] = img[i][j] < 0 ? 0 : img[i][j];
								img[i][j] = img[i][j] > 255 ? 255 : img[i][j];

						}
					}
	      }
					
					grid[Bi][Bj].deficiency = 0;
					grid[Bi][Bj].apexcost = 0;
					grid[Bi][Bj].potential = 0;
					grid[Bi][Bj].distance = INFINITE;
				}
				}
				else{
				
					blockmax[Bi][Bj] = 0;
	      blockmin[Bi][Bj] = maxval;
	      
				for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N; ++v ){
						const int i = Bi*N + u;
						const int j = Bj*N + v;
						if(i < image_height && j < image_width){
							const int r = buffer[i][j];
							if( r < blockmin[Bi][Bj] ){
								blockmin[Bi][Bj] = r;
							}
							if( r > blockmax[Bi][Bj] ){
								blockmax[Bi][Bj] = r;
							}
						}
					}
	      }
				
	      
	      
				
					for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N; ++v ){
						const int i = Bi*N + u;
						const int j = Bj*N + v;
						if( i < image_height && j < image_width ) {

								img[i][j] -= blockmin[Bi][Bj];

						}
					}
	      }
					
					grid[Bi][Bj].deficiency = 0;
					grid[Bi][Bj].apexcost = maxval - blockmax[Bi][Bj] + blockmin[Bi][Bj];
					grid[Bi][Bj].potential = 0;
					grid[Bi][Bj].distance = INFINITE;
				}
	      Node(Bj,Bi);
	      
	      if( Bj > 0 ){
					for( int u = 0; u < N; ++u ){
						const int i = Bi*N + u;
						const int j = Bj*N;
				if( i < image_height && j < image_width ) {
					grid[Bi][Bj-1].right.difference[u] = img[i][j] - img[i][j-1];
				}
			}
			std::sort( grid[Bi][Bj-1].right.difference, grid[Bi][Bj-1].right.difference+N );
	  } else {
		
			for( int u = 0; u < N; ++u ){
				grid[Bi][width-1].right.difference[u] = 0;
			}
	  }
	  if( Bi > 0 ){
			for( int v = 0; v < N; ++v ){
				const int i = Bi*N;
				const int j = Bj*N + v;
				if( i < image_height && j < image_width ) {
					grid[Bi-1][Bj].down.difference[v] = img[i][j] - img[i-1][j];
				}
			}
			std::sort( grid[Bi-1][Bj].down.difference, grid[Bi-1][Bj].down.difference+N );
	  }else {
			for( int u = 0; u < N; ++u ){
				grid[height-1][Bj].down.difference[u] = 0;
			}
	  }
	}
		}}
	
	/**
	 *\brief calculates the borders between blocks
	 * \param int h, w: height and width of the buffer
	 * \param int maxval: maxval
	 * \param int scan
	 * \param unsigned char** buffer: current buffer
	 **/
	void fill_grid(int h, int w,int maxval, int ** buffer){
		
		int bpc = (h+N-1)/N;			//blocks per column
		int bpr = (w+N-1)/N;            //blocks per row
		int val = 0;
		
		for(int Bi = 0; Bi < bpc; Bi++){
			for(int Bj = 0; Bj < bpr; Bj++){
				
				const int ts = Bi+state*bpc;
				const int last_scan = image_height/scanlines;
				
				if(ts >= height)
					break;
				
				Node(Bj,ts);
				
				//right difference
				if(Bj > 0){
					for(int u = 0; u < N; ++u){
						
						const int i = Bi*N + u;
						const int j = Bj*N;
						
						if(i < image_height && j < image_width){
							
							val = buffer[i][j] - buffer[i][j-1];
							grid[ts][Bj-1].right.difference[u] = val;
						}
					}
					std::sort( grid[ts][Bj-1].right.difference, grid[ts][Bj-1].right.difference+N );
				}
				else{
					
					for(int u = 0; u < N; ++u){
								grid[ts][width-1].right.difference[u] = 0;
					}
				}
				
				// down difference
				if(Bi > 0 || state > 0){
					
					const int i = Bi*N;
					
					//first line in buffer but not first scan
					if(Bi == 0 && state > 0){
						for(int v = 0; v < N; ++v){
							
							const int j = Bj*N+v;
							
							if(i < image_height && j < image_width){
								val = buffer[i][j] - border[j];
								grid[ts-1][Bj].down.difference[v] = val;
							}
						}
					}else{//any other line
						for(int v = 0; v < N; ++v){
							
							const int j = Bj*N+v;
							 
							if(i < image_height && j < image_width){
								val = buffer[i][j] - buffer[i-1][j];
								grid[ts-1][Bj].down.difference[v] = val;
							}
						}
					}
					std::sort( grid[ts-1][Bj].down.difference, grid[ts-1][Bj].down.difference+N );
				}
				else{
					for(int v = 0; v < N; ++v){
						
							grid[height-1][Bj].down.difference[v] = 0;
					
					}
				}
				//save the last line in buffer
				if(state < last_scan){
					const int i = scanlines-1;
					for(int v = 0; v < N; ++v){
							
						const int j = Bj*N+v;
						if(i < image_height && j < image_width){
								
							border[j] = buffer[i][j];
						}
					}
				}
			}
		}
		
	}

	
	/**
	 * \brief shifts one block 
	 * \param int i,j: index of block
	 * \param int val: shift values
	 * \param unsigned char** arr: image
	 **/
	void shift_block( int x, int y, int val, int** arr, int c){
		
		for(int u = 0; u < N; u++){
			for(int v = 0; v < N; v++){
				
				const int i = x+u;
				const int j = y+v;
				
				if(j < image_width && i < image_height){
					arr[i][j] = arr[i][j] + val;
					
				}
			}
		}
	}
	
	void shift_block(int x, int y, int val, unsigned char** arr,int a){
		
					for(int u = 0; u < N; u++){
						for(int v = 0; v < N; v++){
							
							const int i = x+u;
							const int j = y+v;
							
							if(j < image_width && i < image_height){
								
								int res = static_cast<unsigned int>(arr[i][j]) + val;
								if(res < 0)
									res = 0;
								if(res > 255)
									res = 255;
								
								arr[i][j] = static_cast<unsigned char>(res);
								
							}
						}
					}
				
	}
	
	/**
	*\brief calculates the min,max of a block
	*\param h,w: height, width 
	*\param i,j: current index i,j
	*\param maxval: maximal greyvalue
	*\param buffer: current buffer
	**/
	void calc_min_max(const int& h, const int& w, int state, int** buffer){

		int bpc = (h+N-1)/N;			//blocks per column
		int bpr = (w+N-1)/N;			//blocks per row

		for(int Bi = 0; Bi < bpc; Bi++){
			for(int Bj = 0; Bj < bpr; Bj++){
				
				for(int u = 0; u < N ; u++){
					for(int v = 0; v < N; v ++){
						
						const int i = Bi*N + u;                             
		  			const int j = Bj*N + v;
						
						int min = 255;
						int max = 0;
						
						if(j < image_width &&  i < image_height){
							min = buffer[i][j];
						
						max = min;
						
						int ts = Bi+bpc*state;
						
						if(ts < height){
		  			if( min < blockmin[ts][Bj] ){
					    blockmin[ts][Bj] = min;
		  			}
					  
					  if( max > blockmax[ts][Bj] ){
					    blockmax[ts][Bj] = max;
		  			}
						}
					}}
				}
				
			}
		}
	}
	
	void shift(int **arr, int x_size, int y_size, int scan, int** blockmin, int* saved, bool pixelshift){

		int bpc = (y_size + N-1)/N;
		int bpr = (x_size + N-1)/N;

	
		for(int Bi = 0; Bi < bpc; ++Bi){
			for(int Bj = 0; Bj < bpr; ++Bj){
				
				int ts = Bi+bpc*state;
				
				int t = Bj+ts*width;
				
				if(pixelshift){
					if(saved[t] == 0){
				
						for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									arr[i][j] = arr[i][j] - blockmin[ts][Bj];
									
									if(arr[i][j] < 0)
										arr[i][j] = 0;
									if(arr[i][j] > 255)
										arr[i][j] = 255;
								}
							}
						}
					}
					else{
						for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									if(arr[i][j] < 0)
										arr[i][j] = 0;
									if(arr[i][j] > 255)
										arr[i][j] = 255;
								}
							}
						}
					}
				}
				else{
					for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									arr[i][j] = arr[i][j] - blockmin[ts][Bj];
									
									if(arr[i][j] < 0)
										arr[i][j] = 0;
									if(arr[i][j] > 255)
										arr[i][j] = 255;
								}
							}
						}
					}
			}
		}
	}
	void shift(unsigned char **arr, int x_size, int y_size, int scan, int** blockmin, int* saved, bool pixelshift){

		int bpc = (y_size + N-1)/N;
		int bpr = (x_size + N-1)/N;

	
		for(int Bi = 0; Bi < bpc; ++Bi){
			for(int Bj = 0; Bj < bpr; ++Bj){
				
				int ts = Bi+bpc*state;
				
				int t = Bj+ts*width;
				
				if(pixelshift){
					if(saved[t] == 0){

						for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									int res = static_cast<int>(arr[i][j]) - blockmin[ts][Bj];
									
									if(res < 0)
										res = 0;
									if(res > 255)
										res = 255;
									
									arr[i][j] = res;
								}
							}
						}
					}
					else{
						for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									if(arr[i][j] < 0)
										arr[i][j] = 0;
									if(arr[i][j] > 255)
										arr[i][j] = 255;
								}
							}
						}
					}
				}
				else{
					for(int u = 0; u < N; u++){
							for(int v = 0; v < N; v++){
								
								const int i = Bi*N + u;
								const int j = Bj*N + v;
								if(i < y_size && j < x_size && ts < height){
									
									int res = static_cast<int>(arr[i][j]) - blockmin[ts][Bj];
									
									if(res < 0)
										res = 0;
									if(res > 255)
										res = 255;
									
									arr[i][j] = res;
									
								}
							}
						}
					}
				
			}
		}
	}
	
	void init_grid(int x_size, int y_size, bool no_rangelimit, bool ori_rangelimit, int* saved, bool pixelshift){
		
		int bpc = (y_size+N-1)/N;
		int bpr = (x_size+N-1)/N;
		
		for(int Bi = 0; Bi < bpc; ++Bi){
			for(int Bj = 0; Bj < bpr; ++Bj){
				
				const int ts = Bi+state*bpc;
				if(ts < height){
					
					int t = Bj+ts*width;
					int apexcost = 0;
					if(pixelshift){
						if(saved[t] == 0){
							apexcost = 255 - blockmax[ts][Bj] + blockmin[ts][Bj];
						}
					}
					else{
						apexcost = 255 - blockmax[ts][Bj] + blockmin[ts][Bj];
					}
					grid[Bi][Bj].deficiency = 0;
					grid[ts][Bj].apexcost = apexcost;
					grid[ts][Bj].potential = 0;
					grid[ts][Bj].distance = INFINITE;
				}
			}
		}
	}
	
	void initialize() {
	  framenumber = 100000;
	  for( int Bi = 0; Bi < height; ++Bi )
	  {
	    for( int Bj = 0; Bj < width; ++Bj )
	    {
	      grid[Bi][Bj].deficiency = 0;
	      grid[Bi][Bj].potential = 0;
	      grid[Bi][Bj].distance = INFINITE;
	    }
	  }
	  
	  apex.deficiency = 0;
	  apex.apexcost = 0;
	  apex.potential = 0;
	  apex.distance = INFINITE;
	  Apex.x = -1;
	  Apex.y = -1;
	}
	
	~ApexGrid() {
	  /*if( owner ) {
	  for( int Bi = 0; Bi < height; ++Bi ) {
			std::cout << Bi << std::endl;
	    if( blockmin[Bi] ) delete[] blockmin[Bi];
	    if( blockmax[Bi] ) delete[] blockmax[Bi];
	  }
	  if( blockmin )
	    delete[] blockmin;
	  if( blockmax )
	    delete[] blockmax;
	  for( int i = 0; i < height; ++i ) {
	    if( grid[i] ) delete[] grid[i];
	  }
	    if( grid )
			delete[] grid;
	  }*/
	}
	
	/**
	 * \brief Reconstructs the image based on the min cost flow solution
	 * \Return true or false, for success, failure respectively
	 */
	bool apextoImage( int sign = 1) {
		
		for( int Bi = 0; Bi < height; ++Bi ){
	    for( int Bj = 0; Bj < width; ++Bj ){
				
				const int offset = apex.potential - grid[Bi][Bj].potential;
				
	      for( int u = 0; u < N; ++u ){
					for( int v = 0; v < N*num_colors; v++ ){
						const int i = Bi*N + u;
						const int j = Bj*N*num_colors + v;
			
						if( i < image_height && j < image_width*num_colors ) {
							img[i][j] += sign*offset;
							offsets[Bi][Bj] = sign*offset;
							if( img[i][j] < 0  ){
								img[i][j] = 0;
							}
							if(img[i][j] > maxval){
								img[i][j] = maxval;
							}
						}
					}
	      }
	    }
	  }
	  
	  return true;
	}

	/**
	 * \brief Reconstructs the image based on the min cost flow solution
	 * \Return true or false, for success, failure respectively
	 */
	bool toImage( int sign = 1 ) {
	  for( int Bi = 0; Bi < height; ++Bi )
	  {
	    for( int Bj = 0; Bj < width; ++Bj )
	    {
	      const int offset = apex.potential - grid[Bi][Bj].potential;
	      for( int u = 0; u < N; ++u )
	      {
		for( int v = 0; v < N; ++v )
		{
		  const int i = Bi*N + u;
		  const int j = Bj*N + v;
			
		  if( i < image.n && j < image.m ) {
		    image.r(i,j) += sign*offset;
		    if( image.r(i,j) < 0  )
		    {
		      cout<< "Error: image.r(i,j) < 0"<<endl;
		      return false;
		    }
		    if(image.r(i,j) > image.maxval)
		    {
		      cout<< "Error: image.r(i,j) > image.maxval"<<endl;
		      return false;
		    }
		  }
		}
	      }
	    }
	  }
	  return true;
	}
	
	
	/**
	 * \brief Return position with the smallest non-negative arc cost otherwise the number of arcs
	 * \param differece : table with arc costs
	 * \return position
	 */
	int MinAvailableDifference(int difference[]  ) const
	{
		for(int k=0; k<N; k++)
		{
			if(difference[k]>=0)				
			{
				return k;
			}
		}
		return N;
	}
	
	/**
	 * \brief Return position in difference table according to the current flow, in order to be used for forward_length calculation
	 * \param flow : current flow
	 * \return Position
	 */
	int ReturnR(int flow) const
  	{
  		return (int)((int)(N+flow)/(int)2);
  	}
  	
  	/**
	 * \brief Return position in difference table according to the current flow, in order to be used for backward_length calculation
	 * \param flow : current flow
	 * \return Position
	 */
  	int ReturnL(int flow) const
  	{
		
  		return (int)((int)(N-flow)/(int)2);
  	}
	
	/**
	 * \brief Return forward length from a left node to rightwards or from an up node to downwards
	 * \param a : position in difference table
	 * \param u : Visited node
	 * \param adjacent: adjacent node of u
	 * \param difference: table with arc costs
	 * \return Forward length
	 */
	int ForwardLength(int a,Node u, Node adjacent, int difference[]) const
	{
		return difference[a]+grid[u.y][u.x].potential - grid[adjacent.y][adjacent.x].potential;
	}
	
	
	/**
	 * \brief Return backward length from a right node to leftwards or from a down node to upwards
	 * \param b : position in difference table
	 * \param u : Visited node
	 * \param adjacent: adjacent node of u
	 * \param difference: table with arc costs
	 * \return Forward length
	 */
	int BackwardLength(int b,Node u, Node adjacent,int difference[]) const
	{
		return -difference[N-1-b] + grid[u.y][u.x].potential - grid[adjacent.y][adjacent.x].potential;
	}
	
	
	/**
	 * \brief Calculates the shortest path from the apex to the rest grid nodes
	 * \param s : Source node
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the shortest path calculation takes place
	 */
	Node Source_ApexNode(const Node &s,const int top, const int bottom, const int left_bound, const int right_bound )
	{
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
			  int cost;
			  if(grid[i][j].flowtoapex>0)
			    cost = - grid[i][j].apexcost + apex.potential - grid[i][j].potential;
			  else
			    cost= apex.potential - grid[i][j].potential;
			  if( cost == 0 && grid[i][j].deficiency < 0 ) {
			    //img assert( apex.distance <= image.maxval );
					assert(apex.distance <= maxval);
			    Q.push( Node( i, j, apex.distance ) );
			    grid[i][j].distance =  apex.distance;
			    grid[i][j].parent.x = -1;
			    grid[i][j].parent.y = -1;
			    return Node(i,j);
			  }
			}
		}
		for (int i=top; i<bottom; i++)
		{
		  for (int j=left_bound; j<right_bound; j++)
		  {
		    
				int cost;
				if(grid[i][j].flowtoapex>0)
					cost = - grid[i][j].apexcost + apex.potential - grid[i][j].potential;
				else
					cost= apex.potential - grid[i][j].potential;
				
				assert(cost>=0);
				const int newdistance = apex.distance + cost;
				//assert( newdistance <= 2*image.maxval );
				assert(newdistance <= 2*maxval);
				if ( newdistance < (grid[i][j].distance) )
				{
					Q.push( Node( i, j, newdistance ) );
					grid[i][j].distance =  newdistance;
					grid[i][j].parent.x = -1;
					grid[i][j].parent.y = -1;
				}
			}
		}
		return Node();
	}
	
	/**
	 * \brief Calculates the shortest path from a node to the rest available adjacent nodes
	 * \param s: Source node
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the shortest path calculation takes place
	 * \return destination, a node with negative deficiency
	 */
	Node shortest_path( const Node &s, const int top, const int bottom, const int left_bound, const int right_bound ) 
	{
		
		int EdgeCost_u_adjacentnode=0;
		visited.clear();
		
		while (!Q.empty()) 
		{
			const Node u = Q.top();
			Q.pop();
			if(u.x==-1)
			{
				if(u.key!=apex.distance)
				{
					//Q.pop();
					continue;
				}
				if( apex.deficiency < 0 ) return Apex;
				const Node t = Source_ApexNode(s,top,bottom,left_bound,right_bound);
				if( t.x != -1 ) return t;
				continue;
			}
			
			if (grid[u.y][u.x].distance != u.key)
			{
				continue;
			}
			
			if( grid[u.y][u.x].deficiency < 0 ) return u;
			
			visited.push_back( u );
			
			if (u.y-1>=top) //Upward adjacent node
			{
				const int l=ReturnL(grid[u.y-1][u.x].down.flow);
				if(l<N )
				{
					EdgeCost_u_adjacentnode = BackwardLength(l,u, Node( u.y-1, u.x), grid[u.y-1][u.x].down.difference);
					assert(EdgeCost_u_adjacentnode>=0);
					grid[u.y-1][u.x].down.backward_length = EdgeCost_u_adjacentnode;
					UpdateDistance(Node(u.y-1,u.x),EdgeCost_u_adjacentnode,u);
				}
			}
			if(u.y+1<bottom) //Downward adjacent node
			{
				const int r=ReturnR(grid[u.y][u.x].down.flow);
				if(r<N )
				{	
					EdgeCost_u_adjacentnode = ForwardLength(r,u, Node(u.y+1,u.x), grid[u.y][u.x].down.difference);
					assert(EdgeCost_u_adjacentnode>=0);
					grid[u.y][u.x].down.forward_length = EdgeCost_u_adjacentnode;
					UpdateDistance(Node(u.y+1,u.x),EdgeCost_u_adjacentnode,u);
				}
			}
			if(u.x-1>=left_bound) //Leftward adjacent node
			{
				const int l=ReturnL(grid[u.y][u.x-1].right.flow);
				if(l<N)
				{
					EdgeCost_u_adjacentnode = BackwardLength(l,u, Node(u.y,u.x-1), grid[u.y][u.x-1].right.difference);
					assert(EdgeCost_u_adjacentnode>=0);
					grid[u.y][u.x-1].right.backward_length = EdgeCost_u_adjacentnode;
					UpdateDistance(Node(u.y,u.x-1),EdgeCost_u_adjacentnode,u);
					
				}
			}
			if(u.x+1<right_bound) //Rightward adjacent node 
			{
				const int r=ReturnR(grid[u.y][u.x].right.flow);
				if(r<N)
				{
					EdgeCost_u_adjacentnode = ForwardLength(r,u, Node(u.y,u.x+1), grid[u.y][u.x].right.difference);
					if(EdgeCost_u_adjacentnode < 0)
						cout << "errrrrrr " << u.x << " " << u.y << endl;
					assert(EdgeCost_u_adjacentnode>=0);
					grid[u.y][u.x].right.forward_length = EdgeCost_u_adjacentnode;
					UpdateDistance(Node(u.y,u.x+1),EdgeCost_u_adjacentnode,u);
					
				}
			}
			
			//Apex adjacent node
			if(grid[u.y][u.x].flowtoapex<0)
				UpdateDistanceApex( grid[u.y][u.x].potential - apex.potential,u);
			else
				UpdateDistanceApex( grid[u.y][u.x].apexcost + grid[u.y][u.x].potential - apex.potential,u);
			
		}
		return s;
	}
	
	/**
	 * \brief Updates the distance of adjacent node from the source
	 * \param EdgeCost_u_adjacentnode: Cost between current visited node and his adjacent node
	 * \param u: Current visited node
	 */
	inline void UpdateDistanceApex( const int& EdgeCost_u_adjacentnode, const Node& u)
	{
		
		const int distance = grid[u.y][u.x].distance + EdgeCost_u_adjacentnode ;
		
		assert(distance>=0);
		
		if ( distance < apex.distance)
			{				
				Apex.key=distance;
				//img assert( distance <= 2*image.maxval );
				assert(distance <= 2*maxval);
				Q.push(Apex);	
				apex.distance =  distance;
				apex.parent.x = u.x;
				apex.parent.y = u.y;
			}
		return;
	}

	/**
	 * \brief Updates the distance of adjacent node from the source
	 * \param adjacent: Adjacent node
	 * \param EdgeCost_u_adjacentnode: Cost between current visited node and his adjacent node
	 * \param u: Current visited node
	 */
	inline void UpdateDistance( const Node& adjacent, const int& EdgeCost_u_adjacentnode, const Node& u)
	{
	  
	  const int distance = grid[u.y][u.x].distance + EdgeCost_u_adjacentnode ;
	  
	  assert(distance>=0);
	  
	  if( distance < grid[adjacent.y][adjacent.x].distance)
	    {
	      //img assert( distance <= 2*image.maxval );
				assert(distance <= 2*maxval);
	      Q.push(Node(adjacent.y,adjacent.x,distance));
	      grid[adjacent.y][adjacent.x].distance =  distance;
	      grid[adjacent.y][adjacent.x].parent.x = u.x;
	      grid[adjacent.y][adjacent.x].parent.y = u.y;
	    }

	    return;
	}
	
	/**
	 * \brief Updates the flow of nodes that consist the shortest path
	 * \param source: source node
	 * \param destination: destination node
	 */
	void UpdateFlowPath(Node source,Node destination)
	{
		Node w = destination;
		
		//Traverse the path in reverse order
		while((w.x!=source.x) || (w.y!=source.y))
		{
			if(w.x==-1)
			{
				const Node v = apex.parent;
				grid[v.y][v.x].flowtoapex++;
				w = v;
			}
			else
			{
				const Node v = grid[w.y][w.x].parent;
				if( v.x == -1 ) {
				  grid[w.y][w.x].flowtoapex--;
				}
				//Rightward
				else if( v.x<w.x && v.y==w.y )
				{
				  grid[v.y][v.x].right.flow++;
				}
				//Downward
				else if(v.x==w.x && v.y<w.y)
				{
				  grid[v.y][v.x].down.flow++;
				}
				//Leftward
				else if(v.x>w.x && v.y==w.y)
				{
				  grid[w.y][w.x].right.flow--;
				}
				//Upward
				else if(v.x==w.x && v.y>w.y)
				{
				  grid[w.y][w.x].down.flow--;
				}
				else
				{
				  cout << "--> Error: wrong coordinates during path traversal <--"<<endl;
				}
				w = v;
			}
		}
		
		return;
	}
	
	
	/**
	 * \brief Initializes the environment for a shortest path calculation, sends a ource to the shortest path calculation, updates information for each grid node concering potentials and deficiencies
	 * \param sources: Vector with available sources
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the sources and destination belong to
	 * \return the maximum apex potential
	 */
	int successive_shortest_path(vector< Node >& sources, const int top, const int bottom, const int left_bound, const int right_bound ) 
	{	
		while(!sources.empty())
		{
			
			Node source = sources.back();
			
			if(source.y>=top && source.y<bottom && source.x>=left_bound && source.x<right_bound)
			{
				if(grid[source.y][source.x].deficiency>0)
				{
					
					grid[source.y][source.x].deficiency-=1;
					grid[source.y][source.x].distance = 0;
					Q.push(Node(source.y,source.x,0));
					
					Node dest = shortest_path(source,top,bottom,left_bound,right_bound);
					
					//Apex node as destination
					if(dest.x==-1)
					{
						assert(apex.deficiency<0);
						apex.deficiency+=1;
						UpdateFlowPath(source,Apex);
						const int distt = apex.distance;
						for( vector< Node >::iterator iter = visited.begin(), end = visited.end(); iter != end; ++iter ) {
							const Node v = *iter;
							if( (grid[v.y][v.x].distance) < INFINITE ) {
								grid[v.y][v.x].potential += grid[v.y][v.x].distance - distt;
							}
							grid[v.y][v.x].distance = INFINITE;
							grid[v.y][v.x].parent.x=v.x;
							grid[v.y][v.x].parent.y=v.y;
							grid[v.y][v.x].parent.key=INFINITE;
							Node(v.y,v.x,INFINITE);
							
						}
						while( !Q.empty() ) {
							const Node v = Q.top();
							grid[v.y][v.x].distance = INFINITE;
							grid[v.y][v.x].parent.x=v.x;
							grid[v.y][v.x].parent.y=v.y;
							grid[v.y][v.x].parent.key=INFINITE;
							Node(v.y,v.x,INFINITE);
							Q.pop();
						}
						
						
					}
					else
					{
						assert(grid[dest.y][dest.x].deficiency<0);
						grid[dest.y][dest.x].deficiency+=1;
						UpdateFlowPath(source,dest);
						const int distt = grid[dest.y][dest.x].distance;
						for( vector< Node >::iterator iter = visited.begin(), end = visited.end(); iter != end; ++iter ) {
							const Node v = *iter;
							if( (grid[v.y][v.x].distance) < INFINITE ) {
								grid[v.y][v.x].potential += grid[v.y][v.x].distance - distt;
							}
							grid[v.y][v.x].distance = INFINITE;
							grid[v.y][v.x].parent.x=v.x;
							grid[v.y][v.x].parent.y=v.y;
							grid[v.y][v.x].parent.key=INFINITE;
							Node(v.y,v.x,INFINITE);
						}
						while( !Q.empty() ) {
							const Node v = Q.top();
							if( v.x != -1 ) {
								grid[v.y][v.x].distance = INFINITE;
								grid[v.y][v.x].parent.x=v.x;
								grid[v.y][v.x].parent.y=v.y;
								grid[v.y][v.x].parent.key=INFINITE;
								Node(v.y,v.x,INFINITE);
							}
							Q.pop();
						}
						if( (apex.distance) < distt ) {
							apex.potential += apex.distance - distt;
							assert(apex.potential == grid[0][0].potential);
						}
						grid[dest.y][dest.x].distance = INFINITE;
						grid[dest.y][dest.x].parent.x=dest.x;
						grid[dest.y][dest.x].parent.y=dest.y;
						grid[dest.y][dest.x].parent.key=INFINITE;
						Node(dest.y,dest.x,INFINITE);
						
					}
					
					apex.distance = INFINITE;
					apex.parent.x = -1;
					apex.parent.y = -1;
					Apex.key = INFINITE;

// 					if( framenumber%framerate == 0 ) {
// 					  toImage();
// 					  stringstream filename;
// 					  filename << "frame" << framenumber << ".pgm";
// 					  ofstream file( filename.str().c_str() );
// 					  file << image;
// 					  file.close();
// 					  toImage(-1);
// 					}
// 					++framenumber;
				}
				else
				{
					sources.pop_back();		
				}
			}
			else
			{
				cout<<"--> Error: Out of bounds source <--" <<endl;
				return apex.potential;
			}
		}
		
		return apex.potential;
	}
	
	/**
	 * \brief Initializes the flow for each grid node
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the flow initialization takes place
	 */
	void FlowInitialization(const int top, const int bottom, const int left_bound, const int right_bound)
	{
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				grid[i][j].flowtoapex = 0;
				
				int r = MinAvailableDifference(grid[i][j].right.difference);
				grid[i][j].right.flow=(2*r)-N;
				
				r =MinAvailableDifference(grid[i][j].down.difference);
				grid[i][j].down.flow=(2*r)-N;
			}
		}
		return;
	}
	
	/**
	 * \brief Initializes the deficiency for each grid node, choosing the available sources
	 * \param Sources: Nodes with positive deficiency
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the deficiency initialization takes place
	 */
	bool DeficiencyInitialization(vector <Node> &Sources,const int top, const int bottom, const int left_bound, const int right_bound)
	{
		int excess = 0;
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				int total_deficiency=0;
				//Upward
				if(i-1>=top)
				{
					total_deficiency+= grid[i-1][j].down.flow;
				}
				//Downward
				if(i+1<bottom)
				{
					total_deficiency-= grid[i][j].down.flow;
				}
				//Leftward
				if(j-1>=left_bound)
				{
					total_deficiency+= grid[i][j-1].right.flow;
				}
				//Rightwarad
				if(j+1<right_bound)
				{
					total_deficiency-= grid[i][j].right.flow;
				}
				
				total_deficiency -= grid[i][j].flowtoapex;
				
				grid[i][j].deficiency = total_deficiency;
				if(total_deficiency>0)
				{
					Sources.push_back(Node(i,j));
					excess += total_deficiency;
				}
			}
		}
		//cout << excess << endl;
		return true;
	}
	
	/**
	 * \brief Starts the algorithm to solve the min cost flow problem
	 * \return true for successfull solution of min cost flow problem
	 */
	bool solve_min_cost_flow( const int& levels ) 
	{
		min_cost_circulation(levels,0,height,0,width);
		
		assert(check(0,height,0,width));
		assert(CheckDeficiencies(0,height,0,width));
		return true;
	}
	
	
	
	
	int success(int top,int bottom, int left_bound, int right_bound)
	{
		cout << "-----"<<endl;
		
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				cout<<"("<<i<<","<<j<<") ";
			}
			cout << endl;
		}
		cout << "-----"<<endl;
		return right_bound;
	}
	
	/**
	 * \brief Solves normally or recursively the min cost flow problem
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the min cost flow problem is solved
	 * \param temp_levels : determines the level of recursion
	 * \return the maximum apex potential
	 */
	int min_cost_circulation(int temp_levels,int top, int bottom,  int left_bound, int right_bound)
	{
		if( ( (right_bound-left_bound)>2 || (bottom-top)>2) && (temp_levels >1))
		{
			
			//Width>height, grid division by width dimension
			if((right_bound-left_bound)>(bottom-top))
			{
				int half_width = (left_bound + right_bound)/2;
				
				apex.potential=0;
				int potential_1 = min_cost_circulation(temp_levels-1,top,bottom,left_bound,half_width);
				
				
				apex.potential=0;
				int potential_2 = min_cost_circulation(temp_levels-1,top,bottom,half_width,right_bound);
				
				
				apex.potential=0;	
				UpdatePotentialsWithMax(potential_1,top,bottom,left_bound,half_width);
				UpdatePotentialsWithMax(potential_2,top,bottom,half_width,right_bound);
				
				
				
				vector <Node> Sources;
				assert(!Sources.size());
				
				VerticalLengthBalance(half_width-1,top,bottom,left_bound,right_bound);
				DeficiencyInitialization(Sources,top,bottom,left_bound,right_bound);
				
				int pot = successive_shortest_path(Sources,top,bottom,left_bound,right_bound);
				assert(!apex.deficiency);
				return pot;
			}
			
			else 
			{
				int half_height = (top + bottom)/2;
				
				apex.potential=0;
				int potential_1 = min_cost_circulation(temp_levels-1,top,half_height,left_bound,right_bound);
				
				
				apex.potential=0;
				int potential_2 = min_cost_circulation(temp_levels-1,half_height,bottom,left_bound,right_bound);
				
				
				apex.potential=0;
				UpdatePotentialsWithMax(potential_1,top,half_height,left_bound,right_bound);
				UpdatePotentialsWithMax(potential_2,half_height,bottom,left_bound,right_bound);
				
				
				
				vector <Node> Sources;
				assert(!Sources.size());
				
				HorizontalLengthBalance(half_height-1,top,bottom,left_bound,right_bound);
				DeficiencyInitialization(Sources,top,bottom,left_bound,right_bound);
				
				int pot = successive_shortest_path(Sources,top,bottom,left_bound,right_bound);
				assert(!apex.deficiency);
				return pot;
			}
		}
		else
		{
			vector <Node> Sources;
			assert(!Sources.size());
			FlowInitialization(top,bottom,left_bound,right_bound);
			DeficiencyInitialization(Sources,top,bottom,left_bound,right_bound);
			apex.deficiency = -TotalDeficiency(top,bottom,left_bound,right_bound);
			return successive_shortest_path(Sources,top,bottom,left_bound,right_bound);
		}
	}
	
	/**
	 * \brief Iterates over all edges that cross the boundary between the 2 parts and modifies the flow such that forward_length and backward_lengthe become non-negative. 
	 * \param half_width: the central point of the grid based on the width
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the boundary balance takes place
	 */
  	void VerticalLengthBalance(int half_width, int top, int bottom,  int left_bound, int right_bound)
  	{
  		const int temp_height= bottom - top;
  		for(int i=0; i<temp_height; i++ )
  		{
  			
  			int r = ReturnR(grid[top + i][half_width].right.flow);
  			grid[top + i][half_width].right.forward_length = grid[top + i][half_width].right.difference[r] +  grid[top + i][half_width].potential - grid[top + i][half_width+1].potential;
  			while( (grid[top + i][half_width].right.forward_length<0 && r<N) )
  			{
  				if(grid[top + i][half_width].right.forward_length<0 && r<N)
  				{
  					grid[top + i][half_width].right.flow++;
					
  				}
				
  				r = ReturnR(grid[top + i][half_width].right.flow);
  				grid[top + i][half_width].right.forward_length = grid[top + i][half_width].right.difference[r] +  grid[top + i][half_width].potential - grid[top + i][half_width+1].potential;
  				if(r==N)
  				{
  					break;
  				}
  				
  				
  			}
  			
  			int l = ReturnL(grid[top + i][half_width].right.flow);
  			grid[top + i][half_width].right.backward_length = -grid[top + i][half_width].right.difference[N-1-l] -  grid[top + i][half_width].potential + grid[top + i][half_width+1].potential;
  			while((grid[top + i][half_width].right.backward_length<0 && l<N))
  			{
  				if (grid[top + i][half_width].right.backward_length<0 && l<N)
  				{
  					grid[top + i][half_width].right.flow--;
  				}
  				
  				l = ReturnL(grid[top + i][half_width].right.flow);
  				grid[top + i][half_width].right.backward_length = -grid[top + i][half_width].right.difference[N-1-l] -  grid[top + i][half_width].potential + grid[top + i][half_width+1].potential;
  				if(l==N)
  				{
  					break;
  				}
  				
  			}
  			
  			assert(grid[top + i][half_width].right.flow>=-N);
  			assert(grid[top + i][half_width].right.flow<=N);
  		}
  		return;
  	}
  	
	
	/**
	 * \brief Iterates over all edges that cross the boundary between the 2 parts and modifies the flow such that forward_length and backward_lengthe become non-negative. 
	 * \param half_height: the central point of the grid based on the height
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the boundary balance takes place
	 */
  	void HorizontalLengthBalance(int half_height, int top, int bottom,  int left_bound, int right_bound)
  	{
  		int temp_width= right_bound - left_bound;
  		for(int i=0; i<temp_width; i++ )
  		{
  			int r = ReturnR(grid[half_height][left_bound + i].down.flow);
  			grid[half_height][left_bound + i].down.forward_length = grid[half_height][left_bound + i].down.difference[r] +  grid[half_height][left_bound + i].potential - grid[half_height+1][left_bound + i].potential;
  			while((grid[half_height][left_bound + i].down.forward_length<0 && r<N) )
  			{
  				if(grid[half_height][left_bound + i].down.forward_length<0 && r<N)
  				{
  					grid[half_height][left_bound + i].down.flow++;
  				}
				
  				r = ReturnR(grid[half_height][left_bound + i].down.flow);
  				grid[half_height][left_bound + i].down.forward_length = grid[half_height][left_bound + i].down.difference[r] +  grid[half_height][left_bound + i].potential - grid[half_height+1][left_bound + i].potential;
  				if(r==N)
  				{
  					break;
  				}
  				
  			}
  			
  			int l = ReturnL(grid[half_height][left_bound + i].down.flow);
  			grid[half_height][left_bound + i].down.backward_length = -grid[half_height][left_bound + i].down.difference[N-1-l] - grid[half_height][left_bound + i].potential + grid[half_height+1][left_bound + i].potential;
  			while((grid[half_height][left_bound + i].down.backward_length<0 && l<N))
  			{
  				if(grid[half_height][left_bound + i].down.backward_length<0 && l<N)
  				{
  					grid[half_height][left_bound + i].down.flow--;
  				}
  				
  				l = ReturnL(grid[half_height][left_bound + i].down.flow);
  				grid[half_height][left_bound + i].down.backward_length = -grid[half_height][left_bound + i].down.difference[N-1-l] - grid[half_height][left_bound + i].potential + grid[half_height+1][left_bound + i].potential;
  				if(l==N)
  				{
  					break;
  				}
  				
				
  			}
  			
  			assert(grid[half_height][left_bound + i].down.flow>=-N);
  			assert(grid[half_height][left_bound + i].down.flow<=N);
  		}
  		return;
  	}
	
	/**
	 * \brief Subtract from each grid node the apex potential
	 * \param apex_potential: current apex potential of the relative grid
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the update of potentials takes place
	 */
  	void UpdatePotentialsWithMax(int apex_potential, int top, int bottom,  int left_bound, int right_bound)
  	{
  		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				grid[i][j].potential = grid[i][j].potential - apex_potential;
			}
		}
		
  		return;
  	}
  	
  	/**
	 * \brief Calculates the total deficiency of the subgrid
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the calculation of total deficiency takes place
	 * \Return total deficiency
	 */
  	int TotalDeficiency(const int top, const int bottom, const int left_bound, const int right_bound)
  	{
  		int total_deficiency = 0;
  		
  		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				total_deficiency += grid[i][j].deficiency;
			}
		}
		
		return total_deficiency;
	}	
	
	/**
	 * \brief Checks the forward and bacward lengths for non-negativity
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the non-negativity check takes place
	 * \Return true or false for non-negativity, negatitity respectively.
	 */
	bool check(const int top, const int bottom, const int left_bound, const int right_bound)
	{
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				
				int r = ReturnR(grid[i][j].right.flow);
				int l = ReturnL(grid[i][j].right.flow);
				if(j+1<right_bound )
				{	
					if(r<N)
					{
						grid[i][j].right.forward_length		= grid[i][j].right.difference[r] +  grid[i][j].potential - grid[i][j+1].potential;
					}
					else
					{
						grid[i][j].right.forward_length = 0;
					}
					
					if(l<N)
					{
						grid[i][j].right.backward_length 	= -grid[i][j].right.difference[N-1-l] -  grid[i][j].potential + grid[i][j+1].potential;
					}
					else
					{
						grid[i][j].right.backward_length=0;
					}
				}
				else
				{
					grid[i][j].right.forward_length=0;
					grid[i][j].right.backward_length=0;
				}
				
				
				r = ReturnR(grid[i][j].down.flow);
				l = ReturnL(grid[i][j].down.flow);
				if(i+1<bottom)
				{
					if(r<N)
					{
						grid[i][j].down.forward_length		= grid[i][j].down.difference[r] +  grid[i][j].potential - grid[i+1][j].potential;
					}
					else
					{
						grid[i][j].down.forward_length = 0;
					}
					
					if(l<N)
					{
						grid[i][j].down.backward_length 	= -grid[i][j].down.difference[N-1-l] -  grid[i][j].potential + grid[i+1][j].potential;
					}
					else
					{
						grid[i][j].down.backward_length=0;
					}
				}
				else
				{
					grid[i][j].down.forward_length =0;
					grid[i][j].down.backward_length=0;
				}
				
				
				
				
				
				int enable = 0;
				if(	grid[i][j].right.forward_length<0)
				{
					cout << "--> Negative Length RF<--"<<" ("<<i<<","<<j<<")"<<endl;
					enable =1;
				}
				
				if(grid[i][j].right.backward_length<0) {
					cout << "--> Negative Length RB<--"<<" ("<<i<<","<<j<<")"<<endl;
					enable =1;
				}
				
				if(grid[i][j].down.forward_length<0)
				{
					cout << "--> Negative Length DF<--"<<" ("<<i<<","<<j<<")"<<endl;
					enable =1;
				}	
				
				if(grid[i][j].down.backward_length<0)
				{
					cout << "--> Negative Length DB<--"<<" ("<<i<<","<<j<<")"<<endl;
					enable =1;
				}
				const int h = apex.potential - grid[i][j].potential;
				if( h < 0 || h > grid[i][j].apexcost || (grid[i][j].flowtoapex < 0 && h != 0 ) ) {
					cout << "apex potential error @" << i <<"," << j << ": "<< h << " flow: " << grid[i][j].flowtoapex << "\n";
				}
				if(enable)
				{	
					return false;
				}
			}
		}
		return true;
	}
	
	
	/**
	 * \brief Checks the deficiencies to be zero after the min cost flow solution
	 * \param bottom,top,left_bound,right_bound : Coordinates of grid in which the deficiency check takes place
	 * \Return true or false for zero, non-zero respectively.
	 */
	bool CheckDeficiencies(const int top, const int bottom, const int left_bound, const int right_bound)
	{
		int total_deficiency=0;
		for (int i=top; i<bottom; i++) 
		{
			for (int j=left_bound; j<right_bound; j++) 
			{
				total_deficiency=0;
				//Upward
				if(i-1>-1)
				{
					total_deficiency+= grid[i-1][j].down.flow;
				}
				//Downward
				if(i+1<height)
				{
					total_deficiency-= grid[i][j].down.flow;
				}
				//Leftward
				if(j-1>-1)
				{
					total_deficiency+= grid[i][j-1].right.flow;
				}
				//Rightwarad
				if(j+1<width)
				{
					total_deficiency-= grid[i][j].right.flow;		
				}
				
				total_deficiency-=grid[i][j].flowtoapex;
				
				if(total_deficiency>0  || total_deficiency<0)
				{
					return false;
				}
			}
		}
		
		assert(!apex.deficiency);
		
		return true;
	}
	
	
	void PrintFinalGrid(void)
	{
		int i,j;
		for ( i=0; i<height; i++)
		{
			for ( j=0; j<width; j++)
			{
				cout<<"("<<i<<","<<j<<")"<< "[Flow:"<< grid[i][j].right.flow << "," <<grid[i][j].down.flow <<"][Pot:"<<  grid[i][j].potential<< "][Dis:" << grid[i][j].distance << "][Par:"<<grid[i][j].parent.y<<","<<grid[i][j].parent.x<<"]\t";
			}
			cout << endl;
		}
		
		cout<< "Apex Pot:"<< apex.potential<<"\n";
		
		return;
	}
};


/** \struct PPMImage
 * \brief Initializes the grid
 */
template< int N , typename Image >
struct InstanceConversion {
	ApexGrid<N,Image> apexgrid;
	
	InstanceConversion( const Image& img ) : apexgrid( img ) {}

	bool toImage() {
	  return apexgrid.toImage();
  }
	    
};


#endif
