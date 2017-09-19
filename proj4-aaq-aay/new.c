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
	float fdepth[imageHeight*imageWidth];
	#pragma omp for 
	for (int y = featureHeight; y < imageHeight - featureHeight; y++) {
		for (int x = featureWidth; x < xmax; x++) {
			for (int fy = y-featureHeight; fy<=y+featureHeight; fy++){
				for (int fx = x-featureWidth; fx<=x+featureWidth; fx++){

				}
			}
		}
	}
	}
}
