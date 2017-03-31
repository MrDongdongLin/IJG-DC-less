

#ifndef RECURSION_H_
#define RECURSION_H_

// template<typename Buffer>
// struct ImageFromBuffer{
// 
// 	int position;
// 
// 	int n;					//number of lines
// 	int m;					//number of columns
// 
// 	int maxval;
// 	Buffer buffer;
// 	
// 	int **blockmin; 
// 	int **blockmax;
// 	
// 	ImageFromBuffer( int n, int m, int maxval) : n(n), m(m), maxval(maxval){
// 		
// 		blockmin = new int*[n];
// 		blockmax = new int*[n];
// 		
// 		int blocksize = 8;
// 			
// 		int bpc = (n+blocksize-1)/blocksize;			//blocks per column
// 		
// 		for(int i = 0; i < bpc; i++){
// 			blockmin[i] = new int[m];
// 			blockmax[i] = new int[m];
// 		}
// 	};
// 
// 	unsigned char& r( const int& i, const int& j )
// 	{
// 		assert( 0 <= i );
// 		assert( i < n );
// 		return buffer[j][i];
// 	}
// 
// };

//n = number of lines
//m = number of columns
//template <typename Buffer>
void recover(int n, int m, int maxval, ApexGrid<8,unsigned char ** > apexgrid) {
	int l = 1;
	int r = 1;
	
 	bool success = false;
 	//clock_t start = clock();
 	for( int rr = 0; rr < r; ++rr ) {
 
 		//apexgrid.initialize();
 		success = apexgrid.solve_min_cost_flow(l);
 	}
 	//clock_t end = clock();
  	if( success ) {
  
  		//const double diff = (end - start) / static_cast<double>( CLOCKS_PER_SEC );
  	  
			//cout << apexgrid.image_height*apexgrid.image_width/64 << " " << l << " " << diff << " " << apexgrid.Q.pushes << endl;
				
     	    if( !apexgrid.apextoImage() ) {
     
     	      cerr << "Failure in constructing image from instance!" << endl;
     	      return;
     	    }
  
  	}
  	else
  	{
  		cout<<"Failed"<<endl;
  	}




}

#endif /* RECURSION_H_ */
