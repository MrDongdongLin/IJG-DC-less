#include "Decompression.h"
#include "stdlib.h"

#ifdef PPM_SUPPORTED

#ifndef DEFAULT_FMT		/* so can override from CFLAGS in Makefile */
#define DEFAULT_FMT	FMT_PPM
#endif

int main(int argc, const char **argv){

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JDIMENSION num_scanlines;
	
	Decompression *decomp = new Decompression();
	decomp->requested_fmt = DEFAULT_FMT;

	if(argc < 3){
		cerr << argv[0] << " <input> <output>" << endl; 			// 1 -> one more call of jpeg_read_scanlines, 0 else
		return -1;
	}

	int index = 1;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	
	//open the input file (jpeglib needs the type FILE*)
	const char* in_path = argv[index];
	decomp->input_file = fopen(in_path, "r");
	if(decomp->input_file == NULL){
		cerr << "Cannot open " << in_path << endl;
		return -1;
	}
	index++;
	
	if(argc == 4){
		
		const char* option = argv[index];

		if(!strcmp(option, "-no_rangelimit")){
			cinfo.no_rangelimit = TRUE;
		}
		else if(!strcmp(option, "-ori_rangelimit")){
			cinfo.ori_rangelimit = TRUE;
		}
		else if(!strcmp(option, "-pixelshift")){
			cinfo.pixelshift = TRUE;
		}
		else{
			cerr << "Unknown option " << option << endl;
			return -1;
		}
		index++;
	}
	
	//open the outputfile
	string out_path = argv[index];
	decomp->output_file.open(out_path);
	cinfo.path = argv[index];
	
	jerr.first_addon_message = JMSG_FIRSTADDONCODE;
	jerr.last_addon_message = JMSG_LASTADDONCODE;
	jpeg_stdio_src(&cinfo, decomp->input_file);							//specify data src for decompression
	
	
	(void) jpeg_read_header(&cinfo, TRUE);

	jpeg_calc_output_dimensions(&cinfo);

	cinfo.buffered_image = TRUE;
	cinfo.enable_2pass_quant = TRUE;
	cinfo.progressive_mode = TRUE;
	cinfo.rec_outbuf_height = 16;
	cinfo.psaved = FALSE;

	WritePPM *wp = new WritePPM();
	wp->jinit_write_ppm(&cinfo);

	//start decompression
	jpeg_start_decompress(&cinfo);
	
	wp->start_output_ppm(cinfo.out_color_space, cinfo.image_width, cinfo.image_height, decomp->output_file);
	
	int rows;
	int colors;
	
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
    return -1;
  }
	
	ApexGrid<8,unsigned char ** > apex(cinfo.image_height, cinfo.image_width, 255, rows);
	apex.num_colors = colors;
	apex.init();
	apex.initialize();

	apex.state = 0;
	int count = 0;
	apex.scan = 0;
	
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
			num_scanlines = jpeg_read_scanlines(&cinfo, wp->buffer, cinfo.image_height,apex);
			
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
		
		num_scanlines = jpeg_read_scanlines(&cinfo, wp->buffer, cinfo.image_height,apex);
		
		
			
		wp->put_pixel_rows(num_scanlines,decomp->output_file, cinfo);
		
	  count += 2;
	}
	jpeg_finish_output(&cinfo);
	
	wp->finish_output_ppm(decomp->output_file);
	delete wp;

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(decomp->input_file);
	decomp->output_file.close();

	delete decomp;
	return 0;
}

#endif /*PPM_SUPPORTED*/









