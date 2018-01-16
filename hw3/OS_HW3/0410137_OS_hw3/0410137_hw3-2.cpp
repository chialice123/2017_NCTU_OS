// Student ID: 0410137
// Name      : LIU, CHIA-LIN
// Date      : 2017.11.03

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
int FILTER_SIZE, FILTER_EDGE;
int *filter_Gx, *filter_Gy;

int size = 0, cnt_grey = 0, cnt_conv=0;

sem_t lock0[2];
sem_t lock1[2];
sem_t lock2[2];
sem_t lock3[2];

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};

const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

unsigned char *pic_in, *pic_grey, *pic_sobel, *pic_final;

unsigned char RGB2grey(int w, int h)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GxFilter(int w, int h)
{
	int tmp = 0;
	int a, b;

	for (int j = 0; j<FILTER_EDGE; ++j)
	for (int i = 0; i<FILTER_EDGE; ++i)
	{
		a = w + i - (FILTER_EDGE / 2);
		b = h + j - (FILTER_EDGE / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_Gx[j*FILTER_EDGE + i] * pic_grey[b*imgWidth + a];
	};
	
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GyFilter(int w, int h)
{
	int tmp = 0;
	int a, b;
	// int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<FILTER_EDGE; ++j)
	for (int i = 0; i<FILTER_EDGE; ++i)
	{
		a = w + i - (FILTER_EDGE / 2);
		b = h + j - (FILTER_EDGE / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_Gy[j*FILTER_EDGE + i] * pic_grey[b*imgWidth + a];
	};
	
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
					sem_post(&lock0[1]);
				}
			}
			sem_post(&lock0[0]);
			sem_post(&lock0[0]);
			
			break;
		}
		
		case 1:{
			for (int j=size; j<(size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size==FILTER_EDGE){
					sem_post(&lock1[1]);
				}
			}
			sem_post(&lock1[0]);
			sem_post(&lock1[0]);
			break;
		}
		
		case 2:{
			for (int j=(size+size); j<(size+size+size); ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size-size==FILTER_EDGE){
					sem_post(&lock2[1]);
				}
			}
			sem_post(&lock2[0]);
			sem_post(&lock2[0]);
			break;
		}
		
		case 3:{
			for (int j=(size+size+size); j<imgHeight; ++j){
				int addr = j*imgWidth;
				for (int i=0; i<imgWidth; ++i){
					pic_grey[addr + i] = RGB2grey(i, j);
				}
				if (j-size-size-size==FILTER_EDGE){
					sem_post(&lock3[1]);
				}
			}
			sem_post(&lock3[0]);
			sem_post(&lock3[0]);
			break;
		}
	}
}

void *thread_RGB2grey (void* param)
{
	_RGB2grey(cnt_grey++);
	pthread_exit(0);
}


void _Gxy(int n){
	double temp;
	switch(n) //divide into 4 blocks
	{
		case 0:{
			sem_wait(&lock0[0]); //self done
			for (int j = 0; j<size; ++j) {
				int addr = j*imgWidth;
				for (int i = 0; i<imgWidth; ++i){
					temp = pow(GxFilter(i, j),2) + pow(GyFilter(i, j),2);
					temp = sqrt(temp);
					if (temp<=255 && temp>=0)
						pic_sobel[addr + i]=(unsigned char)temp;
					else if (temp<0)
						pic_sobel[addr + i]=0;
					else
						pic_sobel[addr + i]=255;
				}
			}
			sem_post(&lock0[0]); //self done
			break;
		}
		
		case 1:{
			sem_wait(&lock1[0]); //self done
			sem_wait(&lock0[0]); //pre done
			sem_wait(&lock2[1]); //next ok			
			for (int j = size; j<(size+size); ++j) {
				int addr = j*imgWidth;
				for (int i = 0; i<imgWidth; ++i){
					temp = pow(GxFilter(i, j),2) + pow(GyFilter(i, j),2);
					temp = sqrt(temp);
					if (temp<=255 && temp>=0)
						pic_sobel[addr + i]=(unsigned char)temp;
					else if (temp<0)
						pic_sobel[addr + i]=0;
					else
						pic_sobel[addr + i]=255;
				}
			}
			sem_post(&lock2[1]);
			sem_post(&lock0[0]);
			sem_post(&lock1[0]);

			break;
		}
		
		case 2:{
			sem_wait(&lock2[0]); //self done
			sem_wait(&lock1[0]); //pre done
			sem_wait(&lock3[1]); //next ok
			for (int j = (size+size); j<(size+size+size); ++j) {
				int addr = j*imgWidth;
				for (int i = 0; i<imgWidth; ++i){
					temp = pow(GxFilter(i, j),2) + pow(GyFilter(i, j),2);
					temp = sqrt(temp);
					if (temp<=255 && temp>=0)
						pic_sobel[addr + i]=(unsigned char)temp;
					else if (temp<0)
						pic_sobel[addr + i]=0;
					else
						pic_sobel[addr + i]=255;
				}
			}
			sem_post(&lock3[1]);
			sem_post(&lock1[0]);
			sem_post(&lock2[0]);
			break;
		}
		
		case 3:{
			sem_wait(&lock3[0]); //self done
			sem_wait(&lock2[0]); //pre done
			for (int j = (size+size+size); j<imgHeight; ++j) {
				int addr = j*imgWidth;
				for (int i = 0; i<imgWidth; ++i){
					temp = pow(GxFilter(i, j),2) + pow(GyFilter(i, j),2);
					temp = sqrt(temp);
					if (temp<=255 && temp>=0)
						pic_sobel[addr + i]=(unsigned char)temp;
					else if (temp<0)
						pic_sobel[addr + i]=0;
					else
						pic_sobel[addr + i]=255;
				}
			}
			sem_post(&lock2[0]);
			sem_post(&lock3[0]);
			break;
		}
	}
}

