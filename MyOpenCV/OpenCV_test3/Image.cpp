#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

enum SobelDirection
{
	Diagonal_Right,
	Diagonal_Left,
	Vertical,
	Horizontal
};

const int THRESHOLD_GRAYSCALE = 133; // optimal value was found using ImageJ
const int THRESHOLD_SOBEL = 30; // found by experimenting

Mat ConvertColorImageToBlackWhite(Mat colorImage);
Mat MeanFilter(Mat input);
Mat ThresholdBlackWhiteImage(Mat blackWhiteImage, int threshold);
Mat SobelEdgeDetecting(Mat input, enum SobelDirection direction, bool useMeanFilterBeforeDoingEdgeDetecting, int threshold);
Mat AddTwoMatsTogether(Mat matA, Mat matB);
void SortValues(uchar* a, int size);



int main()
{
	cout << "Edge detection using the Sobel kernel (and OpenCV to load images)" << endl;
	cout << "By Gustav Dahl - Medialogy 3rd semester 2012 - Aalborg University\n\n";
	// Load the image
    Mat colorImage = imread("building.jpg");
    if (colorImage.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;
    }

	// Here is something cool

	cout << "Processing image. Please wait..." << endl;

	// Convert to black-white
	Mat gray = ConvertColorImageToBlackWhite(colorImage);
	cv::imwrite("grayscale.jpg", gray);

	// Mean (black and white only)
	Mat mean = MeanFilter(gray);
	imwrite("mean.jpg", mean);

	// Threshold
	Mat threshold = ThresholdBlackWhiteImage(gray, THRESHOLD_GRAYSCALE);
	imwrite("threshold.jpg", threshold);

	// Edge detecting
	Mat edge_diagonal_right = SobelEdgeDetecting(gray, Diagonal_Right, true, THRESHOLD_SOBEL);
	Mat edge_diagonal_left = SobelEdgeDetecting(gray, Diagonal_Left, true, THRESHOLD_SOBEL);
	Mat edge_vertical = SobelEdgeDetecting(gray, Vertical, true, THRESHOLD_SOBEL);
	Mat edge_horizontal = SobelEdgeDetecting(gray, Horizontal, true, THRESHOLD_SOBEL);

	Mat verti_plus_horiz = AddTwoMatsTogether(edge_vertical, edge_horizontal);
	Mat diagonal_right_plus_left = AddTwoMatsTogether(edge_diagonal_right, edge_diagonal_left);
	Mat diagonal_plus_vertical_horizontal = AddTwoMatsTogether(verti_plus_horiz, diagonal_right_plus_left);

	imwrite("diagonal_right.jpg", edge_diagonal_right);
	imwrite("edge_diagonal_left.jpg", edge_diagonal_right);
	imwrite("edge_vertical.jpg", edge_vertical);
	imwrite("edge_horizontal.jpg", edge_horizontal);
	imwrite("verti_plus_horiz.jpg", verti_plus_horiz);
	imwrite("diagonal_right_plus_left.jpg", diagonal_right_plus_left);
	imwrite("diagonal_plus_vertical_horizontal.jpg", diagonal_plus_vertical_horizontal);

    cv::imshow("Original image", colorImage);
	cv::imshow("Grayscale image", gray);
	cv::imshow("Mean filter", mean);
	cv::imshow("Binary image", threshold);
	cv::imshow("Edge diagonal right", edge_diagonal_right);
	cv::imshow("Edge diagonal left", edge_diagonal_left);
	cv::imshow("Edge vertical", edge_vertical);
	cv::imshow("Edge horizontal", edge_horizontal);
	cv::imshow("Diagonal plus vertical and horizontal", diagonal_plus_vertical_horizontal);

    waitKey(0);
}

Mat ConvertColorImageToBlackWhite(Mat colorImage)
{
	// Mat(rows, columns, type)
	// rows = y = 300
	// cols = x = 600

	Mat grayScaleImage(colorImage.rows, colorImage.cols, CV_8UC1); // new image with only 1 channel

	// Formula for converting from color to grayscale (3.3, p. 30 in Introduction to Video and Image Processing book)
	// I = weightR * R + weightG * G + weightB * B

	// Common weight values used in TV production to calculate to grayscale
	float RedWeight = 0.299;
	float GreenWeight = 0.587;
	float BlueWeight = 0.114;
	

	// Iterate through all the pixels and apply the formula for grayscale
	for (int y = 0; y < colorImage.rows; y++) // rows
	{
		for (int x = 0; x < colorImage.cols; x++)
		{
			// Calculate grayscale value
			int grayValue = colorImage.at<cv::Vec3b>(y, x)[0] * BlueWeight
				+ colorImage.at<cv::Vec3b>(y, x)[1] * GreenWeight
				+ colorImage.at<cv::Vec3b>(y, x)[2] * RedWeight;

			// Apply the grayscale value (0-255)
			grayScaleImage.at<uchar>(y, x) = grayValue;		

		}
	}	
	return grayScaleImage;
}

