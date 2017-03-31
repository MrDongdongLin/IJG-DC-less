#ifndef READPPM_H_
#define READPPM_H_
#include "Compression.h"
#include "jpeglib.h"
#include <stdio.h>
#include "jerror.h"
#include "cderror.h"

#define U_CHAR unsigned char
#define jinit_read_ppm		jIRdPPM
#define jinit_write_ppm		jIWrPPM

using namespace std;


class ReadPPM
{
	public:
		JSAMPLE *rescale;

		void start_input_ppm(j_compress_ptr cinfo);
		unsigned int read_pbm_integer(j_compress_ptr cinfo, FILE *infile);
		int pbm_getc(FILE *infile);
		JDIMENSION get_text_gray_row(j_compress_ptr cinfo);
		void compute_rescale_array(unsigned int maxval, j_compress_ptr cinfo);
		
	private:
	
};
#endif /*READPPM_H_*/
