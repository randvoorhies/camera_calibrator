#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CalibrationData.hpp"

int main(int const argc, char const ** argv)
{
  char const * keys =
    "{    h | help      | false      | show help message}"
    "{    f | file      | calib.yaml | calibration yaml file}"
    "{    c | camera    | 0          | camera number}"
    "{    s | checksize | 27         | checker size (in mm)}"
    "{    x | checknumx | 8          | number of checker corners along the x dimension}"
    "{    y | checknumy | 6          | number of checker corners along the y dimensione}"
    ;

  cv::CommandLineParser parser(argc, argv, keys);

  if (parser.get<bool>("help"))
  {
    parser.printParams();
    return 0;
  }

  float checkSize = parser.get<float>("checksize");
  cv::Size checkDims(parser.get<int>("checknumx"), parser.get<int>("checknumy"));

  int capnum = parser.get<int>("camera");

  cv::VideoCapture cap(capnum);
  if(!cap.isOpened())
  {
    std::cerr << "Could not open camera" << std::endl;
    exit(-1);
  }

  cv::namedWindow("image");

  CalibrationData calibData;

  // ----------------------------------------------------------------------
  // Capture Checkerboard Corners
  // ----------------------------------------------------------------------
  std::vector<std::vector<cv::Point2f>> allCorners;
  while(true)
  {
    cv::Mat frame;
    cap >> frame;

    calibData.imageSize = frame.size();

    cv::Mat greyFrame;
    cvtColor(frame, greyFrame, CV_BGR2GRAY);

    std::vector<cv::Point2f> corners;
    bool found = cv::findChessboardCorners(greyFrame, checkDims, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    if(found)
      cv::cornerSubPix(greyFrame, corners, cv::Size(11, 11), cv::Size(-1, -1),
          cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

    cv::drawChessboardCorners(frame, checkDims, corners, found);

    if(found)
      cv::putText(frame,  "Press <space> to capture",  cv::Point(0,  calibData.imageSize.height-10),  cv::FONT_HERSHEY_COMPLEX_SMALL,  1.0,  cv::Scalar(0,  0, 255));
    cv::putText(frame,  "Press <enter> to finish",   cv::Point(0,  calibData.imageSize.height-30),  cv::FONT_HERSHEY_COMPLEX_SMALL,  1.0,  cv::Scalar(0,  0, 255));
    cv::putText(frame,  "Press  <q>    to quit",     cv::Point(0,  calibData.imageSize.height-50),  cv::FONT_HERSHEY_COMPLEX_SMALL,  1.0,  cv::Scalar(0,  0, 255));

    imshow("image", frame);

    int key = cv::waitKey(5);

    if(key == 'q') exit(0);      // [q] to Exit
    else if(key == 13) break;    // [Enter] to end capture
    else if(key == ' ' && found) // [space] to capture a sample
    {
      allCorners.push_back(corners);
      std::cout << allCorners.size() << " frames captured" << std::endl;
    }
  }

  // ----------------------------------------------------------------------
  // Perform Calibration
  // ----------------------------------------------------------------------

  std::cout << "Calibrating... Please wait" << std::endl;

  // Create the "object points" vector
  std::vector<cv::Point3f> objectPoints;
  for(int y = 0; y<checkDims.height; ++y)
    for(int x = 0; x<checkDims.width; ++x)
      objectPoints.push_back(cv::Point3f(y*checkSize, x*checkSize, 0));
  std::vector<std::vector<cv::Point3f>> allObjectPoints(allCorners.size(), objectPoints);

  // Perform the camera calibration optimization
  std::vector<cv::Mat> rvecs, tvecs;
  double error = cv::calibrateCamera(allObjectPoints, allCorners, calibData.imageSize, calibData.cameraMatrix, calibData.distCoeffs, rvecs, tvecs);
  calibData.save(parser.get<std::string>("file"));

  // Make sure there are no NaNs or Infs in the matrices
  if(!cv::checkRange(calibData.cameraMatrix) && cv::checkRange(calibData.distCoeffs))
  {
    std::cerr << "Calibration Failed!" << std::endl;
    exit(-1);
  }

  std::cout << "Finished calibration with RMS error: " << error << std::endl;

  // ----------------------------------------------------------------------
  // Show the undistorted image
  // ----------------------------------------------------------------------
  
  // Create the undistortion maps
  cv::Mat map1, map2;
  cv::initUndistortRectifyMap(calibData.cameraMatrix, calibData.distCoeffs, cv::Mat(), 
      getOptimalNewCameraMatrix(calibData.cameraMatrix, calibData.distCoeffs, calibData.imageSize, 1, calibData.imageSize, 0),
      calibData.imageSize, CV_16SC2, map1, map2);

  cv::namedWindow("undistorted");
  while(true)
  {
    cv::Mat frame, undistFrame;
    cap >> frame;

    cv::remap(frame, undistFrame, map1, map2, cv::INTER_LINEAR);

    cv::imshow("image", frame);
    cv::imshow("undisttorted", undistFrame);

    cv::waitKey(10);
  }

}


