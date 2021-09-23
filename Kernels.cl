


inline void AtomicAdd(volatile __local float *source, const float operand){
	union {
		unsigned int intVal;
		float floatVal;
	} newVal;
	union {
		unsigned int intVal;
		float floatVal;
	} prevVal;
	do {
		prevVal.floatVal = *source;
		newVal.floatVal = prevVal.floatVal + operand;
	} while (atomic_cmpxchg((volatile __local unsigned int*)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

__kernel void MatrixMultiply3(__global float* a, __global float* b, __global float* c, int Wa, int Wb) {

	//int Wa = get_global_size(2);
	int row = get_global_id(0);
	int col = get_global_id(1);
	int hmm = get_global_id(2);
	local float *value;
	*value = 0.0f;
	float res = a[row*Wa + hmm] * b[hmm*Wb + col];

	///*if (row == 0 && col == 0) */printf("(%i, %i, %i)  %f +%.4f\n", row, col, hmm,*value,  res);
	AtomicAdd(value, res);
	///if (row == 0 && col == 0 && hmm == 0) printf("(%i, %i, %i) === %f\n", row, col, hmm, *value);
	barrier(CLK_LOCAL_MEM_FENCE);
	//work_group_barrier(CLK_LOCAL_MEM_FENCE);  //Nvidia ìîãåò, à Intel íåò (ñòðàííî)
	//if (/*row == 0 && col == 0 && */hmm == 0)  printf("====================================\n %f + %f =", c[row*Wb + col], *value);
	c[row*Wb + col] += *value;
	//c[row*Wb + col] = *value;
	//if (/*row == 0 && col == 0 &&*/ hmm == 0) printf(" %f\n", c[row*Wb + col]);

	/*õç çà÷åì ýòî âàùå*/
	value = 0;
	local double val;
	val = val + 1;
}

//#include "image.h"
typedef struct pixel_t { unsigned char b,g,r; } pixel_t;

__kernel void erode2( __global pixel_t* img, int W ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	local unsigned char max_value;
		
		max_value = 0x00;
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if(img[row*W+col].r > max_value)
	{
		max_value=img[row*W+col].r;
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
	//work_group_barrier(CLK_LOCAL_MEM_FENCE);  //Nvidia ìîãåò, à Intel íåò (ñòðàííî)
	
		
	int group_row = get_group_id(0);
	int group_col = get_group_id(1);
	
	if(group_row==2 && group_col==2)
	{
		img[row*W+col].r=max_value;
		img[row*W+col].g=max_value;
		img[row*W+col].b=max_value;
	}
}

__kernel void dilate( __global pixel_t* src, __global pixel_t* dst, int W ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	unsigned char max_value = src[row*W+col].r;
	
	
	if(row!=0 && src[(row-1)*W+col].r > max_value)
	{
		max_value = src[(row-1)*W+col].r;
		
	}
	if(col!=0 && src[row*W+col-1].r > max_value)
	{
		max_value = src[row*W+col-1].r;
	}
	if(row!=get_global_size(0)-1 && src[(row+1)*W+col].r > max_value)
	{
		max_value = src[(row+1)*W+col].r;
	}
	if(col!=get_global_size(1)-1 && src[row*W+col+1].r > max_value)
	{
		max_value = src[row*W+col+1].r;
	}
	
	 if(row!=0 && col!=0 && src[(row-1)*W+col-1].r > max_value)
	{
		max_value = src[(row-1)*W+col-1].r;
	}
	if(col!=0 && row!=get_global_size(0)-1 && src[(row+1)*W+col-1].r > max_value)
	{
		max_value = src[(row+1)*W+col-1].r;
	}
	if(row!=get_global_size(0)-1 && col!=get_global_size(1)-1 &&  src[(row+1)*W+col+1].r > max_value)
	{
		max_value = src[(row+1)*W+col+1].r;
	}
	if(col!=get_global_size(1)-1 && row!=0 &&  src[(row-1)*W+col+1].r > max_value)
	{
		max_value = src[(row-1)*W+col+1].r;
	}
	
	
	dst[row*W+col].r=max_value;
	dst[row*W+col].g=max_value;
	dst[row*W+col].b=max_value;
	
}
__kernel void erode( __global pixel_t* src, __global pixel_t* dst, int W ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	unsigned char min_value = src[row*W+col].r;
	
	
	if(row!=0 && src[(row-1)*W+col].r < min_value)
	{
		min_value = src[(row-1)*W+col].r;
	}
	if(col!=0 && src[row*W+col-1].r < min_value)
	{
		min_value = src[row*W+col-1].r;
	}
	if(row!=get_global_size(0)-1 && src[(row+1)*W+col].r < min_value)
	{
		min_value = src[(row+1)*W+col].r;
	}
	if(col!=get_global_size(1)-1 && src[row*W+col+1].r < min_value)
	{
		min_value = src[row*W+col+1].r;
	}
	
	 if(row!=0 && col!=0 && src[(row-1)*W+col-1].r < min_value)
	{
		min_value = src[(row-1)*W+col-1].r;
	}
	if(col!=0 && row!=get_global_size(0)-1 && src[(row+1)*W+col-1].r < min_value)
	{
		min_value= src[(row+1)*W+col-1].r;
	}
	if(row!=get_global_size(0)-1 && col!=get_global_size(1)-1 &&  src[(row+1)*W+col+1].r < min_value)
	{
		min_value = src[(row+1)*W+col+1].r;
	}
	if(col!=get_global_size(1)-1 && row!=0 &&  src[(row-1)*W+col+1].r < min_value)
	{
		min_value = src[(row-1)*W+col+1].r;
	}
	
	
	dst[row*W+col].r=min_value;
	dst[row*W+col].g=min_value;
	dst[row*W+col].b=min_value;
	
}


__kernel void greyscale(__global pixel_t* src, __global pixel_t* dst, int W /*, int H*/) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	double luminosity=0.299  /* 0.221 */*src[row*W+col].r+0.587*src[row*W+col].g+0.114*src[row*W+col].b;
	//double luminosity=0.0f;
	//printf("%f", luminosity);
	dst[row*W+col].r=luminosity;
	dst[row*W+col].g=luminosity;
	dst[row*W+col].b=luminosity;

	
}

__kernel void binarization(__global pixel_t* src, __global pixel_t* dst, int W, int threshold ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	//already greyscaled image
	if(src[row*W+col].r < threshold)
	{
		dst[row*W+col].r=0;
		dst[row*W+col].g=0;
		dst[row*W+col].b=0;	
	}
	else
	{
		dst[row*W+col].r=255;
		dst[row*W+col].g=255;
		dst[row*W+col].b=255;
	}
}


inline unsigned char char2uchar(char arg){
	return arg >= 0 ? arg : 0; 
}

__kernel void subtraction(__global pixel_t* minuend, __global pixel_t* subtrahend, __global pixel_t* res, int W ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	//already greyscaled image
	
	unsigned char newval = char2uchar(minuend[row*W+col].r-subtrahend[row*W+col].r);
	
	res[row*W+col].r=newval;
	res[row*W+col].g=newval;
	res[row*W+col].b=newval;	
	
}

__kernel void central_differences( __global pixel_t* src, __global pixel_t* dst, int W, int threshold ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	double L=0;
	
	if(row!=0 && col!=0 && row!=get_global_size(0)-1 && col!=get_global_size(1)-1)
	{
		double Lx = -0.5*src[(row)*W+col-1].r + 0.5 * src[(row)*W+col+1].r;
		double Ly = -0.5*src[(row+1)*W+col].r + 0.5 * src[(row-1)*W+col].r;
		
		 L = sqrt(pow(Lx,2)+pow(Ly,2));
	}
	
	if(L>threshold)
	{
		dst[row*W+col].r=255;
		dst[row*W+col].g=255;
		dst[row*W+col].b=255;
	}
	else
	{
		dst[row*W+col].r=0;
		dst[row*W+col].g=0;
		dst[row*W+col].b=0;
	}
}
__kernel void sobel( __global pixel_t* src, __global pixel_t* dst, int W, int threshold ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	
	double L=0;
	
	if(row!=0 && col!=0 && row!=get_global_size(0)-1 && col!=get_global_size(1)-1)
	{
		double Lx = -1*src[(row-1)*W+col-1].r -2 * src[(row)*W+col-1].r -1 * src[(row+1)*W+col-1].r;
			  Lx += 1*src[(row-1)*W+col+1].r +2 * src[(row)*W+col+1].r +1 * src[(row+1)*W+col+1].r;
		double Ly = +1*src[(row-1)*W+col-1].r +2 * src[(row-1)*W+col].r +1 * src[(row-1)*W+col+1].r;
			  Ly += -1*src[(row+1)*W+col-1].r -2 * src[(row+1)*W+col].r -1 * src[(row+1)*W+col+1].r;
		 L = sqrt(pow(Lx,2)+pow(Ly,2));
		
		
	}
	
	if(L>threshold)
		{
			dst[row*W+col].r=255;
			dst[row*W+col].g=255;
			dst[row*W+col].b=255;
		}
		else
		{
	
			dst[row*W+col].r=0;
			dst[row*W+col].g=0;
			dst[row*W+col].b=0;
		}
}
__kernel void prewitt( __global pixel_t* src, __global pixel_t* dst, int W, int threshold ) {

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	
	double L=0;
	
	if(row!=0 && col!=0 && row!=get_global_size(0)-1 && col!=get_global_size(1)-1)
	{
		double Lx = -1*src[(row-1)*W+col-1].r -1 * src[(row)*W+col-1].r -1 * src[(row+1)*W+col-1].r;
			  Lx += 1*src[(row-1)*W+col+1].r +1 * src[(row)*W+col+1].r +1 * src[(row+1)*W+col+1].r;
		double Ly = -1*src[(row-1)*W+col-1].r -1 * src[(row-1)*W+col].r -1 * src[(row-1)*W+col+1].r;
			  Ly += +1*src[(row+1)*W+col-1].r +1 * src[(row+1)*W+col].r +1 * src[(row+1)*W+col+1].r;
		 L = sqrt(pow(Lx,2)+pow(Ly,2));
		
		
	}
	
	if(L>threshold)
		{
			dst[row*W+col].r=255;
			dst[row*W+col].g=255;
			dst[row*W+col].b=255;
		}
		else
		{
	
			dst[row*W+col].r=0;
			dst[row*W+col].g=0;
			dst[row*W+col].b=0;
		}
}

__kernel void canny( __global pixel_t* src, __global pixel_t* dst, int W, int maxthreshold, int minthreshold );
__kernel void roberts( __global pixel_t* src, __global pixel_t* dst, int W, int threshold ){

	int row = get_global_id(0);
	int col = get_global_id(1);
	
	double L=0;
	
	if(row!=get_global_size(0)-1 && col!=get_global_size(1)-1)
	{
		double Lx = abs(src[(row)*W+col].r - src[(row+1)*W+col+1].r);
		double Ly = abs(src[(row)*W+col+1].r - src[(row+1)*W+col].r);
		
		 L = sqrt(pow(Lx,2)+pow(Ly,2));
	}
	
	
		dst[row*W+col].r=(int)L;
		dst[row*W+col].g=(int)L;
		dst[row*W+col].b=(int)L;
	
}