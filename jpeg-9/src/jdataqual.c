#include "jpeglib.h"
#include "jinclude.h"
#include <algorithm>
#include <vector>

METHODDEF(double) luminance (j_decompress_ptr dinfo, int N, int** offsets){
	
	const double c_1 = (0.01*255.0)*(0.01*255.0);
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	//int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	int m = bpc*bpr;
	
	double mu_x = dinfo->src->qual.mu_x;
	double mu_x_dcfree = dinfo->src->qual.mu_x_dcfree;
	
	
	long int sum_h = 0;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			sum_h += offsets[bi][bj];
		}
	}
	
	double mu_h = sum_h/m;
	
	double mu_y = mu_x_dcfree + mu_h;
	dinfo->src->qual.mu_y = mu_y;

	double luma = (2*mu_x*mu_y + c_1)/(mu_x*mu_x+mu_y*mu_y +c_1);
	
	return luma;
}

METHODDEF(void) calc_fix_vals (j_decompress_ptr dinfo, int** img_dcfree, int** offsets, int N){
	
	
	//int height = dinfo->image_height;
	//int width = dinfo->image_width;
	//int n = height*width;
	
	//int bpc = (height+N-1)/N;
	//int bpr = (width+N-1)/N;
	//int m = bpc*bpr;
	
	dinfo->src->qual.x_sums = dinfo->src->qual.calc_sums_c(dinfo, dinfo->src->qual.origin, N, FALSE);
	dinfo->src->qual.xx_sums = dinfo->src->qual.calc_sums_c(dinfo, dinfo->src->qual.origin, N, TRUE);
	dinfo->src->qual.mu_x = dinfo->src->qual.calc_mu(dinfo, dinfo->src->qual.x_sums, N);
	
	dinfo->src->qual.x_dcfree_sums = dinfo->src->qual.calc_sums(dinfo, img_dcfree,N, FALSE);
	dinfo->src->qual.xx_dcfree_sums = dinfo->src->qual.calc_sums(dinfo, img_dcfree, N, TRUE);
	dinfo->src->qual.mu_x_dcfree = dinfo->src->qual.calc_mu(dinfo, dinfo->src->qual.x_dcfree_sums, N);
	
	
}


METHODDEF(long int**) calc_sums(j_decompress_ptr dinfo, int** arr, int N, boolean pot){
	
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	//int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	
	long int** res = new long int*[bpc];
	for(int bi = 0; bi < bpc; bi++){
		res[bi] = new long int[bpr];
	}
	
	
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			long int sum = 0;
			for(int u = 0; u < N; u++){
				for(int v = 0; v < N; v++){
					
					int i = bi*N+u;
					int j = bj*N+v;
					
					if(i < height && j < width){
					
						if(pot)
							sum += arr[i][j]*arr[i][j];
						else
							sum += arr[i][j];
					}
				}
			}
			
			res[bi][bj] = sum;
			
		}
	}
	return res;
}

METHODDEF(long int**) calc_sums_c(j_decompress_ptr dinfo, unsigned char** arr, int N, boolean pot){
	
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	
	long int** res = new long int*[bpc];
	for(int bi = 0; bi < bpc; bi++){
		res[bi] = new long int[bpr];
	}
	

	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
				long int sum = 0;
			for(int u = 0; u < N; u++){
				for(int v = 0; v < N; v++){
					
					int i = bi*N+u;
					int j = bj*N+v;
					
					if(i < height && j < width){
						if(pot)
							sum += arr[i][j]*arr[i][j];
						else
							sum += arr[i][j];
					}
				}
			}
			
			res[bi][bj] = sum;
			
		}
	}
	return res;
}

METHODDEF(double) calc_mu(j_decompress_ptr dinfo, long int** sums, int N){
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	
	double mu = 0.0;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			mu += sums[bi][bj];
		}
	}
	
	return mu/n;
}

METHODDEF(void) calc_sigma_y (j_decompress_ptr dinfo, int** img, int N){
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	
	double sum = 0.0;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			
			 for(int u = 0; u < N; u++){
				 for(int v = 0; v < N; v++){
					 
					 int i = bi*N+u;
					 int j = bj*N+v;
					 
					 if(i < height && j < width){
						 sum += ((img[i][j] - dinfo->src->qual.mu_y)*(img[i][j] - dinfo->src->qual.mu_y))/(n-1);
					 }
				 }
			 }
		}
	}
	
	dinfo->src->qual.sigma_y = sum;
	
}

