// CS 61C Fall 2015 Project 4

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

// include OpenMP
#if !defined(_MSC_VER)
#include <pthread.h>
#endif
#include <omp.h>

#include "calcDepthOptimized.h"
#include "calcDepthNaive.h"

/* DO NOT CHANGE ANYTHING ABOVE THIS LINE. */
#include <math.h>
void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement){
	__m128 zero = _mm_setzero_ps();
	int size = (imageHeight*imageWidth);
	int size_four = size/4*4;
	
	#pragma omp parallel for firstprivate(size_four)
	for (int i = 0; i < size_four; i += 4) _mm_store_ps(&(depth[i]), zero);
	for (int i = size_four; i < size; i++) depth[i] = 0;

	omp_set_num_threads(8);
	omp_set_nested(1);
	omp_set_dynamic(1);
	#pragma omp parallel firstprivate(depth,left,right,imageWidth,imageHeight,featureWidth,featureHeight,maximumDisplacement)
	{
	float squaredDifference, minimumSquaredDifference, ptr[4] = {0};
	float* lptr;
	float* rptr;
	int ycap = imageHeight - featureHeight, xcap = imageWidth - featureWidth;
	int dy2, dxy2, min_dxy2, index, ymfh, ypfh, xmfw, xpfw, l_adj, r_adj;
	int fw2 = featureWidth*2+1, fh2 = featureHeight*2+1;
	int rem1 = fh2%4, rem2 = fw2%4;
	int LR_adj = imageWidth - fw2 + rem2;
	int fheight = fh2 - rem1, fwidth = fw2 - rem2;
	for (int i = 0; i < rem2; i++) ptr[i] = 1;
	__m128 zero = _mm_setzero_ps(), sum, mask = _mm_loadu_ps(ptr);
	#pragma omp for
	for (int y = featureHeight; y < ycap; y++) {
		index = y*imageWidth+featureWidth;
		ymfh = y - featureHeight;
		ypfh = y + featureHeight;
		for (int x = featureWidth; x < xcap; x++) {      
			xmfw = x - featureWidth;
			xpfw = x + featureWidth;
			minimumSquaredDifference = -1;
			min_dxy2 = 0;
			l_adj = ymfh*imageWidth + xmfw;
			r_adj = (ymfh - maximumDisplacement)*imageWidth + xmfw;                                      
			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++, r_adj+=imageWidth) {
				if (ymfh + dy < 0 || ypfh + dy >= imageHeight) continue; 
				dy2 = dy*dy;                
				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++) {   
					if (xmfw + dx < 0 || xpfw + dx >= imageWidth) continue;
					sum = zero;
					squaredDifference = 0;
					dxy2 = dy2 + dx*dx;
					lptr = left+l_adj, rptr = right+r_adj+dx;       
					for (int i = 0; i < fheight; i+=4) {                                          
						for (int j = 0; j < fwidth; j+=4, lptr+=4, rptr+=4) {                           
							__m128 load = _mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr));
							sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); }
						__m128 load = _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr)), mask);
						sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); 
						lptr+=LR_adj, rptr+=LR_adj;
						for (int j = 0; j < fwidth; j+=4, lptr+=4, rptr+=4) {                           
							__m128 load = _mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr));
							sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); }
						load = _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr)), mask);
						sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); 
						lptr+=LR_adj, rptr+=LR_adj;
						for (int j = 0; j < fwidth; j+=4, lptr+=4, rptr+=4) {                           
							__m128 load = _mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr));
							sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); }
						load = _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr)), mask);
						sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); 
						lptr+=LR_adj, rptr+=LR_adj;
						for (int j = 0; j < fwidth; j+=4, lptr+=4, rptr+=4) {                           
							__m128 load = _mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr));
							sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); }
						load = _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr)), mask);
						sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); 
						lptr+=LR_adj, rptr+=LR_adj;
					}
					for (int i = 0; i < rem1; i++, lptr+=LR_adj, rptr+=LR_adj) {
							for (int j = 0; j < fwidth; j+=4, lptr+=4, rptr+=4) {                           
							__m128 load = _mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr));
							sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); }
						__m128 load = _mm_mul_ps(_mm_sub_ps(_mm_loadu_ps(lptr), _mm_loadu_ps(rptr)), mask);
						sum = _mm_add_ps(sum, _mm_mul_ps(load, load)); 
					}
					_mm_store_ps(ptr, sum);
					squaredDifference = ptr[0] + ptr[1] + ptr[2] + ptr[3];
					if (minimumSquaredDifference == -1) {
						minimumSquaredDifference = squaredDifference;
						min_dxy2 = dxy2;
					} 
					else if (minimumSquaredDifference < squaredDifference) continue;
					else if ((minimumSquaredDifference > squaredDifference) || (dxy2 < min_dxy2)) {
						minimumSquaredDifference = squaredDifference;
						min_dxy2 = dxy2;
					}
				}
			}
			if (minimumSquaredDifference > 0) 
				depth[index] = sqrt(min_dxy2);
			index++;
		}
	}
}
}
