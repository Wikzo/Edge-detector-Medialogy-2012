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
const int THRESHOLD_SOBEL = 100; // found by experimenting

Mat ConvertColorImageToBlackWhite(Mat colorImage);
Mat MeanFilter(Mat input);
Mat ThresholdBlackWhiteImage(Mat blackWhiteImage, int threshold);
Mat SobelEdgeDetecting(Mat input, enum SobelDirection direction, bool useMeanFilterBeforeDoingEdgeDetecting, int threshold);
Mat AddTwoMatsTogether(Mat matA, Mat matB);
Mat Erosion(Mat input, int radius);



int main()
{
	// Program description
	cout << "Edge detection using the Sobel kernel (and OpenCV to load images)" << endl;
	cout << "By Gustav Dahl - Medialogy 3rd semester 2012 - Aalborg University\n\n";
	
	// Load the original color image
    Mat colorImage = imread("0_building.jpg");
    
	if (colorImage.empty()) 
    {
        cout << "Cannot load image!" << endl;
        return -1;
    }
	
	// "Loading" screen
	cout << "Processing image. Please wait..." << endl;


	// - - - - - - - - - APPLY IMAGE PROCESSING - - - - - - - - - 
	// Convert color image to grayscale
	Mat gray = ConvertColorImageToBlackWhite(colorImage);
	
	// Mean filter applied(black and white only)
	Mat mean = MeanFilter(gray);

	// Grayscale threshold
	Mat threshold = ThresholdBlackWhiteImage(gray, THRESHOLD_GRAYSCALE);

	// Erosion
	Mat erosion = Erosion(threshold, 1);

	// Finding outline using the eroded image, by subtracting the original grayscale from the eroded image
	Mat erosionOutline = threshold - erosion;

	// Edge detecting using the Sobel kernel
	Mat edge_diagonal_right = SobelEdgeDetecting(gray, Diagonal_Right, true, THRESHOLD_SOBEL);
	Mat edge_diagonal_left = SobelEdgeDetecting(gray, Diagonal_Left, true, THRESHOLD_SOBEL);
	Mat edge_vertical = SobelEdgeDetecting(gray, Vertical, true, THRESHOLD_SOBEL);
	Mat edge_horizontal = SobelEdgeDetecting(gray, Horizontal, true, THRESHOLD_SOBEL);

	// Combine the different kernels
	Mat vertical_plus_horizontal = AddTwoMatsTogether(edge_vertical, edge_horizontal);
	Mat diagonal_right_plus_left = AddTwoMatsTogether(edge_diagonal_right, edge_diagonal_left);
	Mat diagonal_plus_vertical_horizontal = AddTwoMatsTogether(vertical_plus_horizontal, edge_diagonal_right);

	// - - - - - - - - - - - - - - - - - - 

	// Save the images
	imwrite("1_grayscale.jpg", gray);
	imwrite("2_meanFilter.jpg", mean);
	imwrite("3_threshold.jpg", threshold);
	imwrite("4_erosion.jpg", erosion);
	imwrite("5_erosionOutline.jpg", erosionOutline);
	imwrite("6_edge_diagonal_right.jpg", edge_diagonal_right);
	imwrite("7_edge_diagonal_left.jpg", edge_diagonal_left);
	imwrite("8_edge_vertical.jpg", edge_vertical);
	imwrite("9_edge_horizontal.jpg", edge_horizontal);
	imwrite("10_vertical_plus_horizontal.jpg", vertical_plus_horizontal);
	imwrite("11_diagonal_right_plus_left.jpg", diagonal_right_plus_left);
	imwrite("12_diagonal_plus_vertical_horizontal.jpg", diagonal_plus_vertical_horizontal);

	// Show the images
	imshow("original color image", colorImage);
    imshow("grayscale", gray);
	imshow("meanFilter", mean);
	imshow("threshold", threshold);
	imshow("erosion", erosion);
	imshow("erosionOutline", erosionOutline);
	imshow("edge_diagonal_right", edge_diagonal_right);
	imshow("edge_diagonal_left", edge_diagonal_left);
	imshow("edge_vertical", edge_vertical);
	imshow("edge_horizontal", edge_horizontal);
	imshow("vertical_plus_horizontal", vertical_plus_horizontal);
	imshow("diagonal_right_plus_left", diagonal_right_plus_left);
	imshow("diagonal_plus_vertical_horizontal", diagonal_plus_vertical_horizontal);
    waitKey(0);
}

Mat ConvertColorImageToBlackWhite(Mat colorImage)
{
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
			float grayValue = colorImage.at<cv::Vec3b>(y, x)[0] * BlueWeight
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

				// Absolute value
				if (temp < 0)
					temp *= -1;

				// Map values from 0 to 255
				if (temp <= threshold)
					temp = 0;
				else
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

				// Absolute value
				if (temp < 0)
					temp *= -1;

				// Map values from 0 to 255
				if (temp <= threshold)
					temp = 0;
				else
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

				// Absolute value
				if (temp < 0)
					temp *= -1;

				// Map values from 0 to 255
				if (temp <= threshold)
					temp = 0;
				else
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

				// Absolute value
				if (temp < 0)
					temp *= -1;

				// Map values from 0 to 255
				if (temp <= threshold)
					temp = 0;
				else
					temp = 255;
				

				edge.at<uchar>(y, x) = temp;
			}
		}
	}
	else
	{
		// Error text
		putText(edge, "ERROR - Sobel type not defined!", Point(10, 50), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 4, 8, false);
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

// Uses a grayscale image
Mat Erosion(Mat input, int radius)
{
	Mat output = input.clone();

	for(int x = radius; x < input.cols-radius; x++)
	{
		for(int y = radius; y < input.rows-radius; y++)
		{
			bool pixelIsaccepted = true;
			for(int filterX = x - radius; pixelIsaccepted && filterX <= x + radius; filterX++)
			{
				for(int filterY = y - radius; pixelIsaccepted && filterY <= y + radius; filterY++)
				{
					if (input.at<uchar>(filterY,filterX) == 0)
					{
						pixelIsaccepted = false;
					}
				}
			}
			if (pixelIsaccepted == true)
				output.at<uchar>(y,x) = 255;
			else
				output.at<uchar>(y,x) = 0;
		}
	}

	return output;
}