/*
 * WritePPM.cpp
 *
 *  Created on: 24.04.2013
 *      Author: Janine
 */

#include "WritePPM.h"


#ifdef PPM_SUPPORTED

#if BITS_IN_JSAMPLE == 8
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) (v)
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#else
#ifdef PPM_NORAWWORD
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) ((v) >> (BITS_IN_JSAMPLE-8))
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#else
/* The word-per-sample format always puts the MSB first. */
#define PUTPPMSAMPLE(ptr,v)			\
	{ register int val_ = v;		\
	  *ptr++ = (char) ((val_ >> 8) & 0xFF);	\
	  *ptr++ = (char) (val_ & 0xFF);	\
	}
#define BYTESPERSAMPLE 2
#define PPM_MAXVAL ((1<<BITS_IN_JSAMPLE)-1)
#endif
#endif


using namespace std;


int WritePPM::start_output_ppm(int colorspace, int width, int height, std::ofstream &output_file){


	switch(colorspace){
	case JCS_GRAYSCALE:
		output_file << "P2\n";
		
		output_file << width << " " << height << "\n";
		output_file << PPM_MAXVAL << "\n";
		
		return JCS_GRAYSCALE;
	case JCS_RGB:
		output_file << "P3\n";
		output_file << width << " " << height << "\n";
		output_file << PPM_MAXVAL << "\n";
		return JCS_RGB;
	default:
		cerr << "UNKNOWN COLORSPACE!";
	}
	
	return 0;
}

void WritePPM::finish_output_ppm(std::ofstream &output_file){

	output_file.close();
}

void WritePPM::jinit_write_ppm(j_decompress_ptr cinfo){

	jpeg_calc_output_dimensions(cinfo);

	JDIMENSION samples_per_row = cinfo->output_width*cinfo->out_color_components;
	this->buffer_width = samples_per_row * (BYTESPERSAMPLE * sizeof(char));
	this->buffer_height = cinfo->image_height;


	buffer = new JSAMPROW[buffer_height];

	for(size_t i = 0; i < buffer_height; i++){
		buffer[i] = new JSAMPLE[buffer_width];
	}



}

void WritePPM::put_pixel_rows(JDIMENSION rows_supplied, std::ofstream &out, jpeg_decompress_struct cinfo){
  
	unsigned int width;
	if(cinfo.out_color_space == JCS_GRAYSCALE){
	  width =  cinfo.image_width;
	}else{
	  width = cinfo.image_width*3;
	}
	for(unsigned int i=0; i < rows_supplied; i++){
		for(unsigned int j = 0; j < width; j++){
			out << static_cast<unsigned int>(buffer[i][j]) << " ";
			//cout << static_cast<unsigned int> (buffer[i][j]) << " ";
		}
		out <<"\n";
		//cout << endl;
	}

}


#endif


