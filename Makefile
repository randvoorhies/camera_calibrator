all: calibrate showcalibration

calibrate: calibrate.cpp CalibrationData.hpp
	g++ -std=c++11 calibrate.cpp -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_imgproc -o calibrate

showcalibration: showcalibration.cpp CalibrationData.hpp
	g++ -std=c++11 showcalibration.cpp -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_imgproc -o showcalibration