void *thread_Gxy (void* param)
{
	_Gxy(cnt_conv++);
	pthread_exit(0);
}

int main()
{
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);

	filter_Gx = new int[FILTER_SIZE];
	filter_Gy = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; ++i)
		fscanf(mask, "%d", &filter_Gx[i]);
	for (int i = 0; i<FILTER_SIZE; ++i)
		fscanf(mask, "%d", &filter_Gy[i]);
	fclose(mask);
	FILTER_EDGE = (int)sqrt((float)FILTER_SIZE);

	BmpReader* bmpReader = new BmpReader();
	
	for (int k = 0; k<5; k++){
		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_sobel = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
		
		// threads
		pthread_t t_grey[4], t_conv[4];
		pthread_attr_t attr;
		/* get the default attributes */
		pthread_attr_init(&attr);
		
		// divide into 4 blocks
		size = imgHeight/4;
		
		//initialize semaphore locks
		sem_init(&lock0[0],0,2);
		sem_init(&lock1[0],0,2);
		sem_init(&lock2[0],0,2);
		sem_init(&lock3[0],0,2);
		sem_init(&lock0[1],0,1);
		sem_init(&lock1[1],0,1);
		sem_init(&lock2[1],0,1);
		sem_init(&lock3[1],0,1);
		
		//lock all mutex
		sem_wait(&lock0[0]);
		sem_wait(&lock0[0]);
		sem_wait(&lock1[0]);
		sem_wait(&lock1[0]);
		sem_wait(&lock2[0]);
		sem_wait(&lock2[0]);
		sem_wait(&lock3[0]);
		sem_wait(&lock3[0]);
		sem_wait(&lock0[1]);
		sem_wait(&lock1[1]);
		sem_wait(&lock2[1]);
		sem_wait(&lock3[1]);
		
		
		//convert RGB image to grey image
		for (int i=0; i<4; ++i){
			pthread_create(&t_grey[i], &attr, thread_RGB2grey, NULL);
		}
		
		//apply the Gaussian filter to the image
		for (int i=0; i<4; ++i){
			pthread_create(&t_conv[i], &attr, thread_Gxy, NULL);
		}
		
		for (int i=0; i<4; ++i){
			pthread_join(t_grey[i], NULL);
		}
		
		for (int i=0; i<4; ++i){
			pthread_join(t_conv[i], NULL);
		}

		//extend the size form WxHx1 to WxHx3
		for (int j = 0; j<imgHeight; ++j) {
			int addr = j*imgWidth;
			for (int i = 0; i<imgWidth; ++i){
				pic_final[3 * (addr + i) + MYRED] = pic_sobel[addr + i];
				pic_final[3 * (addr + i) + MYGREEN] = pic_sobel[addr + i];
				pic_final[3 * (addr + i) + MYBLUE] = pic_sobel[addr + i];
			}
		}

		// write output BMP file
		bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_sobel);
		free(pic_final);
		cnt_grey=0;
		cnt_conv=0;
	}

	return 0;
}