METHODDEF(void) calc_sigma_xy (j_decompress_ptr dinfo, int** img,int** offsets, int N){
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	//int m = bpc*bpr;
	
	double sigma_xy = 0.0;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			for(int u = 0; u < N; u++){
				for(int v = 0; v < N; v++){
					
					int i = bi*N+u;
					int j = bj*N+v;
					
					if(i < height && j < width)
						sigma_xy += (dinfo->src->qual.origin[i][j] - dinfo->src->qual.mu_x)*((img[i][j]) - dinfo->src->qual.mu_y)/(n-1);
		
					
				}
			}
		}
	}
	
	dinfo->src->qual.sigma_xy = sigma_xy;
}

METHODDEF(void) calc_sigma_x (j_decompress_ptr dinfo, int N){
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	int n = height*width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	
	double sum = 0.0;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			
			 for(int u = 0; u < N; u++){
				 for(int v = 0; v < N; v++){
					 
					 int i = bi*N+u;
					 int j = bj*N+v;
					 
					 if(i < height && j < width){
						 
						 sum += ((dinfo->src->qual.origin[i][j] - dinfo->src->qual.mu_x)*(dinfo->src->qual.origin[i][j] - dinfo->src->qual.mu_x))/(n-1);
					 }
				 }
			 }
		}
	}
	
	dinfo->src->qual.sigma_x = sum;
}





METHODDEF(double) struc_chrom(j_decompress_ptr dinfo, int** img, int** offsets, int N){
	
	double c_2 = (0.03*255.0)*(0.03*255.0);
	
	calc_sigma_x (dinfo, N);
	calc_sigma_y (dinfo, img, N);
	calc_sigma_xy (dinfo, img, offsets, N);
	
	double res = (2*dinfo->src->qual.sigma_xy+c_2)/(dinfo->src->qual.sigma_x+dinfo->src->qual.sigma_y +c_2);
	
	return res;
}

METHODDEF(void) split_refimg(j_decompress_ptr dinfo, int** img_dcfree, int N){
	int height = dinfo->image_height;
	int width = dinfo->image_width;
	
	int bpc = (height+N-1)/N;
	int bpr = (width+N-1)/N;
	double frac = 1.0/(N*N);
	
	dinfo->src->qual.origin_offsets = new int*[bpc];
	for(int bi = 0; bi < bpc; bi++){
		dinfo->src->qual.origin_offsets[bi] = new int[bpr];
	}
	int min = 255;
	
	for(int bi = 0; bi < bpc; bi++){
		for(int bj = 0; bj < bpr; bj++){
			
			double sum = 0.0;
			
			for(int u = 0; u < N; u++){
				for(int v = 0; v < N; v++){
					
					const int i = bi*N+u;
					const int j = bj*N+v;
					
					
					
					if(i < height && j < width){
						
						sum += frac*(dinfo->src->qual.origin[i][j] - img_dcfree[i][j]);
						
						int val = sum; //dinfo->src->qual.origin[i][j]; //dinfo->src->qual.origin[i][j];// - img_dcfree[i][j];
						if(val < min)
							min = val;
					}
				}
			}
			
			
			dinfo->src->qual.origin_offsets[bi][bj] = sum;
		}
	}
	
}

METHODDEF(boolean) block_compare(int bi, int bj, unsigned char** ref_img, int** des_img, j_decompress_ptr dinfo){

	unsigned int si = bi*8;
	unsigned int sj = bj*8;
	
	if(si < dinfo->image_height && sj < dinfo->image_width){
		int test = static_cast<unsigned int>(ref_img[si][sj])-des_img[si][sj];
		if(test != 0)
			return FALSE;
	}
	return TRUE;
}

METHODDEF(double) calc_mse(unsigned char** ref_img, int** des_img, j_decompress_ptr dinfo){
	
	int h = dinfo->image_height;
	int w = dinfo->image_width;
	int n = h*w;
	
	double sum = 0.0;
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			int c = static_cast<int>(ref_img[i][j]) - des_img[i][j];
			sum += c*c;
		}
	}
	
	double result = sum/n;
	
	return result;
}
// 
bool pairCompare(const pair<int,pair<int,int>> f, const pair<int,pair<int,int>> s){
	
	return f.first < s.first;
}

METHODDEF(int*) calc_diffs(j_decompress_ptr dinfo,int h, int w, int n,int** offsets, int** img, int* saved){
	const int bpc = (h+n-1)/n;
	const int bpr = (w+n-1)/n;
	const int length = bpc*bpr;
	
	dinfo->src->qual.diffs = new int*[3];
	for(int i = 0; i < 3 ;i++){
		dinfo->src->qual.diffs[i] = new int[length];
	}
	
	int diff = 0;
	int bi = 0;
	int bj = 0;
	int* res = new int[2];
	
	for(int i = 0; i < bpc; i++){
		for(int j = 0; j < bpr; j++){
			
			int u = i*n;
			int v = j*n;
			
			int p = j+i*bpr;
			
			if(u < h && v < w && saved[p] == 0){
				int a = dinfo->src->qual.origin_offsets[i][j] - offsets[i][j];
				if(a < 0){
					a = 0-a;
				}
				if(a > diff){
					diff = a;
					bi = i;
					bj = j;
					
				}
			}
		}
	}
	
	res[0] = bi;
	res[1] = bj;
	
	return res;
	
}

