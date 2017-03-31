#ifndef READ_P2_PPM_H_
#define READ_P2_PPM_H_
#ifdef PPM_SUPPORTED
#include "ReadPPM.h"

class Read_P2_PPM : public ReadPPM
{
public:
//	size_t buffer_height;

	JSAMPARRAY buffer;
	//unsigned char* blocks;

//	JSAMPROW buffer;



	void start_input(j_compress_ptr cinfo, Compression *comp);
	JDIMENSION get_pixel_rows(j_compress_ptr cinfo, Compression *comp);
	void save_pixel(jpeg_compress_struct *cinfo, int bi, int bj, int n, unsigned char** buffer, int* blocks);
	unsigned char** read_image(j_compress_ptr cinfo, Compression *comp);
	unsigned char**get_pixel_rows(j_compress_ptr cinfo, int begin, int rows, unsigned char** buf);
};

typedef Read_P2_PPM *pgm_ptr;
#endif /*PPM_SUPPORTED*/
#endif /*READ_P2_PPM_H_*/
