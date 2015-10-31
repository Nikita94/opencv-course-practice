#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <algorithm>

using namespace cv;
using namespace std;

int main() {
	int height = 620;
	int width = 440;
	Mat img = imread("lena.jpg", 1);
	// RGB to GS
	Mat gs(img.size(), CV_8UC1);
	cvtColor(img, gs, CV_RGB2GRAY);
	// end RGB to GS
	if (!img.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	//namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	//imshow("Display window", img);
	//imshow("GS", gs);

	

	double lowThreshold = 70, uppThreshold = 260;
	Mat edgesImg;
	Canny(gs, edgesImg, lowThreshold, uppThreshold);

	//imshow("Canny", edgesImg);

	// Binary invert
	threshold(edgesImg, edgesImg, 70, 255, CV_THRESH_BINARY_INV);

	// Perform the distance transform algorithm
	Mat dist;
	distanceTransform(edgesImg, dist, CV_DIST_L2, 3);
	// Normalize the distance image for range = {0.0, 1.0}
	// so we can visualize and threshold it
	normalize(dist, dist, 0, 1., NORM_MINMAX);
	//imshow("Distance Transform Image", dist);

	/*Mat sum;

	integral(dist, sum, CV_8UC1);
	imshow("Integral of image", sum); */

	vector<Mat> bgrMat(3);

	split(img, bgrMat); 

	for (int k = 0; k < bgrMat.size(); k++) {
		Mat integral;
		cv::integral(bgrMat[k], integral, CV_32F);
		for (int j = 0; j < integral.rows; j++)
			for (int i = 0; i < bgrMat[i].cols; i++) {
			
				int ksize = static_cast<int>(dist.at<float>(j, i) / 5);
				if (ksize < 1) ksize = 1;
				if (ksize > 10) ksize = 10;
				int minx = max(0, j - ksize / 2);
				int maxx = min(img.cols - 1, j + ksize / 2);
				int miny = max(0, i - ksize / 2);
				int maxy = min(img.rows - 1, i + ksize / 2);

				uchar rgb = integral.at<uchar>(maxy + 1, maxx + 1)
					- integral.at<uchar>(miny + 0, maxx + 1)
					- integral.at<uchar>(maxy + 1, minx + 0)
					+ integral.at<uchar>(miny + 0, minx + 0);
				if ((maxy - miny + 1) == 0)
					rgb /= (maxx - minx + 1);
				else if((maxx - minx + 1 == 0))
					rgb /= (maxy - miny + 1);
				else 
					rgb /= (maxy - miny + 1) * (maxx - minx + 1);
				bgrMat[k].at<uchar>(j, i) = rgb;
			}
	}
	Mat out;
	merge(bgrMat, out);

	imshow("Final", out);
	waitKey(0);
	return 0;
}

