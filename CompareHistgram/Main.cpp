#pragma warning(disable:4996)

#include <cstdio>
#include <fstream>
#include <string>
#include <iomanip>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>   
#include <functional>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>



using namespace std;


int main(void){

	

	IplImage *src1 = 0, *src2 = 0;
	IplImage *dst1[4] = { 0, 0, 0, 0 }, *dst2[4] = { 0, 0, 0, 0 };
	CvHistogram *hist1, *hist2;
	int sch1 = 0, sch2 = 0;
	int hist_size = 256;
	float range_0[] = { 0, 256 };
	float *ranges[] = { range_0 };

	
	string read1 = "good_lbp_24_3_uniform.png";
	string read2 = "good2_lbp_24_3_uniform.png";
	const char* image1 = read1.c_str();
	const char* image2 = read2.c_str();

	src1 = cvLoadImage(image1, CV_LOAD_IMAGE_COLOR);
	src2 = cvLoadImage(image2, CV_LOAD_IMAGE_COLOR);


		if (src1 == 0 || src2 == 0){
			return -1;
		}
			sch1 = src1->nChannels;
			sch2 = src2->nChannels;
		if (sch1 != sch2){
			return -1;
		}

		for (int i = 0; i < sch1; i++) {
			dst1[i] = cvCreateImage(cvSize(src1->width, src1->height), src1->depth, 1);
			dst2[i] = cvCreateImage(cvSize(src2->width, src2->height), src2->depth, 1);
		}

		//ヒストグラム構造体を確保
		hist1 = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
		hist2 = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

		//入力画像がマルチチャンネルの場合，画像をチャンネル毎に分割
		if (sch1 == 1) {
			cvCopy(src1, dst1[0], NULL);
			cvCopy(src2, dst2[0], NULL);
		}
		else{
			cvSplit(src1, dst1[0], dst1[1], dst1[2], dst1[3]);
			cvSplit(src2, dst2[0], dst2[1], dst2[2], dst2[3]);
		}
		//ヒストグラムの計算
		for (int i = 0; i < sch1; i++){
			cvCalcHist(&dst1[i], hist1, 0, NULL);
			cvCalcHist(&dst2[i], hist2, 0, NULL);
			cvNormalizeHist(hist1, 1.0);
			cvNormalizeHist(hist2, 1.0);
		}

		
		double correlation = cvCompareHist(hist1, hist2, CV_COMP_CORREL);
		double chi = cvCompareHist(hist1, hist2, CV_COMP_CHISQR);
		double intersection = cvCompareHist(hist1, hist2, CV_COMP_INTERSECT);
		double bhattacharyya = cvCompareHist(hist1, hist2, CV_COMP_BHATTACHARYYA);


		printf("ヒストグラムの比較");
		printf("%s ",image1);
		printf("%s\n", image2);
		printf("相関 : %f\n", correlation);
		printf("カイ2乗 : %f\n", chi);
		printf("交差: %f\n", intersection);
		printf("Bhattacharyya距離: %f\n", bhattacharyya);

		return 0;


	
}