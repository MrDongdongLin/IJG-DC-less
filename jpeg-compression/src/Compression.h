using namespace std;
#ifndef COMPRESSION_H_
#define COMPRESSION_H_


#include <stdio.h>
#include "jpeglib.h"
#include "ReadPPM.h"
#include "string.h"
#include "Decompression.h"



class Compression{
	
public:
	FILE *input_file;
	FILE *output_file;
	const char* in_path;
	const char*out_path;
	const char* decompress_option;
	unsigned char** buf;
	unsigned char** rgbbuf;
	boolean improve;
	int* pixel_to_save;
	
	jpeg_compress_struct init_compress_object();
	int read_input(int argc, const char** argv, jpeg_compress_struct &cinfo, int loop_counter);
	void set_defaults(jpeg_compress_struct &cinfo);
	
	jpeg_decompress_struct init_decompress_object();
	int* decompress(const char* in, const char* out, const char* op, int loop_counter);
	void compress(jpeg_compress_struct &cinfo, boolean trans_data, boolean feedbackloop);
		
};



#endif /*COMPRESSION_H_*/
