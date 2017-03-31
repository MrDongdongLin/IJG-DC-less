#include <iostream>
#include "Compression.h"
#include "Read_P2_PPM.h"
#include "Read_P3_PPM.h"
#include "Assist.h"

using namespace std;

#ifdef PPM_SUPPORTED

int Compression::read_input(int argc, const char** argv, jpeg_compress_struct &cinfo, int loop_counter){
	
	int index;
	const char* option1;
	const char* option2;
	
	if(argc < 3){
		cerr << "usage: " << argv[0] << " <input> <output>";
		return -1;
	}

	index = 1;
	
	//open input file
	in_path = argv[index];
	input_file = fopen(in_path, "r");
	if(input_file == NULL){
		cerr << "Cannot open " << in_path << endl;
		return -1;
	}
	cout << "- inpath = " << in_path << endl;
	
	index++;
	
	/*Compress mode*/
	if(argc >= 4){
		option1 = argv[index];

		if(!strcmp(option1, "-noDC")){
			cinfo.noDC = TRUE;
		}
		else if(!strcmp(option1, "-DC_to_zero")){
			cinfo.DC_to_zero = TRUE;
		}
		else if(!strcmp(option1, "-noDCimprove")){
				cinfo.noDC = TRUE;
				improve = TRUE;
			}
			else if(!strcmp(option1, "-DC_to_zeroimprove")){
				cinfo.DC_to_zero = TRUE;
				improve = TRUE;
			}
		else{
			cerr << "Unknown option " << option1 << endl;
			return -1;
		}
		
		++index;
		
		cout << "- " << option1 << endl;
		
		if(argc >= 5){
			option2 = argv[index];
			if(!strcmp(option1, "-noDC_improve")){
				cinfo.noDC = TRUE;
				improve = TRUE;
			}
			else if(!strcmp(option1, "-DC_to_zero_improve")){
				cinfo.DC_to_zero = TRUE;
				improve = TRUE;
			}
			else{
			cerr << "Unknown option " << option2 << endl;
			return -1;
			}
			index++;

		}
	}
	
	//open output file
	out_path = argv[index];
	output_file = fopen(out_path, "w");
	if(output_file == NULL){
		cerr << "Cannot open " << out_path << endl;
		return -1;
	}
	cout << "- outpath = " << out_path << endl;
	return 1;
}

int* Compression::decompress(const char* in, const char* out, const char* op, int loop_counter){
	
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	WritePPM *wp = new WritePPM();
	Decompression *decomp = new Decompression();
	int* des_img;
	
	decomp->requested_fmt = DEFAULT_FMT;

	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	decomp->read_input(in, out, op, cinfo, loop_counter);
	
	decomp->set_defaults(cinfo);
	cinfo.src->qual.origin = buf;
	cinfo.feedbackloop = TRUE;
	wp->jinit_write_ppm(&cinfo);
	jpeg_start_decompress(&cinfo);
	
	
	wp->start_output_ppm(cinfo.out_color_space, cinfo.image_width, cinfo.image_height, decomp->output_file);
	
	des_img = decomp->process_data(cinfo, wp);

	decomp->output_file.close();

	
	return des_img;
}

void Compression::set_defaults(jpeg_compress_struct &cinfo){
	
	jpeg_set_defaults(&cinfo);
	cinfo.progressive_mode = TRUE;
	jpeg_simple_progression(&cinfo);
	jpeg_default_colorspace(&cinfo);
	
}

