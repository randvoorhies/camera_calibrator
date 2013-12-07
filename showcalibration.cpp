#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CalibrationData.hpp"


int main(int argc, char const ** argv)
{
  char const * keys =
    "{    h | help   | false            | show help message}"
    "{    f | file   | calibration.yaml | calibration yaml file}"
    "{    c | camera | 0                | camera number}"
    ;

  cv::CommandLineParser parser(argc, argv, keys);

  if (parser.get<bool>("help"))
  {
    parser.printParams();
    return 0;
  }
  
  int capnum = parser.get<int>("camera");
  cv::VideoCapture cap(capnum);

  CalibrationData calibData;
  calibData.load(parser.get<std::string>("file"));

  cv::Mat map1, map2;
  cv::initUndistortRectifyMap(calibData.cameraMatrix, calibData.distCoeffs, cv::Mat(), 
      getOptimalNewCameraMatrix(calibData.cameraMatrix, calibData.distCoeffs, calibData.imageSize, 1, calibData.imageSize, 0),
      calibData.imageSize, CV_16SC2, map1, map2);

  cv::namedWindow("undistorted");
  while(true)
  {
    cv::Mat frame, undistFrame;
    cap >> frame;

    if(frame.size() != calibData.imageSize)
    {
      std::cerr << "Frame size does not match calibration image size" << std::endl;
      exit(-1);
    }

    cv::remap(frame, undistFrame, map1, map2, cv::INTER_LINEAR);

    cv::imshow("image", frame);
    cv::imshow("undistorted", undistFrame);

    cv::waitKey(10);
  }

  return 0;
}
