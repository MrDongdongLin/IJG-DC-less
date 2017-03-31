#ifndef WRITEPPM_H_
#define WRITEPPM_H_

#define jinit_write_ppm		jIWrPPM

#include "jinclude.h"
#include "jpeglib.h"
#include <iostream>
#include <fstream>

class WritePPM {

public:

	JSAMPARRAY buffer;
	void put_pixel_rows(JDIMENSION rows_supplied, std::ofstream &output_file, jpeg_decompress_struct cinfo);
	int start_output_ppm(int colorspace, int width, int height,std::ofstream &output_file);
	void finish_output_ppm(std::ofstream &output_file);
	void jinit_write_ppm(j_decompress_ptr cinfo);

private:

	JSAMPROW pixrow;
	JDIMENSION samples_per_row;
	JDIMENSION buffer_height;
	std::ofstream *output_file;
	unsigned char *iobuffer;
	int *buffer_help;
	size_t buffer_width;


};

#endif /* WRITEPPM_H_ */
