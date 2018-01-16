// Student ID: 0410137
// Name      : LIU, CHIA-LIN
// Date      : 2017.12.08

#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight;
int FILTER_SIZE;
int FILTER_SCALE;
int FILTER_EDGE;
int *filter_G;

int size = 0, cnt_grey = 0, cnt_conv=0;

pthread_mutex_t lock0[3] = {PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t lock1[3] = {PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t lock2[3] = {PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t lock3[3] = {PTHREAD_MUTEX_INITIALIZER};

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] = {
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;

unsigned char RGB2grey(int w, int h)
{
	int addr = 3*(h*imgWidth + w);
	int tmp = (
		pic_in[addr + MYRED] +
		pic_in[addr + MYGREEN] +
		pic_in[addr + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GaussianFilter(int w, int h)
{
	int tmp = 0;
	int a, b;
	// int ws = (int)sqrt((float)FILTER_SIZE);
	
	for (int j = 0; j<FILTER_EDGE; j++)
	for (int i = 0; i<FILTER_EDGE; i++)
	{
		a = w + i - (FILTER_EDGE / 2);
		b = h + j - (FILTER_EDGE / 2);
		
		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_G[j*FILTER_EDGE + i] * pic_grey[b*imgWidth + a];
	};
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

void _RGB2grey(int n){
	switch(n) //divide into 4 blocks
	{
		case 0:{
			for (int j=0; j<size; ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j==FILTER_EDGE){
					pthread_mutex_unlock(&lock0[2]);
				}
			}
			pthread_mutex_unlock(&lock0[0]);
			pthread_mutex_unlock(&lock0[1]);
			
			break;
		}
		
		case 1:{
			for (int j=size; j<(size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size==FILTER_EDGE){
					pthread_mutex_unlock(&lock1[2]);
				}
			}
			pthread_mutex_unlock(&lock1[0]);
			pthread_mutex_unlock(&lock1[1]);
			break;
		}
		
		case 2:{
			for (int j=(size+size); j<(size+size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size-size==FILTER_EDGE){
					pthread_mutex_unlock(&lock2[2]);
				}
			}
			pthread_mutex_unlock(&lock2[0]);
			pthread_mutex_unlock(&lock2[1]);
			break;
		}
		
		case 3:{
			for (int j=(size+size+size); j<imgHeight; ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size-size-size==FILTER_EDGE){
					pthread_mutex_unlock(&lock3[2]);
				}
			}
			pthread_mutex_unlock(&lock3[0]);
			pthread_mutex_unlock(&lock3[1]);
			break;
		}
	}
}

void *thread_RGB2grey (void* param)
{
	_RGB2grey(cnt_grey++);
	pthread_exit(0);
}

void _Gaussian(int n){
	switch(n) //divide into 4 blocks
	{
		case 0:{
			pthread_mutex_lock(&lock0[0]); //self done
			for (int j=0; j<size; ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_blur[addr + i] = GaussianFilter(i, j);
				}
			}
			pthread_mutex_unlock(&lock0[0]); //self done
			break;
		}
		
		case 1:{
			pthread_mutex_lock(&lock1[0]); //self done
			pthread_mutex_lock(&lock0[1]); //pre done
			pthread_mutex_lock(&lock2[2]); //next ok			
			for (int j=size; j<(size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_blur[addr + i] = GaussianFilter(i, j);
				}
			}
			pthread_mutex_unlock(&lock2[2]);
			pthread_mutex_unlock(&lock0[1]);
			pthread_mutex_unlock(&lock1[0]);

			break;
		}
		
		case 2:{
			pthread_mutex_lock(&lock2[0]); //self done
			pthread_mutex_lock(&lock1[1]); //pre done
			pthread_mutex_lock(&lock3[2]); //next ok			
			for (int j=(size+size); j<(size+size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_blur[addr + i] = GaussianFilter(i, j);
				}
			}
			pthread_mutex_unlock(&lock3[2]);
			pthread_mutex_unlock(&lock1[1]);
			pthread_mutex_unlock(&lock2[0]);
			break;
		}
		
		case 3:{
			pthread_mutex_lock(&lock3[0]); //self done
			pthread_mutex_lock(&lock2[1]); //pre done
			for (int j=(size+size+size); j<imgHeight; ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_blur[addr + i] = GaussianFilter(i, j);
				}
			}
			pthread_mutex_unlock(&lock2[1]);
			pthread_mutex_unlock(&lock3[0]);
			break;
		}
	}
}

void *thread_Gaussian (void* param)
{
	_Gaussian(cnt_conv++);
	pthread_exit(0);
}


int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);
	FILTER_EDGE = (int)sqrt((float)FILTER_SIZE);

	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		// int alloc = imgWidth*imgHeight;
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3*imgWidth*imgHeight*sizeof(unsigned char));
		
		// threads
		pthread_t t_grey[4], t_conv[4];
		pthread_attr_t attr;
		/* get the default attributes */
		pthread_attr_init(&attr);
		
		// divide into 4 blocks
		size = imgHeight/4;
		
		//lock all mutex
		for (int i=0; i<3; ++i){
			pthread_mutex_lock(&lock0[i]);
			pthread_mutex_lock(&lock1[i]);
			pthread_mutex_lock(&lock2[i]);
			pthread_mutex_lock(&lock3[i]);
		}
		
		//convert RGB image to grey image
		for (int i=0; i<4; ++i){
			pthread_create(&t_grey[i], &attr, thread_RGB2grey, NULL);
		}

		//apply the Gaussian filter to the image
		for (int i=0; i<4; ++i){
			pthread_create(&t_conv[i], &attr, thread_Gaussian, NULL);
		}
		
		for (int i=0; i<4; ++i){
			pthread_join(t_grey[i], NULL);
		}
		
		for (int i=0; i<4; ++i){
			pthread_join(t_conv[i], NULL);
		}

		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; j++) {
			int addr = j*imgWidth;
			for (int i = 0; i<imgWidth; i++){
				pic_final[3 * (addr + i) + MYRED] = pic_blur[addr + i];
				pic_final[3 * (addr + i) + MYGREEN] = pic_blur[addr + i];
				pic_final[3 * (addr + i) + MYBLUE] = pic_blur[addr + i];
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_blur);
		free(pic_final);
		cnt_grey = 0;
		cnt_conv = 0;
	}

	return 0;
}