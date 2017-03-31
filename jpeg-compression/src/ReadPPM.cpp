#include "ReadPPM.h"

#ifdef PPM_SUPPORTED


///**
// * Read next char, skipping over any comments
// * A comment/newline sequence is returned as a newline
// * **/
//
//int pbm_getc(FILE *infile){
//
//	int ch;
//
//	ch = getc(infile);
//	if(ch == '#'){
//		do{
//			ch = getc(infile);
//		}while(ch != '\n' && ch != EOF);
//	}
//	return ch;
//}


/**
 * Read an unsigned decimal integer from the PPM file
 * Swallows one trailing character after the integer
 * Note that on a 16-bit-int machine, only values up to 64k can be read.
 * This should not be a problem in practice.
 * **/
unsigned int ReadPPM::read_pbm_integer(j_compress_ptr cinfo, FILE *infile){

	int ch;
	unsigned int val;
	
	//skip any leading whitespace
	do{
		ch = pbm_getc(infile);
		if(ch == EOF)
			ERREXIT(cinfo, JERR_INPUT_EOF);
		}while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
	
	
	if(ch < '0' || ch > '9'){
		ERREXIT(cinfo, JERR_PPM_NONNUMERIC);
	}
	
	val = ch - '0';
	while((ch = pbm_getc(infile)) >= '0' && ch <= '9'){
		val *= 10;
		val += ch - '0';
	}
	return val;
}

//allocate space for I/O buffer: 1 or 3 bytes of words/pixel
//U_CHAR* ReadPPM::allocateIO(unsigned int maxval, j_compress_ptr cinfo){
//	
//	size_t buffer_width;
//	int i;
//	if(maxval <= 255){
//		i = SIZEOF(U_CHAR);
//	}
//	else{
//		i = 2* SIZEOF(U_CHAR); 
//	}
//	
//	buffer_width = (size_t) w * input_components * i;
//	return ((U_CHAR*)(*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_IMAGE, buffer_width));
//	
//}

int ReadPPM::pbm_getc(FILE *infile){
	
	register int ch;
	
	ch = getc(infile);
	if(ch == '#'){
		do{
			ch = getc(infile);
		}while(ch != '\n' && ch != EOF);
	}
	return ch;
}

//compute the rescaling array		JSAMPLE*
void ReadPPM::compute_rescale_array(unsigned int maxval, j_compress_ptr cinfo){

		INT32 val, half_maxval;

		//(unsigned) char - jsample

		// on 16-bit-int machines we have to be careful of maxval = 65535
		//long i = (size_t)(((long) maxval + 1L)* sizeof(JSAMPLE));

		//alloc_small --> jmemmgr.c
//		jpeg_memory_mgr* bla = cinfo->mem;
//		auto a = bla->alloc_small;
//
//		j_common_ptr s = (j_common_ptr) cinfo;
//
//		rescale = static_cast<JSAMPLE *>( (a)(s, JPOOL_IMAGE, i) );
		rescale = new JSAMPLE[maxval+1];
		half_maxval = maxval/2;
			
		for(val = 0; val <= (INT32) maxval; val++){
			/* The multiplication here must be done in 32 bits to avoid overflow */
			rescale[val] = (JSAMPLE) ((val*MAXJSAMPLE + half_maxval)/maxval);

		}
}

#endif /*PPM_SUPPORTED*/
