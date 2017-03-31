
#ifndef ASSIST_H_
#define ASSIST_H_

#include <iostream>
#include <fstream>
#include <tgmath.h>
#include <string>

//boolean compare(int** buffer, int** buffer)

unsigned char** cast_int_to_uchar(int** buffer, int h, int w){
	
	unsigned char** result = new unsigned char*[h];
	for(int i = 0; i < h; i++){
		result[i] = new unsigned char[w];
	}
	
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			result[i][j] = buffer[i][j] < 0 ? 0 : buffer[i][j];
			result[i][j] = buffer[i][j] > 255 ? 255 : buffer[i][j];
		}
	}
	
	return result;
}

void write_min(int** blockmin, j_decompress_ptr cinfo, const char* s){
	
	ofstream file;
	
	string p = cinfo->path;
	int l = static_cast<int>(p.size());
	char path[l+9];
	strncpy(path,cinfo->path,l-4);
	strcat(path,"_bmin");
	strcat(path,s);
	strcat(path, ".pgm");
	file.open(path);
	int h = (cinfo->image_height+7)/8;
	int w = (cinfo->image_width+7)/8;
	
	file << "P2\n";
	file << w << " " << h << "\n";
	file << 255 << "\n";
	
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			file << blockmin[i][j] << " ";
		}
		file << "\n";
	}
	file.close();
	//file.open()
}

void gamma_correction(double y, int** arr, int height, int width){
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			
			double ex = 1/y;
			
			arr[i][j] = static_cast<unsigned char>(pow(static_cast<double>(arr[i][j]),ex));
			
		}
	}
}

void showBlocks(unsigned char** arr, int height, int width, int y_size, int x_size, std::string path){
	std::ofstream file;
	
	
	
	file.open(path);
	
	int h = (height + y_size-1)/y_size;
	int w = (width + x_size-1)/x_size;
	
	for(int bi = 0; bi < h; bi++){
		for(int bj = 0; bj < w; bj++){
			
			for(int u = 0; u < y_size;u++){
				for(int v = 0; v < x_size; v++){
					
					const int i = bi*y_size+u;
					const int j = bj*x_size+v;
					
					int val = static_cast<int>(arr[i][j]);
					
					if(val < 10)
						file << "  " << val << " ";
					else if(val < 100)
						file << " " << val << " ";
					else 
						file << val << " ";
				}
				file << "\n";
			}
			file << "\n";
		}
		file << "------------------------------------------------------------\n";
	}
// 	for(int i = 0; i < height; i++){
// 		cout << i << endl;
// 		for(int j = 0; j < width; j++){
// 			
// 			if(j == 0)
// 				u = x_size-1;
// 			
// 			int a = static_cast<int>(arr[i][j]);
// 			
// 			if(a < 10)
// 				file << "  " << a << " ";
// 			else if(a < 100)
// 				file << " " << a << " ";
// 			else
// 				file << a << " ";
// 			
// 			if(j == u && i == 6){
// 				file << "\n\n";
// 			}
// 			if(j == width-1 && i == v)
// 				file << "\n ------------------------------------------------------------- \n";
// 			if(j == u){
// 				file << "\n";
// 				//u += x_size;
// 			}
// 			
// 		}
// 	}
	file.close();
}

void printImage(int height, int width, int** buffer){
  for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			std::cout << buffer[i][j] << " ";
	}
		std::cout << std::endl;
	}
	//std::cout << std::endl;
}

void printImage(int height, int width, JSAMPARRAY buffer){

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			std::cout << static_cast<int>(buffer[i][j]) << " ";
	}
		std::cout << std::endl;
	}
  std::cout << std::endl;
}

int calcLum(unsigned char **arr, int x_size, int y_size){
  
  int sum = 0;
  
  for(int i = 0; i < y_size; i++){
    for(int j = 0; j < x_size; j++){
	sum += arr[i][j];
    }
  }
  
  int quo = x_size*y_size;
  
  int res = sum/quo*0.3;
  
  return res;
}



void startOutput(std::string outpath, int width, int height, int type, int maxval){

}

/*void saveImage( unsigned char** image){

	std::ofstream outfile;
	outfile.open(outpath);
	
	std::string s;
	
	switch(type){
	case 1:
		outpath += ".pgm";
		s = "P2\n";
	case 2:
		outpath += ".ppm";
		s = "P3\n";
	default:
		std::cerr << "UNKNOWN COLORSPACE!" << std::endl;
	}
	
	outfile << s << "\n";
	outfile << width << " " << height << "\n";
	outfile << maxval << "\n";
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			outfile << static_cast<unsigned int>(image[i][j]);
		}
		outfile << "\n";
	}
	
	outfile.close();

	
}*/











#endif /* ASSIST_H_ */
