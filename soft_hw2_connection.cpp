// ConsoleApplication1.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<opencv/cv.hpp>
using namespace cv;
#include <iostream>
using namespace std;
#define MY_FIND_OK 1
#define MY_FIND_FAIL -1


int ustc_ConnectedComponentLabeling( Mat grayImg, Mat& labelImg, int bin_th)
{
	if (NULL == grayImg.data || 1 != grayImg.channels())
	{
		cout << "invalid image input" << endl;
		return MY_FIND_FAIL;
	}
	int cols = grayImg.cols;
	int rows = grayImg.rows;
	

	//connection map
	Mat connection_map(rows, cols, CV_8UC1);
	grayImg.copyTo(connection_map);
	namedWindow("copy_image", 0);
	imshow("copy_image", connection_map);
	//mask
	int forward_x[5] = { -1,-1,0,1,0 };
	int forward_y[5] = { 0,-1,-1,-1,0 };
	int backward_x[5] = { 1,1,0,-1,0 };
	int backward_y[5] = { 0,1,1,1,0 };
	
	int m = 1;//incrementing index
	// pre-scan approximate the size of m, proposed by Yuhang
	for (int j = 0; j< rows; j++)
		for (int k = 0; k < cols; k++)
		{
			int cord = j*cols + k;
			int pix_value = connection_map.data[cord];
			if (pix_value <= bin_th)
			{
				int min_neighbour = bin_th + 1;
				int foreground_neighbour = 0;
				for (int i = 0; i < 4; i++)
				{
					int j_neighbour = j + forward_y[i];
					int k_neighbour = k + forward_x[i];
					if (k_neighbour >=0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
					{
						int cord_neighbour = j_neighbour*cols + k_neighbour;
						if (min_neighbour > connection_map.data[cord_neighbour])
						{
							foreground_neighbour = 1;
							break;
						}
					}
				
				}
				if (!foreground_neighbour)
					m++;

			}
		}
	int* T = new int[m+1];//connection table
	for (int i = 0; i < m+1; i++)
	{
		T[i] = i;
	}
	m = 1;
	for(int j = 0; j< rows; j++)
		for (int k = 0; k < cols; k++)
		{
			int cord = j*cols + k;
			int pix_value = connection_map.data[cord];
			if (pix_value <= bin_th)
			{
				int min_neighbour = bin_th + 1;
				int j_neighbour_min = j;
				int k_neighbour_min = k;
				int cord_neighbour_min = cord;
				for (int i = 0; i < 4; i++)
				{
					int j_neighbour = j + forward_y[i];
					int k_neighbour = k + forward_x[i];
					if (k_neighbour >=0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
					{
						int cord_neighbour = j_neighbour*cols + k_neighbour;
						if (connection_map.data[cord_neighbour]<= bin_th && min_neighbour > T[connection_map.data[cord_neighbour]])
						{
							min_neighbour = T[connection_map.data[cord_neighbour]];
							j_neighbour_min = j_neighbour;
							k_neighbour_min = k_neighbour;
							cord_neighbour_min = cord;
						}
					}
				
				}
				if (min_neighbour <= bin_th)
				{
					connection_map.data[cord] = min_neighbour;
					for (int i = 0; i < 4; i++)
					{
						int j_neighbour = j + forward_y[i];
						int k_neighbour = k + forward_x[i];
						if (k_neighbour >= 0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
						{
							int cord_neighbour = j_neighbour*cols + k_neighbour;
							if(connection_map.data[cord_neighbour]<= bin_th)
							T[connection_map.data[cord_neighbour]] = min_neighbour;
						}

					}
				}
				else {
					connection_map.data[cord] = m;
					T[m] = m;
					m++;
				}

			}
			
		}
	int final_flag = 0;
	while (!final_flag)
	{
		final_flag = 1;
		//backward
		for (int j = rows-1; j>=0; j--)
			for (int k = cols-1; k>=0; k--)
			{
				int cord = j*cols + k;
				int pix_value = connection_map.data[cord];
				if (pix_value <= bin_th)
				{
					int min_neighbour = bin_th +1;
					for (int i = 0; i < 5; i++)
					{
						int j_neighbour = j + backward_y[i];
						int k_neighbour = k + backward_x[i];
						if (k_neighbour >= 0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
						{
							int cord_neighbour = j_neighbour*cols + k_neighbour;
							if (connection_map.data[cord_neighbour] <= bin_th && min_neighbour > T[connection_map.data[cord_neighbour]])
							{
								min_neighbour = T[connection_map.data[cord_neighbour]];
							}
						}

					}
					for (int i = 0; i < 5; i++)
					{
						int j_neighbour = j + backward_y[i];
						int k_neighbour = k + backward_x[i];
						if (k_neighbour >= 0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
						{
							int cord_neighbour = j_neighbour*cols + k_neighbour;
							if (connection_map.data[cord_neighbour] <= bin_th && T[connection_map.data[cord_neighbour]] != min_neighbour)
							{
								T[connection_map.data[cord_neighbour]] = min_neighbour;
								final_flag = 0;

							}
						}
					}

				}
			}
		//forward
		for (int j = 0; j <rows; j++)
			for (int k = 0; k <cols; k++)
			{
				int cord = j*cols + k;
				int pix_value = connection_map.data[cord];
				if (pix_value <= bin_th)
				{
					int min_neighbour = bin_th +1;
					for (int i = 0; i < 5; i++)
					{
						int j_neighbour = j + forward_y[i];
						int k_neighbour = k + forward_x[i];
						if (k_neighbour >= 0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
						{
							int cord_neighbour = j_neighbour*cols + k_neighbour;
							if (connection_map.data[cord_neighbour] <= bin_th && min_neighbour > T[connection_map.data[cord_neighbour]])
							{
								min_neighbour = T[connection_map.data[cord_neighbour]];
							}
						}

					}
					//connection_map.data[cord] = min_neighbour;
					for (int i = 0; i < 5; i++)
					{
						int j_neighbour = j + forward_y[i];
						int k_neighbour = k + forward_x[i];
						if (k_neighbour >= 0 && k_neighbour < cols && j_neighbour>=0 && j_neighbour < rows)
						{
							int cord_neighbour = j_neighbour*cols + k_neighbour;
							if (connection_map.data[cord_neighbour] <= bin_th && T[connection_map.data[cord_neighbour]] != min_neighbour)
							{
								T[connection_map.data[cord_neighbour]] = min_neighbour;
								final_flag = 0;

							}
						}
					}

				}
			}
	}
	for (int j = 0; j <rows; j++)
		for (int k = 0; k < cols; k++)
		{
			int cord = j*cols + k;
			if (connection_map.data[cord] <= bin_th)
				connection_map.data[cord] = T[connection_map.data[cord]];
		}
	connection_map.copyTo(labelImg);
	//visualization in the below sections
	int index = 1;
	for (int i = 0; i < m+1; i++)
	{
		T[i] = 0;
	}
	for(int j = 0; j< rows; j++)
		for (int k = 0; k < cols; k++)
		{
			int ord = j* cols + k;
			int pixel_value = connection_map.data[ord];
			if (pixel_value <= bin_th)
			{
				if (0 == T[pixel_value])
				{
					T[pixel_value] = index;
					connection_map.data[ord] = index ;
					index++;
				}
				else {
					connection_map.data[ord] = T[pixel_value];
				}
				
			}
		}
	int index_show = 255 / index;
	for (int j = 0; j< rows; j++)
		for (int k = 0; k < cols; k++)
		{
			int ord = j* cols + k;
			int pixel_value = connection_map.data[ord];
			if (pixel_value <= bin_th)
			{
				connection_map.data[ord] = index_show* connection_map.data[ord];
			}
		}
	cout << "total number of connected region= " << (index - 1) << endl;
	namedWindow("connection_map", 0);
	imshow("connection_map", connection_map);
	waitKey();
	return MY_FIND_OK;

}

int main()
{
	Mat image1 = imread("E:\\courses\\software\\homework2\\multi_circle.png");
	Mat image2;
	cvtColor(image1, image2, CV_BGR2GRAY);
	Mat labelImg(image2.cols, image2.rows, CV_8UC1);
	ustc_ConnectedComponentLabeling(image2, labelImg,250);
	namedWindow("connection_map", 0);
	imshow("connection_map", labelImg);
	waitKey();


    return 0;
}

