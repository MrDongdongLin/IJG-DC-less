#ifndef DECOMPRESSION_H_
#define DECOMPRESSION_H_

#ifndef DEFAULT_FMT		/* so can override from CFLAGS in Makefile */
#define DEFAULT_FMT	FMT_PPM
#endif
#ifdef PPM_SUPPORTED


/*
 * For 12-bit JPEG data, we either downscale the values to 8 bits
 * (to write standard byte-per-sample PPM/PGM files), or output
 * nonstandard word-per-sample PPM/PGM files.  Downscaling is done
 * if PPM_NORAWWORD is defined (this can be done in the Makefile
 * or in jconfig.h).
 * (When the core library supports data precision reduction, a cleaner
 * implementation will be to ask for that instead.)
 */

typedef enum {
	FMT_BMP,		/* BMP format (Windows flavor) */
	FMT_GIF,		/* GIF format */
	FMT_OS2,		/* BMP format (OS/2 flavor) */
	FMT_PPM,		/* PPM/PGM (PBMPLUS formats) */
	FMT_RLE,		/* RLE format */
	FMT_TARGA,		/* Targa format */
	FMT_TIFF		/* TIFF format */
} IMAGE_FORMATS;


#if BITS_IN_JSAMPLE == 8
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) (v)
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#else
#ifdef PPM_NORAWWORD
#define PUTPPMSAMPLE(ptr,v)  *ptr++ = (char) ((v) >> (BITS_IN_JSAMPLE-8))
#define BYTESPERSAMPLE 1
#define PPM_MAXVAL 255
#endif
#endif

#include "WritePPM.h"



class Decompression {

public:
	std::ofstream output_file;
	FILE *input_file;
	IMAGE_FORMATS requested_fmt;
	
	jpeg_decompress_struct init_decompress_object();
	int read_input(const char* in, const char* out, const char* option, jpeg_decompress_struct &dinfo, int loop_counter);
	void set_defaults(jpeg_decompress_struct &cinfo);
	int* process_data(jpeg_decompress_struct &cinfo, WritePPM *wp);

};



#endif /*PPM_SUPPORTED*/
#endif /* DECOMPRESSION_H_ */
