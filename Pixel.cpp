Mat ConvertColorImageToBlackWhite(Mat colorImage)
{
	// Mat(rows, columns, type)
	// rows = y
	// cols = x
	
	Mat grayScaleImage(colorImage.rows, colorImage.cols, CV_8UC1);

	//Formula for converting from color to grayscale:
	// I = weightR * R + weightG * G + weightB * B

	//Iterate through all the pixels and apply the formula for grayscale
	
	for (int y = 0; y < colorImage.rows; y++) // rows
	{
		for (int x = 0; x < colorImage.cols; x++) // cols
		{
			// Calculate grayscale value
			int grayValue = colorImage.at<cv::Vec3b>(y, x)[0] * BlueWeight
				+ colorImage.at<cv::Vec3b>(y, x)[1] * GreenWeight
				+ colorImage.at<cv::Vec3b>(y, x)[2] * RedWeight;

			grayScaleImage.at<uchar>(y, x) = grayValue;	

		}
	}	
}
	