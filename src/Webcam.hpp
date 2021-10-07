#ifndef __WEBCAM_HPP__
#define __WEBCAM_HPP__

#include <opencv2/opencv.hpp>
#include <cstring>
#include <vector>
#include "LittleUtils.hpp"
#include "Aruco.hpp"

class Webcam{

public:
    cv::Mat intrinsic, distortion;
    // Store the intrinsic and distortion matrix;

    void cameraCalibration(cv::Size boardSize, cv::VideoCapture &video)
    {
        cv::namedWindow("Frame", cv::WINDOW_NORMAL);
        cv::moveWindow("Frame", 0, 0);
        cv::resizeWindow("Frame", cv::Size(960, 540));

        int boardCorNum = boardSize.width * boardSize.height;
        
        cv::Size imageSize;
        
        std::vector<std::vector<cv::Point2f> > imagePoints;
        std::vector<std::vector<cv::Point3f> > objectPoints;
        
        cv::Mat image;
        while (video.read(image)){
            if (!image.data) {
                showError("Frame is not an image");
                continue;
            }
            imageSize = image.size();
            std::vector<cv::Point2f> corners;
            bool found = findChessboardCorners(image, boardSize, corners);

            if (found) {
                drawChessboardCorners(image, boardSize, corners, found);
                image ^= cv::Scalar::all(255);
                cv::Mat mcorners(corners);

                imagePoints.push_back(corners);
                objectPoints.push_back(std::vector<cv::Point3f>());
                std::vector<cv::Point3f> &opts = objectPoints.back();

                opts.resize(boardCorNum);
                for (int j = 0; j < boardCorNum; j++) {
                    opts[j] =
                        cv::Point3f(
                            static_cast<float>(j / boardSize.width),
                            static_cast<float>(j % boardSize.width), 0.0f
                        );
                }

                cv::imshow("Frame", image);
                cv::waitKey(200);

            }
        }

        double err = cv::calibrateCamera(
            objectPoints, imagePoints, imageSize,
            intrinsic, distortion, cv::noArray(), cv::noArray(),
            cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);


    }

