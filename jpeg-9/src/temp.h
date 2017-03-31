#ifndef TEMP_H
#define TEMP_H

template<typename Buffer>
struct ImageFromBuffer{

	int position;

	int n;					//number of lines
	int m;					//number of columns

	int maxval;
	
	int **blockmin; 
	int **blockmax;
	
	ImageFromBuffer( int n, int m, int maxval) : n(n), m(m), maxval(maxval){
		
		blockmin = new int*[n];
		blockmax = new int*[n];
		
		int blocksize = 8;
			
		int bpc = (n+blocksize-1)/blocksize;			//blocks per column
		
		for(int i = 0; i < bpc; i++){
			blockmin[i] = new int[m];
			blockmax[i] = new int[m];
		}
	};

//   unsigned char& r( const int& i, const int& j ){
		// assert( 0 <= i );
// 		assert( i < n );
		//return buffer[j][i];
// 	}

};


#endif /*TEMP_H*/