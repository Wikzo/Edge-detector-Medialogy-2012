#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat ConvertColorImageToBlackWhite(Mat colorImage);
Mat ThresholdBlackWhiteImage(Mat blackWhiteImage);
Mat EdgeDetecting(Mat inputImage);


int main()
{
	Mat colorImage;

	// Use image
    colorImage = imread("chrome.jpg");
    if (colorImage.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;
    }


	// -----------------------------------------------------

	// Convert to black-white
	Mat gray = ConvertColorImageToBlackWhite(colorImage);
	cv::imwrite("chrome_grayscale.jpg", gray);

	// Threshold
	//Mat threshold = ThresholdBlackWhiteImage(gray);

	// Edge detecting
	//Mat edge = EdgeDetecting(threshold);

	// Edge detecting (OpenCV)
	//cv::Canny(gray, edge, 100, 250);

    cv::imshow("Color image", colorImage);
	cv::imshow("Grayscale image", gray);
	//cv::imshow("Binary image", threshold);
	//cv::imshow("Edge", edge);

	//cv::imshow("Edge", edge);

    waitKey(0);
}

Mat ConvertColorImageToBlackWhite(Mat colorImage)
{
	// Mat(rows, columns, type)
	// rows = y = 300
	// cols = x = 600
	Mat grayScaleImage(colorImage.rows, colorImage.cols, CV_8UC1);

	// Check color image size and channels
	//cout << "colorImage rows: " << colorImage.rows << "; colorImage cols: " << colorImage.cols << endl;
	//cout << "colorImage channels: " << colorImage.channels() << endl;

	// Check grayscale image size and channels
	//cout << "\ngrayScaleImage rows: " << grayScaleImage.rows << "; grayScaleImage cols: " << grayScaleImage.cols << endl;
	//cout << "grayScaleImage channels: " << grayScaleImage.channels() << endl;

	// Formula for converting from color to grayscale (3.3, p. 30 in Introduction to Video and Image Processing book)
	// I = weightR * R + weightG * G + weightB * B

	// Common weight values used in TV production to calculate to grayscale
	float RedWeight = 0.299;
	float GreenWeight = 0.587;
	float BlueWeight = 0.114;
	
	//int yTemp = 199; // 299 = max
	//int xTemp = 299; // 599 = max
	//cout << endl << endl;
	//Vec3b val = colorImage.at<Vec3b>(yTemp, xTemp)[2];
	//cout << "val at [" << yTemp << ", " << xTemp << "]: " << (int)val(0) << endl;
	//rectangle(colorImage, Point(xTemp, yTemp), Point(xTemp+10, yTemp+10), CV_RGB(255, 0, 0), 3);


	//Iterate through all the pixels and apply the formula for grayscale
	for (int y = 0; y < colorImage.rows; y++) // rows
	{
		for (int x = 0; x < colorImage.cols; x++) // < ------------------------------ CANNOT BE BIGGER THAN 200!?!?
		{
			// Calculate grayscale value
			int grayValue = colorImage.at<cv::Vec3b>(y, x)[0] * BlueWeight
				+ colorImage.at<cv::Vec3b>(y, x)[1] * GreenWeight
				+ colorImage.at<cv::Vec3b>(y, x)[2] * RedWeight;

			// Print values
			//cout << "\n\n-------------\nx: " << x << " and y: " << y << endl;
			//cout << "grayValue: " << grayValue << endl;
			grayScaleImage.at<uchar>(y, x) = grayValue;

			//Vec3b val = grayScaleImage.at<Vec3b>(y, x);
			//cout << "Actual val: " << (int)val(0) << endl; // <------------- WORKS!!			

		}
	}	
	return grayScaleImage;
}

Mat ThresholdBlackWhiteImage(Mat blackWhiteImage)
{
	Mat image = blackWhiteImage.clone();
	int threshold = 177;

	for (int y = 0; y < image.rows; y++)
	{
		for (int x = 0; x < image.cols; x++)
		{
			float temp = image.at<cv::Vec3b>(y, x)[0];

			if (temp >= threshold)
			{
				image.at<cv::Vec3b>(y, x)[0] = 255;
				image.at<cv::Vec3b>(y, x)[1] = 255;
				image.at<cv::Vec3b>(y, x)[2] = 255;
				
			}
			else
			{
				image.at<cv::Vec3b>(y, x)[0] = 0;
				image.at<cv::Vec3b>(y, x)[1] = 0;
				image.at<cv::Vec3b>(y, x)[2] = 0;
			}
		}
	}

	return image;
}

Mat EdgeDetecting(Mat inputImage)
{
	Mat image = inputImage.clone();

	int xElements = image.cols;
	int yElements = image.rows;

	int pixelThreshold = 5;

	bool* myArrayX = new bool[xElements];
	bool* myArrayY = new bool[yElements];

	for (int i = 0; i < xElements; i++)
		myArrayX[i] = false;

	for (int i = 0; i < yElements; i++)
		myArrayY[i] = false;

	for (int y = 0; y < image.rows; y++)
	{
		for (int x = pixelThreshold; x < 250; x++)
		{
			// We have an edge
			/*if ((image.at<cv::Vec3b>(y, x)[0] != image.at<cv::Vec3b>(y, x-pixelThreshold)[0]) && (image.at<cv::Vec3b>(y, x-pixelThreshold)[0] != image.at<cv::Vec3b>(y, x+pixelThreshold)[0]))
			{
				myArrayX[x] = true;
				myArrayY[y] = true;
			}*/

			// We have an edge
			if ((image.at<cv::Vec3b>(y, x)[0] != image.at<cv::Vec3b>(y, x+1)[0]))
			{
				image.at<cv::Vec3b>(x, x)[0] = 20;
				image.at<cv::Vec3b>(x, x)[1] = 20;
				image.at<cv::Vec3b>(x, x)[2] = 200;
			}
		}
	}

	// Put red color in
	for (int y = 0; y < 500;)
	{
		y++;
		for (int x = 0; x < 200;)
		{
			x++;
			if (myArrayX[x] == true)
			{
				image.at<cv::Vec3b>(x, x)[0] = 20;
				image.at<cv::Vec3b>(x, x)[1] = 20;
				image.at<cv::Vec3b>(x, x)[2] = 200;
			}
		}
	}

	return image;
}