    void cameraCalibration(cv::Size boardSize, std::string caliFolder)
    {
        int boardCorNum = boardSize.width * boardSize.height;
        std::vector<std::string> filenames;
	    std::cout << "Reading in directory " << caliFolder << std::endl;
        cv::glob(caliFolder, filenames);
        int num_files = static_cast<int>(filenames.size());
	    std::cout << "   ... Done. Number of files = " << num_files << std::endl;

        cv::Size imageSize;
        
        std::vector<std::vector<cv::Point2f> > imagePoints;
        std::vector<std::vector<cv::Point3f> > objectPoints;
        
        for (size_t i = 0; i < filenames.size(); ++i) {
            cv::Mat image = cv::imread(filenames[i]);
            if (!image.data) {
                std::cerr << filenames[i] << ", file #" << i << ", is not an image" << std::endl;
                continue;
            }
            imageSize = image.size();
            
            std::vector<cv::Point2f> corners;
            bool found = findChessboardCorners(image, boardSize, corners);

            drawChessboardCorners(image, boardSize, corners, found);

            if (found) {
                image ^= cv::Scalar::all(255);
                cv::Mat mcorners(corners);

                imagePoints.push_back(corners);
                objectPoints.push_back(std::vector<cv::Point3f>());
                std::vector<cv::Point3f> &opts = objectPoints.back();

                opts.resize(boardCorNum);
                for (int j = 0; j < boardCorNum; j++) {
                    opts[j] =
                        cv::Point3f(
                            static_cast<float>(j / boardSize.width),
                            static_cast<float>(j % boardSize.width), 0.0f
                        );
                }
                std::cout << "Collected from image @ " << filenames[i] << std::endl;
            }
        }

        std::cout << "\n\n*** CALIBRATING THE CAMERA...\n" << std::endl;

        double err = cv::calibrateCamera(
            objectPoints, imagePoints, imageSize,
            intrinsic, distortion, cv::noArray(), cv::noArray(),
            cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

        std::cout << " *** DONE!\n\nReprojection error is " << err << std::endl;
    }

    void cameraParaOutput(std::string caliFile)
    {
        if (intrinsic.empty()) {
            showError("Empty Camera Parameters");
            return;
        }
	    cv::FileStorage fs(caliFile, cv::FileStorage::WRITE);
        fs  << "camera_matrix" << intrinsic
            << "distortion_coefficients" << distortion;
        fs.release();
    }

    void cameraParaInput(std::string caliFile)
    {
	    cv::FileStorage fs(caliFile, cv::FileStorage::READ);
        fs["camera_matrix"] >> intrinsic;
        fs["distortion_coefficients"] >> distortion;
        fs.release();
    }

    void cameraParaOutput(){
        if (intrinsic.empty()) {
            showError("Empty Camera Parameters");
            return;
        }
        std::cout   << "camera_matrix:\n" << intrinsic << std::endl
                    << "distortion_coefficients:\n" << distortion << std::endl << std::endl;
    }

    void drawChessBoard(cv::Mat & image, cv::Size boardSize){

        std::vector<cv::Point2f> corners;
        bool found = cv::findChessboardCorners(image, boardSize, corners);
        if (found && corners.size() == boardSize.height * boardSize.width){
            std::cout << "Find!" << std::endl;
            cv::drawChessboardCorners(image, boardSize, corners, found);
        }
    }

    bool getChessBoardPosition(cv::Mat& image, cv::Size boardSize, std::vector <cv::Point3f>& PcamResult, std::vector<cv::Point2f>& corners){

        if (image.empty()) return false;

        PcamResult.clear();
        corners.clear();

        bool found = cv::findChessboardCorners(image, boardSize, corners);
        std::vector<cv::Point3f> object;

        int boardCorNum = boardSize.width * boardSize.height;
        // std::cout << "boardCorNum = " << boardCorNum << std::endl;
        object.resize(boardCorNum);

        if (found){
            for (int j=0; j<boardCorNum; ++j){
                object[j] = 
                    cv::Point3f(
                        static_cast<float>(j / boardSize.width),
                        static_cast<float>(j % boardSize.width), 0.0f
                    );
            }
        } else {
            // std::cout << "Not Find the Chess Board" << found << std::endl;
            return false;
        }

        // std::cout << "Corner Counter = " << corners.size() << "; " << object.size() << std::endl;

        cv::Mat raux;
        cv::Mat taux;

        cv::solvePnP(object, corners, intrinsic, distortion, raux, taux, false);

        cv::Mat rVec;
        cv::Mat tVec;

        raux.convertTo(rVec, CV_64F);
        taux.convertTo(tVec, CV_64F);

        cv::Mat rotMat(cv::Size(3, 3), CV_64F);
        cv::Rodrigues(rVec, rotMat);

        cv::Mat rotMatInv(cv::Size(3, 3), CV_64F);
        cv::invert(rotMat, rotMatInv);
        
        cv::Mat tVecTrs;
        cv::transpose(tVec, tVecTrs);

        // P_cam = RP_world + T;
        // let the P_cam = (0, 0, 0) , we can get the position of the camera, which z denotes the depth
        // P_world = R.inverse * (P_cam - T);
        cv::Mat cameraPosition = -rotMatInv*tVec;
        // std::cout << "Camera Position :\n" << cameraPosition << std::endl;

        // However, if we want the Position of the Surface, and set the Camera fixed ?
        // but, the axis in picture and in the screen is not the same, we need to negate it

        for (int i=0; i<object.size(); ++i){
            double objectPosition[1][3] = {object[i].x, object[i].y, object[i].z};
            cv::Mat Pworld(cv::Size(1, 3), CV_64F, objectPosition);
            cv::Mat Pcam = rotMat * Pworld + tVec; 
            cv::Point3f Pcam3f(Pcam);

            // cv::Mat Pscreen;
            // cv::transpose(intrinsic * Pcam, Pscreen);
            // cv::Point3d Psrceen3d(Pscreen);
            // std::cout << "Pscreen = " << Psrceen3d / Psrceen3d.z << "; PCorners = " << corners[i] << std::endl;

            PcamResult.emplace_back(-Pcam3f.x, -Pcam3f.y, Pcam3f.z); // Make the vision Look the same as the picture
        }
        return true;
    }

    bool get3Dto2DTransform(const std::vector<cv::Point3f>& objectPosition, const std::vector<cv::Point2f>& pixelPosition, cv::Mat &rotMat, cv::Mat &tVec){
        
        cv::Mat raux;
        cv::Mat taux;

        cv::solvePnP(objectPosition, pixelPosition, intrinsic, distortion, raux, taux, false);
        
        cv::Mat rVec;

        raux.convertTo(rVec, CV_64F);
        taux.convertTo(tVec, CV_64F);

        cv::Rodrigues(rVec, rotMat);

        cv::Mat rotMatInv(cv::Size(3, 3), CV_64F);
        cv::invert(rotMat, rotMatInv);

        return true;
    }

    bool getCharucoCornersPosition(const std::vector<int> &charucoIds, const std::vector<cv::Point2f> &charucoCorners, std::vector<cv::Point3f> &charucoCornersPositions) {
        std::vector<cv::Point3f> charucoCorner3DPosition;
        charucoCorner3DPosition.clear();
        for (int i=0; i<charucoIds.size(); ++i){
            charucoCorner3DPosition.emplace_back(charucoIds[i]/boardSizeDefault.width, charucoIds[i]%boardSizeDefault.width, 0.0f);
        }

        cv::Mat rotMat;
        cv::Mat tVec;

        get3Dto2DTransform(charucoCorner3DPosition, charucoCorners, rotMat, tVec);

        charucoCornersPositions.clear();
        for (int i=0; i<charucoIds.size(); ++i){
            double objectPosition[1][3] = {charucoCorner3DPosition[i].x, charucoCorner3DPosition[i].y, charucoCorner3DPosition[i].z};
            cv::Mat Pworld(cv::Size(1, 3), CV_64F, objectPosition);
            cv::Mat Pcam = rotMat * Pworld + tVec; 
            cv::Point3f Pcam3f(Pcam);
            charucoCornersPositions.emplace_back(-Pcam3f.x, -Pcam3f.y, Pcam3f.z); // Make the vision Look the same as the picture
        }
        return true;
    }

    void iamgeUndistortShow(cv::Mat &image){
        
        cv::namedWindow("Frame", cv::WINDOW_NORMAL);
        cv::moveWindow("Frame", 0, 0);
        cv::resizeWindow("Frame", cv::Size(960, 540));

        cv::Mat imageResult;
        cv::undistort(image, imageResult, intrinsic, distortion, intrinsic);

        cv::imshow("Frame", imageResult);
        cv::waitKey(0);
    }

    void imageUndistort(cv::Mat &inputImage, cv::Mat &outputImage){
        cv::undistort(inputImage, outputImage, intrinsic, distortion, intrinsic);
    }

    void interpolateValueX(float x, cv::Point2f& p, cv::Point3f& v, cv::Point2f& pl, cv::Point2f& pr, cv::Point3f& vl, cv::Point3f& vr){
        p = (pl*(pr.x - x) + pr*(x - pl.x)) / (pr.x - pl.x);
        v = (vl*(pr.x - x) + vr*(x - pl.x)) / (pr.x - pl.x);
    }

    void interpolateValueY(float y, cv::Point2f& p, cv::Point3f& v, cv::Point2f& pl, cv::Point2f& pr, cv::Point3f& vl, cv::Point3f& vr){
        p = (pl*(pr.y - y) + pr*(y - pl.y)) / (pr.y - pl.y);
        v = (vl*(pr.y - y) + vr*(y - pl.y)) / (pr.y - pl.y);
    }

    bool interpolationPosition(cv::Point2f point, cv::Point3f& value, std::vector<cv::Point2f> controlPoints, std::vector<cv::Point3f> controlPointsValue, cv::Size controlSize){
        // std::cout << "Try Interpolation = " << point << std::endl;
        for (int i=0; i<controlSize.height-1; ++i){
            for (int j=0; j<controlSize.width-1; ++j){
                if (pointInQuad(point, controlPoints[i*controlSize.width+j], controlPoints[i*controlSize.width+j+1], controlPoints[(i+1)*controlSize.width+j], controlPoints[(i+1)*controlSize.width+j+1])){
                    
                    // std::cout << point << "In Quad " <<  controlPoints[i*controlSize.width+j]  << ", " << controlPoints[i*controlSize.width+j+1] << ", " << controlPoints[(i+1)*controlSize.width+j] << ", " << controlPoints[(i+1)*controlSize.width+j+1] << std::endl;
                    // std::cout << "Value Quad : " << controlPointsValue[i*controlSize.width+j]  << ", " << controlPointsValue[i*controlSize.width+j+1] << ", " << controlPointsValue[(i+1)*controlSize.width+j] << ", " << controlPointsValue[(i+1)*controlSize.width+j+1] << std::endl;
                    cv::Point3f v1, v2, v;
                    cv::Point2f p1, p2, p;
                    interpolateValueX(
                        point.x, p1, v1,
                        controlPoints[i*controlSize.width+j], controlPoints[i*controlSize.width+j+1],
                        controlPointsValue[i*controlSize.width+j], controlPointsValue[i*controlSize.width+j+1]);
                    interpolateValueX(
                        point.x, p2, v2,
                        controlPoints[(i+1)*controlSize.width+j], controlPoints[(i+1)*controlSize.width+j+1],
                        controlPointsValue[(i+1)*controlSize.width+j], controlPointsValue[(i+1)*controlSize.width+j+1]);
                    // std::cout << "Middle " << p1 << ": " <<v1 << std::endl << p2 << ": " << v2 << std::endl;
                    interpolateValueY(
                        point.y, p, value,
                        p1, p2,
                        v1, v2
                    );
                    // std::cout << "Inter Result" << value << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    void getLaserPosition(cv::Mat &inputImage, cv::Mat &outputImage, std::vector<cv::Point2f> corner, std::vector<cv::Point3f> cornerPosition, cv::Size boardSize, std::vector<cv::Point3f>& laserPosition, double threshold = 0.5 ){
        laserPosition.clear();
        outputImage = cv::Mat(inputImage.size(), CV_64FC1);
        // std::cout << "Input Image type = " << inputImage.type() << std::endl;
        // std::cout << "Input Image Channels = " << inputImage.channels() << std::endl;
        cv::Size imageSize = inputImage.size();
        for (int i=0; i<imageSize.height; ++i){
            for (int j=0; j<imageSize.width; ++j)
            if (pointInQuad(cv::Point2f(j*1.0f, i*1.0f), corner[0], corner[boardSize.width-1], corner[(boardSize.height-1)*boardSize.width], corner[(boardSize.height*boardSize.width - 1)] )){
                // outputImage.at<double>(i, j) = 0.8f;
                cv::Vec3b pixel = inputImage.at<cv::Vec3b>(i, j);
                double temp1 = (pixel[2] - (pixel[0] + pixel[1]) / 2.0 ) / 255.0;
                outputImage.at<double>(i, j) = temp1 > threshold ? 1 : 0;
                if (temp1>threshold){
                    cv::Point3f pixelPosition;
                    bool result = interpolationPosition(cv::Point2f(j*1.0f, i*1.0f), pixelPosition, corner, cornerPosition, boardSize);
                    if ( !result ) continue;
                    laserPosition.push_back(pixelPosition);
                }
                
            } else {
                outputImage.at<double>(i, j) = 0.0f;
            }
        }
    }

    void getCharucoPosition(cv::Mat &inputImage, std::vector<int> &markerIds, std::vector<std::vector<cv::Point2f> > &markerCorners, std::vector<int> &charucoIds, std::vector<cv::Point2f> &charucoCorners) {
        
        cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
        params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;

        cv::aruco::detectMarkers(inputImage, dictionaryDefault, markerCorners, markerIds, params);

        if (markerIds.size() > 0) {
            cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, inputImage, charucoBoardDefault, charucoCorners, charucoIds);
            if (charucoIds.size() > 0)
                cv::aruco::drawDetectedCornersCharuco(inputImage, charucoCorners, charucoIds, cv::Scalar(255, 0, 0));
        }

        return;

    }

    void drawCharucoPosition(cv::Mat &inputImage, std::vector<int> &markerIds, std::vector<std::vector<cv::Point2f> > &markerCorners, std::vector<int> &charucoIds, std::vector<cv::Point2f> &charucoCorners){
        getCharucoPosition(inputImage, markerIds, markerCorners, charucoIds, charucoCorners);
        // std::cout << "Find Corners = " << charucoIds.size() << std::endl;
        if (charucoIds.size() > 0)
            cv::aruco::drawDetectedCornersCharuco(inputImage, charucoCorners, charucoIds, cv::Scalar(255, 0, 0));

    }

};

#endif