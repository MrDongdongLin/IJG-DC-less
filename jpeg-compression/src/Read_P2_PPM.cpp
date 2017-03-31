#include "Read_P2_PPM.h"

#ifdef PPM_SUPPORTED

/*
 * \brief save the first pixel of block bi,bj
 * \param...
 */


unsigned char** Read_P2_PPM::read_image(j_compress_ptr cinfo, Compression *comp){
	
	FILE * infile = comp->input_file;
	JSAMPLE *ptr;

 	JDIMENSION col;
	
	unsigned char** buffer = new unsigned char*[cinfo->image_height];
	for(unsigned int i = 0; i < cinfo->image_height; i++){
		buffer[i] = new unsigned char[cinfo->image_width];
		ptr = buffer[i];
		
		for (col = cinfo->image_width; col > 0; col--) {
			int i = read_pbm_integer(cinfo, infile);
			JSAMPLE s = rescale[i];
			
			*ptr++ = s;

		}
	}
	
	return buffer;
}

/*
 * int bi, bj: index of block
 * int n: blocksize
 */
void Read_P2_PPM::save_pixel(jpeg_compress_struct* cinfo, int bi, int bj, int n, unsigned char** buffer, int* blocks){
	//cout << "saving pixel" << endl;
	//const int bpc = (cinfo->image_height+n-1)/n;
	const int bpr = (cinfo->image_width+n-1)/n;
	
	//cout << "bpr = " << bpr << " , bpc = " << bpc << endl;
	
	const unsigned int i = bi*n;
	const unsigned int j = bj*n;
	
	assert(i < cinfo->image_height);
	assert(j < cinfo->image_width);
	
	int position = bj+bi*bpr;
	
	if(buffer[i][j] == 0)
		blocks[position] = -2;
	else
		blocks[position] = buffer[i][j];
	
	//cout << "bi = " << bi << " , bj = " << bj << endl; 
 	//cout << "saved pixel: = " << blocks[position] << " on position : " << position << " = (" << bi<< "," << bj << ")" << endl;
	//cout << "failure pixel: = " << blocks[64+15*bpr] << "     " << 64+15*bpr<<endl;
// 	cout << "bi = " << bi << " , bj = " << bj << " ===> " << (int)blocks[position] << endl; 
// 	cout << "u = " << position << endl;
	//cout << endl;
}

unsigned char** Read_P2_PPM::get_pixel_rows(j_compress_ptr cinfo, int begin, int rows, unsigned char** buffer){
	
	unsigned char** buf = new unsigned char*[rows];
	for(int i = 0; i < rows; i++){
		buf[i] = new unsigned char[cinfo->image_width];
	}
	
	for(int i = begin; i < begin+rows; i++){
		buf[i-begin] = buffer[i];
	}
	
	
	//cout << buf[0][0] << endl;
	return buf;
}

// JDIMENSION Read_P2_PPM::get_pixel_rows(j_compress_ptr cinfo, Compression *comp){
// 	
// 	FILE * infile = comp->input_file;
// //	JSAMPROW ptr;
// 	JSAMPLE *ptr;
// 
// 	JDIMENSION col;
// //	unsigned char * b = new (unsigned char[cinfo->image_width+1]);
// 	JSAMPLE *jsampleHelp = new JSAMPLE[cinfo->image_width+1];
// 	JSAMPROW jsamprowHelp = jsampleHelp;
// 	buffer = new JSAMPROW[cinfo->image_width+1];
// 	*buffer = jsamprowHelp;
// 	ptr = buffer[0];
// 
// 	for (col = cinfo->image_width; col > 0; col--) {
// 		int i = read_pbm_integer(cinfo, infile);
// 		JSAMPLE s = rescale[i];
// 		/*Save the first pixel*/
// 		if(!cinfo->fp_saved){
// 			cinfo->firstpix = s;
// 			cinfo->fp_saved = TRUE;
// 			cout << "firstpix = " << (int)s << endl;
// 		}
// 		*ptr++ = s;
// 
// 	}
// //	delete infile;
// 	return 1;
// }

void Read_P2_PPM::start_input(j_compress_ptr cinfo, Compression *comp){
	
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


	cinfo->input_components = 1;
	cinfo->in_color_space = JCS_GRAYSCALE;
//	TRACEMS2(cinfo, 1, JTRC_PPM_TEXT,w , h);
			
	Read_P2_PPM::compute_rescale_array(maxval, cinfo);
//	rescale = compute_rescale_array(maxval,cinfo);

}

#endif /*PPM_SUPPORTED*/