Mat MeanFilter(Mat input)
{
	// 3x3 kernel size

	Mat mean = input.clone();

	// Loop through all pixels
	for (int y = 0; y < input.rows-2; y++)
	{
		for (int x = 0; x < input.cols-2; x++)
		{
			if (x - 2 < 0 || y - 2 < 0) // don't go out of bounds
				continue;

			mean.at<uchar>(y, x) = (
				input.at<uchar>(y-2, x-2) + input.at<uchar>(y-2, x-1)
				+ input.at<uchar>(y-2, x) + input.at<uchar>(y-2, x+1)
				+ input.at<uchar>(y-2, x+2) + input.at<uchar>(y-1, x-2)
				+ input.at<uchar>(y-1, x-1) + input.at<uchar>(y-1, x)
				+ input.at<uchar>(y-1, x+1) + input.at<uchar>(y-1, x+2)
				+ input.at<uchar>(y, x-2) + input.at<uchar>(y, x-1)
				+ input.at<uchar>(y, x) + input.at<uchar>(y, x+1)
				+ input.at<uchar>(y, x+2) + input.at<uchar>(y+1, x-2)
				+ input.at<uchar>(y+1, x-1) + input.at<uchar>(y+1, x)
				+ input.at<uchar>(y+1, x+1) + input.at<uchar>(y+1, x+2)
				+ input.at<uchar>(y+2, x-2)	+ input.at<uchar>(y+2, x-1)
				+ input.at<uchar>(y+2, x) + input.at<uchar>(y+2, x+1)
				+ input.at<uchar>(y+2, x+2)
				) / 25;
		}
	}

	return mean;
}

Mat ThresholdBlackWhiteImage(Mat blackWhiteImage, int threshold)
{
	Mat image = blackWhiteImage.clone();

	// Loop through all pixels and set them to either 255 (white) or 0 (black) using the threhold value
	for (int y = 0; y < image.rows; y++)
	{
		for (int x = 0; x < image.cols; x++)
		{
			if (image.at<uchar>(y, x) >= threshold)
				image.at<uchar>(y, x) = 255;
			else
				image.at<uchar>(y, x) = 0;
		}
	}

	return image;
}

