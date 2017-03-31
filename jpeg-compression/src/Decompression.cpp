#include "Decompression.h"
#include <sstream>

#ifdef PPM_SUPPORTED

#ifndef DEFAULT_FMT		/* so can override from CFLAGS in Makefile */
#define DEFAULT_FMT	FMT_PPM
#endif

// jpeg_decompress_struct Decompression::init_decompress_object(){
// 	
// 	
// 	
// 	//return cinfo;
// };

int Decompression::read_input(const char* in, const char* out, const char* op, jpeg_decompress_struct &dinfo, int loop_counter){

	//open the input file (jpeglib needs the type FILE*)
	const char* in_path = out;
	const char* option = op;
	const char* out_path;
	string s = in;
	
	int a = loop_counter;
	stringstream ss;
	ss << a;
	string str = ss.str();
	
	size_t pos = s.length()-4;
	string rec = "_recov"+str;
	//cout << rec << endl;
	s.insert(pos,rec);
	out_path = s.c_str();
	
	input_file = fopen(in_path, "r");
	if(input_file == NULL){
		cerr << "Cannot open " << in_path << endl;
		return -1;
	}
	cout << "- inpath = " << in_path << endl;
	
	if(option == NULL){
		//cout << "- no options" << endl;
	}
	else if(!strcmp(option, "-no_rangelimit")){
		cout << "- no range-limiting" << endl;
		dinfo.no_rangelimit = TRUE;

	}
	else if(!strcmp(option, "-ori_rangelimit")){
		cout << "- original range-limiting" << endl;
		dinfo.ori_rangelimit = TRUE;
	}
	else if(!strcmp(option, "-pixelshift")){
		cout << "- pixelshift" << endl;
		dinfo.pixelshift = TRUE;
	}

	//open the outputfile
	output_file.open(out_path);
	dinfo.path = out_path;
	cout << "- outpath = " << out_path << endl;
	
	return 1;
}

void Decompression::set_defaults(jpeg_decompress_struct &cinfo){
	
	jpeg_stdio_src(&cinfo, input_file);							//specify data src for decompression
	
	(void) jpeg_read_header(&cinfo, TRUE);

	jpeg_calc_output_dimensions(&cinfo);

	cinfo.buffered_image = TRUE;
	cinfo.enable_2pass_quant = TRUE;
	cinfo.progressive_mode = TRUE;
	cinfo.rec_outbuf_height = 16;
	cinfo.psaved = FALSE;
	init_qual_mgr(cinfo);

};

int* Decompression::process_data(jpeg_decompress_struct &cinfo, WritePPM *wp){
	
	int rows = 8; 
	int colors = 1;
	if(cinfo.out_color_space == JCS_GRAYSCALE){
    rows = 8;
		colors = 1;
  }
  else if(cinfo.out_color_space == JCS_RGB){
    rows = 16;
		colors = 3;
  }
  else{
    std::cerr << "UNKNOWN TYPE " << cinfo.out_color_space << std::endl;
  }
	
	ApexGrid<8,unsigned char ** > apex(cinfo.image_height, cinfo.image_width, 255, rows);
	apex.num_colors = colors;
	apex.init();
	apex.initialize();

	apex.state = 0;
	apex.scan = 0;
	int count = 0;
	
	cinfo.helper = new int*[cinfo.image_height];
	for(unsigned int i = 0; i < cinfo.image_height; i++){
		cinfo.helper[i] = new int[cinfo.image_width];
	}
	
	while(!jpeg_input_complete(&cinfo)){
	
		apex.scan += 1;
		jpeg_start_output(&cinfo, cinfo.input_scan_number);

		count = 0;
		apex.state = 0;

		while(cinfo.output_scanline < cinfo.output_height){
			
			if(apex.scan <= 2)
				apex.state = -1;
			
			if(count == 2){
				if(apex.scan > 2)
					apex.state += 1;
				
				count = 0;
			}
			
			//The return value is the number of lines actually read
			jpeg_read_scanlines(&cinfo, wp->buffer, cinfo.image_height,apex);
			
	  	count += 2;
		}
		jpeg_finish_output(&cinfo);

	}
	
	apex.scan +=1;
	
	jpeg_start_output(&cinfo, cinfo.input_scan_number);

	count = 0;
	apex.state = 0;
		
	while(cinfo.output_scanline < cinfo.output_height){
		
		if(apex.scan <= 2)
			apex.state = -1;
		
		if(count == 2){
			if(apex.scan > 2)
				apex.state += 1;
				
			count = 0;
		}
		
		jpeg_read_scanlines(&cinfo, wp->buffer, cinfo.image_height,apex);
		
	  count += 2;
	}
	
	int* vals = cinfo.src->qual.saved_pix;
	
	jpeg_finish_output(&cinfo);
	
	wp->finish_output_ppm(output_file);
	//delete wp;
	
	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	fclose(input_file);
	
	return vals;
}

#endif /*PPM_SUPPORTED*/









