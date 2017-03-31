#ifndef READ_P3_PPM_H_
#define READ_P3_PPM_H_
#ifdef PPM_SUPPORTED

#include "ReadPPM.h"

class Read_P3_PPM : ReadPPM
{
public:

	JSAMPARRAY buffer;

	void start_input(j_compress_ptr cinfo, Compression *comp);
	unsigned char** read_image(j_compress_ptr cinfo, Compression *comp);
	unsigned char** get_pixel_rows(j_compress_ptr cinfo, int begin, int rows, unsigned char** buffer, boolean feedbackloop);
	void save_pixel(j_compress_ptr cinfo, int bi, int bj, int n, unsigned char** buffer, int* blocks);
};

#endif /*PPM_SUPPORTED*/
#endif /*READ_P3_PPM_H_*/
