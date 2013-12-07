Simple Monocular Camera Calibrator
==================================

A simple utility for calibrating a single camera using OpenCV.

This tool will recover the 3x3 intrinsic camera matrix, as well as the
5 distortion parameters using OpenCV's [calibrateCamera](http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#calibratecamera) method.

### Contents ###

- ```calibrate```: An application to help calibrate a camera using a live view and produce a calibration file.
- ```showcalibration```: An application to undistort a live camera using an existing calibration file.
- ```CalibrationData.hpp```: A class to help with saving and loading of calibration data.