Mat SobelEdgeDetecting(Mat input, enum SobelDirection direction, bool useMeanFilterBeforeDoingEdgeDetecting, int threshold)
{
	Mat edge = input.clone();

	if (useMeanFilterBeforeDoingEdgeDetecting)
		edge = MeanFilter(edge);

	// Apply diagonal edge detecting RIGHT
	if (direction == Diagonal_Right)
	{
		for (int y = 0; y < input.rows-1; y++)
		{
			for (int x = 0; x < input.cols-1; x++)
			{
				if (x-1 < 0 || y-1 < 0) // don't go out of bounds
					continue;


				// temp value is used to not get overflow (value cannot be less than 0 or greater than 255)
				int temp = (
					(input.at<uchar>(y-1, x-1)) * -2
					+ (input.at<uchar>(y, x-1)) * -1
					+ (input.at<uchar>(y+1, x-1)) * 0
					+ (input.at<uchar>(y-1, x)) * -1
					+ (input.at<uchar>(y, x)) * 0
					+ (input.at<uchar>(y+1, x+0)) * 1
					+ (input.at<uchar>(y-1, x+1)) * 0
					+ (input.at<uchar>(y, x+1)) * 1
					+ (input.at<uchar>(y+1, x+1)) * 2
					);


				// Map values from 0 to 255
				if (temp < 0)
					temp = 0;
				else if (temp > 255)
					temp = 255;
				

				edge.at<uchar>(y, x) = temp;
			}
		}
	}
	else if (direction == Diagonal_Left)
	{	// Apply diagonal edge detecting LEFT
		for (int y = 0; y < input.rows-1; y++)
		{
			for (int x = 0; x < input.cols-1; x++)
			{
				if (x-1 < 0 || y-1 < 0) // don't go out of bounds
					continue;

				// temp value is used to not get overflow (value cannot be less than 0 or greater than 255)
				int temp = (
					(input.at<uchar>(y-1, x-1)) * -2
					+ (input.at<uchar>(y, x-1)) * -1
					+ (input.at<uchar>(y+1, x-1)) * 0
					+ (input.at<uchar>(y-1, x)) * -1
					+ (input.at<uchar>(y, x)) * 0
					+ (input.at<uchar>(y+1, x+0)) * 1
					+ (input.at<uchar>(y-1, x+1)) * 0
					+ (input.at<uchar>(y, x+1)) * 1
					+ (input.at<uchar>(y+1, x+1)) * 2
					);

				// Map values from 0 to 255
				if (temp < 0)
					temp = 0;
				else if (temp > 255)
					temp = 255;
				

				edge.at<uchar>(y, x) = temp;
			}
		}
	}
	else if (direction == Vertical)
	{
		// Apply diagonal edge detecting ´VERTICAL
		for (int y = 0; y < input.rows-1; y++)
		{
			for (int x = 0; x < input.cols-1; x++)
			{
				if (x-1 < 0 || y-1 < 0) // don't go out of bounds
					continue;

				// temp value is used to not get overflow (value cannot be less than 0 or greater than 255)
				int temp = (
					(input.at<uchar>(y-1, x-1)) * -1
					+ (input.at<uchar>(y, x-1)) * -2
					+ (input.at<uchar>(y+1, x-1)) * -1
					+ (input.at<uchar>(y-1, x)) * -0
					+ (input.at<uchar>(y, x)) * 0
					+ (input.at<uchar>(y+1, x+0)) * 0
					+ (input.at<uchar>(y-1, x+1)) * 1
					+ (input.at<uchar>(y, x+1)) * 2
					+ (input.at<uchar>(y+1, x+1)) * 1
					);

				// Map values from 0 to 255
				if (temp < 0)
					temp = 0;
				else if (temp > 255)
					temp = 255;
				

				edge.at<uchar>(y, x) = temp;
			}
		}
	}
	else if (direction == Horizontal)
	{
		// Apply diagonal edge detecting HORIZONTAL
		for (int y = 0; y < input.rows-1; y++)
		{
			for (int x = 0; x < input.cols-1; x++)
			{
				if (x-1 < 0 || y-1 < 0) // don't go out of bounds
					continue;

				// temp value is used to not get overflow (value cannot be less than 0 or greater than 255)
				int temp = (
					(input.at<uchar>(y-1, x-1)) * -1
					+ (input.at<uchar>(y, x-1)) * 0
					+ (input.at<uchar>(y+1, x-1)) * 1
					+ (input.at<uchar>(y-1, x)) * -2
					+ (input.at<uchar>(y, x)) * 0
					+ (input.at<uchar>(y+1, x+0)) * 2
					+ (input.at<uchar>(y-1, x+1)) * -1
					+ (input.at<uchar>(y, x+1)) * 0
					+ (input.at<uchar>(y+1, x+1)) * 1
					);

				// Map values from 0 to 255
				if (temp < 0)
					temp = 0;
				else if (temp > 255)
					temp = 255;
				

				edge.at<uchar>(y, x) = temp;
			}
		}
	}
	else
	{
		// Error text
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 50), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 4, 8, false);
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 150), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 4, 8, false);
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 250), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 4, 8, false);
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 300), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 4, 8, false);
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 450), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 4, 8, false);
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 600), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 4, 8, false);
	}

	// Threshold
	edge = ThresholdBlackWhiteImage(edge, 30);
	return edge;
}

Mat AddTwoMatsTogether(Mat matA, Mat matB) // should be same size!
{
	Mat output = matA.clone();

	for (int y = 0; y < matA.rows; y++)
	{
		for (int x = 0; x < matA.cols; x++)
		{
			output.at<uchar>(y, x) = matA.at<uchar>(y, x) + matB.at<uchar>(y, x);
		}

	}

	output = ThresholdBlackWhiteImage(output, THRESHOLD_GRAYSCALE);
	return output;
}

// Not working yet
/*void MedianFilter(Mat input, int kernelSize)
{
	if (kernelSize % 2 == 0) // don't use even numbers
		kernelSize++;

	int radius = kernelSize / 2; // e.g. kernel is 7X7 --> radius is 3

	uchar neighborPixels[9];

	for (int y = radius; radius < (input.rows - radius); y++)
	{
		int counter = 0;
		for (int x = radius; x < (input.cols - radius); x++)
		{
			// Done
			if (counter >= 9)
			{
				//start sort
				// put pixels back in
			}
			neighborPixels[x*y] = input.at<uchar>(y, x);
			counter++;
		}
	}
}*/

// Not working yet
/*uchar* SortValues(uchar* a, int size)
{
	cout << "Before the bubblesort:" << endl;
	for (int i = 0; i < size; i++)
		cout << a[i] << endl;

	// Bubble list
	bool swapped = true;

	while (swapped)
	{
		swapped = false;
		for (int i = 1; i < size; i++)
		{
			if (a[i-1] > a[i])
			{
				// Sort numbers
				int temp = a[i-1];
				a[i-1] = a[i];
                a[i] = temp;

                swapped = true;
			}
		}
	}

	cout << "After the bubblesort:" << endl;
	for (int i = 0; i < size; i++)
		cout << a[i] << endl;

	uchar* aPointer = a;
	//return aPointer;
}*/