#pragma once

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdexcept>

class CalibrationData
{
  public:
    CalibrationData()
    {
      cameraMatrix = cv::Mat::eye(3,3, CV_64F);
      distCoeffs   = cv::Mat::zeros(5,1, CV_64F);
      imageSize    = cv::Size(0, 0);
    }

    void save(std::string const & filename)
    {
      cv::FileStorage fs(filename, CV_STORAGE_WRITE);
      if(!fs.isOpened()) throw std::runtime_error("Can not open file " + filename);

      fs << "ImageWidth" << imageSize.width;
      fs << "ImageHeight" << imageSize.height;
      fs << "CameraMatrix" << cameraMatrix;
      fs << "DistortionCoefficients" << distCoeffs;

      fs.release();
    }

    void load(std::string const & filename)
    {
      cv::FileStorage fs(filename, CV_STORAGE_READ);
      if(!fs.isOpened()) throw std::runtime_error("Can not open file " + filename);

      fs["ImageWidth"] >> imageSize.width;
      fs["ImageHeight"] >> imageSize.height;
      fs["CameraMatrix"] >> cameraMatrix;
      fs["DistortionCoefficients"] >> distCoeffs;

      fs.release();
    }

    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Size imageSize;
};
