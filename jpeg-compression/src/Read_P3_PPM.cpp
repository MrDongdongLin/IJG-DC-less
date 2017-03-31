#include "Read_P3_PPM.h"
#ifdef PPM_SUPPORTED

unsigned char** Read_P3_PPM::read_image(j_compress_ptr cinfo, Compression *comp){
	
	FILE * infile = comp->input_file;
 	JSAMPLE *ptr;
 
 	JDIMENSION col;
 
 	unsigned char** buffer = new unsigned char*[cinfo->image_height];
	comp->rgbbuf = new unsigned char*[cinfo->image_height];
	for(unsigned int i = 0; i < cinfo->image_height; i++){
		buffer[i] = new unsigned char[cinfo->image_width];
		comp->rgbbuf[i] = new unsigned char[cinfo->image_width*3];
	
		ptr = comp->rgbbuf[i];
	
		for (col = cinfo->image_width; col > 0; col--) {
		
 	
			unsigned char r = rescale[read_pbm_integer(cinfo, infile)];
			unsigned char g = rescale[read_pbm_integer(cinfo, infile)];
			unsigned char b = rescale[read_pbm_integer(cinfo, infile)];
			*ptr++ = r;
			*ptr++ = g;
			*ptr++ = b;
			
			
		}
	}
	
	for(unsigned int i = 0; i < cinfo->image_height; i++){
		int v = 0;
		
		for(unsigned int j = 0; j < cinfo->image_width*3; j=j+3){
			unsigned char r = comp->rgbbuf[i][j];
			unsigned char g = comp->rgbbuf[i][j+1];
			unsigned char b = comp->rgbbuf[i][j+2];
			buffer[i][v] = static_cast<unsigned char>( r*0.2990 + g*0.5870 + b*0.1140);
			
			v++;
		}
	}
	
	
 	return buffer;

}

void Read_P3_PPM::save_pixel(j_compress_ptr cinfo, int bi, int bj, int n, unsigned char** buffer, int* blocks){
	
	int nw = 3*n;
	unsigned int width = cinfo->image_width*3;
	
	const int bpr = (cinfo->image_width+n-1)/n;
	
	const unsigned int i = bi*n;
	const unsigned int j = bj*nw;
	
	assert(i < cinfo->image_height);
	assert(j < width);
	
	int r = buffer[i][j];
	int g = buffer[i][j+1];
	int b = buffer[i][j+2];
		
	int y = r*0.2990 + g*0.5870 + b*0.1140;

	
	int position = bj+bi*bpr;

	if(y == 0)
		blocks[position] = -2;
	else
		blocks[position] = y;
	
	if(bi == 0 && bj == 68){
		cout << "pos = " << position << endl;
		cout << blocks[position] << endl;
	}
}

unsigned char** Read_P3_PPM::get_pixel_rows(j_compress_ptr cinfo, int begin, int rows, unsigned char** buffer, boolean feedbackloop){
	
	int scale = 3;
	if(feedbackloop)
		scale = 1;
	
	unsigned char** buf = new unsigned char*[rows];
	for(int i = 0; i < rows; i++){
		buf[i] = new unsigned char[cinfo->image_width*scale];
	}
	
	for(int i = begin; i < begin+rows; i++){
		buf[i-begin] = buffer[i];
	}
	
	return buf;
}

void Read_P3_PPM::start_input(j_compress_ptr cinfo, Compression *comp){
	
	unsigned int w, h, maxval;
	
	/* fetch the remaining header info */
	w = read_pbm_integer(cinfo, comp->input_file);
	h = read_pbm_integer(cinfo, comp->input_file);
	maxval = read_pbm_integer(cinfo, comp->input_file);

	if (w <= 0 || h <= 0 || maxval <= 0) /* error check */
	  ERREXIT(cinfo, JERR_PPM_NOT);

	cinfo->data_precision = BITS_IN_JSAMPLE; /* we always rescale data to this */
	cinfo->image_width = (JDIMENSION) w;
	cinfo->image_height = (JDIMENSION) h;

	cinfo->input_components = 3;
	cinfo->in_color_space = JCS_RGB;
//	TRACEMS2(cinfo, 1, JTRC_PPM_TEXT,w , h);
	

	Read_P3_PPM::compute_rescale_array(maxval, cinfo);
			
//	if(need_rescale){
//		rescale = compute_rescale_array(maxval,cinfo);
//	}
}

#endif /*PPM_SUPPORTED*/