void Compression::compress(jpeg_compress_struct &cinfo, boolean trans_data, boolean feedbackloop){
	
	/*Compression*/
	const int N = 8;		//blocksize
	char c;
	int bpc, bpr, length;
	Read_P2_PPM *rdppm2;
	Read_P3_PPM *rdppm3;

	if(getc(input_file) != 'P'){
		ERREXIT(&cinfo, JERR_PPM_NOT);
	}
	c = (int)getc(input_file);
	
	bpc = 0;
	bpr = 0;
	length = 0;
		
	switch(c){
	/*Greyscale image*/
	case '2':
		rdppm2 = new Read_P2_PPM();

		jpeg_stdio_dest(&cinfo, output_file);
		
		cinfo.in_color_space = JCS_GRAYSCALE;
		
		set_defaults(cinfo);

		rdppm2->start_input(&cinfo, this);
		jpeg_default_colorspace(&cinfo); 
		
		bpc = (cinfo.image_height+N-1)/N;	//blocks per column
		bpr = (cinfo.image_width+N-1)/N;	//blocks per row
		length = bpc*bpr;
		
		buf = rdppm2->read_image(&cinfo, this);
		
		if(improve){
			//buffer for pixeltransfer
			cinfo.pvals = new int[length];
			for(int i = 0; i < length; i++){
				cinfo.pvals[i] = 0;
			}

			cinfo.trans_data = trans_data;
			
			if(cinfo.trans_data){
				for(int bi = 0; bi < bpc; bi++){
					for(int bj = 0; bj < bpr; bj++){
						
						int p = bj+bi*bpr;
						
						if(pixel_to_save[p] == 1){
							rdppm2->save_pixel(&cinfo, bi, bj, N, buf, cinfo.pvals);
						}
					}
				}
			}
		}
		jpeg_start_compress(&cinfo, TRUE);
	
		while(cinfo.next_scanline < cinfo.image_height){
		
			unsigned char** b = rdppm2->get_pixel_rows(&cinfo, cinfo.next_scanline, 1, buf);
			(void) jpeg_write_scanlines(&cinfo, b , 1);
		}
		break;
	/*rgb*/
	case '3':
		rdppm3 = new Read_P3_PPM();

		jpeg_stdio_dest(&cinfo, output_file);
		cinfo.in_color_space = JCS_RGB;
		
		set_defaults(cinfo);
		
		if(feedbackloop)
			cinfo.in_color_space = JCS_GRAYSCALE;

		rdppm3->start_input(&cinfo, this);
		
		
		bpc = (cinfo.image_height+N-1)/N;	//blocks per column
		bpr = (cinfo.image_width+N-1)/N;	//blocks per row
		length = bpc*bpr;
		
		buf = rdppm3->read_image(&cinfo, this);
		
		if(improve){
			//buffer for pixeltransfer
			cinfo.pvals = new int[length];
			for(int i = 0; i < length; i++){
				cinfo.pvals[i] = 0;
			}
			
			cinfo.trans_data = trans_data;
			
			if(cinfo.trans_data){
				for(int bi = 0; bi < bpc; bi++){
					for(int bj = 0; bj < bpr; bj++){
						
						int p = bj+bi*bpr;
						
						if(pixel_to_save[p] == 1){
							rdppm3->save_pixel(&cinfo, bi, bj, N, buf, cinfo.pvals);
						}
					}
				}
			}
		}		
		jpeg_start_compress(&cinfo, TRUE);

		while(cinfo.next_scanline < cinfo.image_height){
			unsigned char** b = rdppm3->get_pixel_rows(&cinfo, cinfo.next_scanline, 1, rgbbuf, feedbackloop);
			
			
			(void) jpeg_write_scanlines(&cinfo, b , 1);
		}
		break;
	default:
		ERREXIT(&cinfo, JERR_PPM_NOT);
	}

	
	jpeg_finish_compress(&cinfo);
	
	fclose(input_file);
	fclose(output_file);
	
}


int main(int argc, const char **argv){

	int loop_counter = 1;
		
	struct jpeg_compress_struct cinfo;

	
	Compression *comp = new Compression();	
		
	struct jpeg_error_mgr jerr1;
	cinfo.err= jpeg_std_error(&jerr1);
		
	jpeg_create_compress(&cinfo);

	/*Compression*/
	cout << endl;
	cout << "*****COMPRESSION*****" << endl;
	
	comp->read_input(argc,argv,cinfo, loop_counter);
	
	comp->compress(cinfo, FALSE,comp->improve);
	jpeg_destroy_compress(&cinfo);
	
	if(comp->improve){
		/*Decompression*/
		cout << endl;
		cout << "*****DECOMPRESSION*****" << endl;
		
		int* vals = comp->decompress(comp->in_path, comp->out_path, comp->decompress_option, loop_counter);

		struct jpeg_compress_struct ccinfo;

		
		Compression *c = new Compression();	
			
		struct jpeg_error_mgr jerr2;
		ccinfo.err= jpeg_std_error(&jerr2);
			
		jpeg_create_compress(&ccinfo);

		/*Compression*/
		cout << endl;
		cout << "*****COMPRESSION*****" << endl;
		
		c->read_input(argc,argv,ccinfo, loop_counter);
		
		c->pixel_to_save = vals;
		c->compress(ccinfo, TRUE, FALSE);
		
		jpeg_destroy_compress(&ccinfo);
	}
	
	return 0;
}

#endif /*PPM_SUPPORTED*/