METHODDEF(double) origin_ssim(j_decompress_ptr dinfo,  unsigned char** x_arr,int** y_arr){
	
	double c_1 = (0.01*255.0)*(0.01*255.0);
	double c_2 = (0.03*255.0)*(0.03*255.0);
	
	double* mu = dinfo->src->qual.origin_mu(dinfo, x_arr, y_arr);
	double mu_x = mu[0];
	double mu_y = mu[1];
	
	
	double* sigma = dinfo->src->qual.origin_sigma(dinfo, x_arr, y_arr, mu_x, mu_y);
	double sigma_x = sigma[0];
	double sigma_y = sigma[1];
	double sigma_xy = sigma[2];
	
	
	double luma = (2*mu_x*mu_y + c_1)/(mu_x*mu_x+mu_y*mu_y+c_1);
	double ch_st = (2*sigma_xy+c_2)/(sigma_x+sigma_y+c_2);
	double ssim = luma * ch_st;
	return ssim;
}

METHODDEF(double*) origin_sigma(j_decompress_ptr dinfo,  unsigned char** x_arr, int** y_arr, double mu_x, double mu_y){
	unsigned int h = dinfo->image_height;
	unsigned int w = dinfo->image_width;
	double n = h*w;
	double x_sum = 0.0;
	double y_sum = 0.0;
	double xy_sum = 0.0;
	
	for(unsigned int i = 0; i < h; i++){
		for(unsigned int j = 0; j < w; j++){
			double xdiff = static_cast<double>(x_arr[i][j]) - mu_x;
			double ydiff = static_cast<double>(y_arr[i][j]) - mu_y;
			x_sum += (1.0/(n-1.0))*(xdiff*xdiff);
			y_sum += (1.0/(n-1.0))*(ydiff*ydiff);
			xy_sum += (1.0/(n-1.0))*(xdiff*ydiff);
		}
	}
	
	double* res = new double[3];
	res[0] = x_sum;
	res[1] = y_sum;
	res[2] = xy_sum;
	
	return res;
	
}

METHODDEF(double*) origin_mu(j_decompress_ptr dinfo,  unsigned char** x_arr, int** y_arr){
	
	unsigned int h = dinfo->image_height;
	unsigned int w = dinfo->image_width;
	double n = h*w;
	double x_sum = 0.0;
	double y_sum = 0.0;
	
	for(unsigned int i = 0; i < h; i++){
		for(unsigned int j = 0; j < w; j++){
			x_sum += 1.0/n*static_cast<double>(x_arr[i][j]);
			y_sum += 1.0/n*static_cast<double>(y_arr[i][j]);
		}
	}
	
	double mu_x = x_sum;
	double mu_y = y_sum;
	double* res = new double[2];
	res[0] = mu_x;
	res[1] = mu_y;
	return res;
}

GLOBAL(void) init_qual_mgr (jpeg_decompress_struct &dinfo) {

	dinfo.src->qual.calc_mu = calc_mu;
	dinfo.src->qual.calc_sums = calc_sums;
	dinfo.src->qual.calc_sums_c = calc_sums_c;
	dinfo.src->qual.calc_sigma_x = calc_sigma_x;
	dinfo.src->qual.calc_sigma_y = calc_sigma_y;
	dinfo.src->qual.calc_sigma_xy = calc_sigma_xy;
 	dinfo.src->qual.calc_mse = calc_mse;
 	dinfo.src->qual.calc_diffs = calc_diffs;
	dinfo.src->qual.origin_mu = origin_mu;
	dinfo.src->qual.origin_sigma = origin_sigma;
	dinfo.src->qual.origin_ssim = origin_ssim;
	dinfo.src->qual.block_compare = block_compare;
	dinfo.src->qual.luminance = luminance;
	dinfo.src->qual.struc_chrom = struc_chrom;
	dinfo.src->qual.calc_fix_vals = calc_fix_vals;
	dinfo.src->qual.split_refimg = split_refimg;
// 	
// 	dinfo.src->qual.calc_mu_i = calc_mu_i;
//  	dinfo.src->qual.calc_mu_c = calc_mu_c;
// 	
// 	dinfo.src->qual.calc_rec_muh = calc_rec_muh;
// 	dinfo.src->qual.calc_mu_blockwise_i = calc_mu_blockwise_i;
// 	dinfo.src->qual.calc_mu_blockwise_c = calc_mu_blockwise_c;
// 	dinfo.src->qual.calc_luma_min = calc_luma_min;
	
// 	dinfo.src->qual.calc_chroma_min = calc_chroma_min;
	
	
}
 

