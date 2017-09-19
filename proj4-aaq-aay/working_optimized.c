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



void calcDepthOptimized(float *depth, float *left, float *right, int imageWidth, int imageHeight, int featureWidth, int featureHeight, int maximumDisplacement) {
	int size = (imageHeight*imageWidth)/4*4;
	__m128 zero = _mm_setzero_ps();
	omp_set_num_threads(8);
	#pragma omp parallel for shared(size)
	for (int i = 0; i < size; i += 4) _mm_store_ps(&(depth[i]), zero);
	#pragma omp parallel for
	for (int i = size; i < imageHeight*imageWidth; i++) depth[i] = 0.0;



	#pragma omp parallel firstprivate (depth, left, right, imageWidth, imageHeight,featureWidth,featureHeight,maximumDisplacement,zero)
	{
	int xmax = imageWidth - featureWidth; 
	int fwidthrem = ((featureWidth*2+1)%4);
	int fwidth = (featureWidth*2+1)-fwidthrem; 
	int fheight = (featureHeight*2+1);
	__m128 difference;
	__m128 mask;
	float ptr[4] = {0};	
	for (int i = 0; i< fwidthrem; i++){
		ptr[i] = 1;
	}
	mask = _mm_loadu_ps(ptr);
	float dysq;
	float dxysq;
	float mindisdxdy=-1;

	#pragma omp for 
	for (int y = featureHeight; y < imageHeight - featureHeight; y++) {
		int bothY = y - featureHeight;
		int index  = y*imageWidth+featureWidth;
		
		for (int x = featureWidth; x < xmax; x++) {

			int bothX = x -featureWidth;
			float minimumSquaredDifference = -1;

			for (int dy = -maximumDisplacement; dy <= maximumDisplacement; dy++)
			{
				dysq = dy*dy;

				for (int dx = -maximumDisplacement; dx <= maximumDisplacement; dx++)
				{

					/* Skip feature boxes that dont fit in the displacement box. */
					if (y + dy - featureHeight < 0 || y + dy + featureHeight >= imageHeight || x + dx - featureWidth < 0 || x + dx + featureWidth >= imageWidth)
					{
						continue;
					}

					int leftdim = (bothY * imageWidth) + bothX;
					int rightdim = (bothY+dy)*imageWidth + bothX + dx;

					dxysq = dysq + dx*dx;

					float squaredDifference = 0;
					__m128 sDifference = zero;
					

					/* Sum the squared difference within a box of +/- featureHeight and +/- featureWidth. */
					for (int i = 0; i < fheight; i++, leftdim+=imageWidth, rightdim+=imageWidth){
						int L = leftdim;
						int R = rightdim;

						for (int j = 0; j < fwidth; j+=4, L+=4, R+=4){
							difference = _mm_sub_ps (_mm_loadu_ps(&(left[L])), _mm_loadu_ps(&(right[R])));
							sDifference = _mm_add_ps(sDifference, _mm_mul_ps(difference,difference));
						}
						if (fwidthrem){
							difference = _mm_mul_ps(_mm_sub_ps (_mm_loadu_ps(&(left[L])), _mm_loadu_ps(&(right[R]))), mask);
							sDifference = _mm_add_ps(sDifference, _mm_mul_ps(difference,difference));

						}
					}
					_mm_store_ps(ptr, sDifference);
					squaredDifference = ptr[0] + ptr[1] + ptr[2] + ptr[3];

					if (minimumSquaredDifference == -1){
						minimumSquaredDifference = squaredDifference;
						mindisdxdy = dxysq;
					} else if (minimumSquaredDifference < squaredDifference) {
						continue;
					} else if ((minimumSquaredDifference>squaredDifference)||(dxysq < mindisdxdy)){
						minimumSquaredDifference = squaredDifference;
						mindisdxdy = dxysq;
					}
				}
			}

			if (minimumSquaredDifference > 0){
				depth[index] = sqrt(mindisdxdy);
			}
			index++;
		}
	}
	}
}
