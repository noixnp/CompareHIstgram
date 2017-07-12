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

using namespace std;


struct FILE_PAIR{
	string a;
	string asubname;
	string b;
	string bsubname;
};

FILE_PAIR files[10000];

int main(void){

	ifstream ifs("input.txt");
	ofstream ofp("output.txt");

	IplImage *src1 = 0, *src2 = 0;
	IplImage *dst1[4] = { 0, 0, 0, 0 }, *dst2[4] = { 0, 0, 0, 0 };
	CvHistogram *hist1, *hist2;
	int sch1 = 0, sch2 = 0;
	int hist_size = 256;
	float range_0[] = { 0, 256 };
	float *ranges[] = { range_0 };

	int N = 0;
	while (ifs >> files[N].asubname >> files[N].bsubname){
		files[N].a = "image/" + files[N].asubname;
		files[N].b = "image/" + files[N].bsubname;
		N++;
	}

	for (int i = 0; i < N; i++){
		cout << files[i].a << "," << files[i].b << endl;
		const char* ca = files[i].a.c_str();
		const char* cb = files[i].b.c_str();

		src1 = cvLoadImage(ca, CV_LOAD_IMAGE_COLOR);
		src2 = cvLoadImage(cb, CV_LOAD_IMAGE_COLOR);
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


		printf("ヒストグラムの比較\n");
		printf("Correlation(CV_COMP_CORREL) : %f\n", correlation);
		printf("Chi-square(CV_COMP_CHSQR) : %f\n", chi);
		printf("Intersection(CV_COMP_INTERSECT) : %f\n", intersection);
		printf("Bhattacharyya distance(CV_COMP_BHATTACHARYYA) : %f\n", bhattacharyya);

		ofp << files[i].a << "," << files[i].b << "\n"
			<< "相関" << correlation << "\n"
			<< "カイ2乗" << chi << "\n"
			<< "交差" << intersection << "\n"
			<< "Bhattacharyya距離" << bhattacharyya << endl;

		return 0;

	}
}