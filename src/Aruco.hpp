#ifndef __ARUCO_HPP__
#define __ARUCO_HPP__

#include <opencv2/aruco.hpp>
#include <opencv2/aruco/charuco.hpp>
#include <opencv2/opencv.hpp>

cv::aruco::PREDEFINED_DICTIONARY_NAME dictNameDefault = cv::aruco::DICT_6X6_250;
cv::Ptr<cv::aruco::Dictionary> dictionaryDefault = cv::aruco::getPredefinedDictionary(dictNameDefault);
cv::Ptr<cv::aruco::CharucoBoard> charucoBoardDefault = cv::aruco::CharucoBoard::create(7, 5, 0.04f, 0.02f, dictionaryDefault);
cv::Ptr<cv::aruco::GridBoard> arucoBoardDefault = cv::aruco::GridBoard::create(7, 5, 0.04f, 0.02f, dictionaryDefault);
cv::Size boardSizeDefault(6, 4);

void generateArucoMarker(int id, int pixel, cv::Mat &Output, int borderBits){
    cv::aruco::drawMarker(dictionaryDefault, id, pixel, Output, 1);
};

void detectArucoMarker(
    const cv::Mat & inputImage, std::vector<int>& markerIds,
    std::vector<std::vector<cv::Point2f>>& markerCorners,
    std::vector<std::vector<cv::Point2f>>& rejectedCandidates ){
    
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    parameters->cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR;

    cv::aruco::detectMarkers(inputImage, dictionaryDefault, markerCorners, markerIds, parameters, rejectedCandidates);

    for (size_t i=0; i!=markerIds.size(); ++i){
        std::cout << "ID " << i << ": ";
        for (size_t j=0; j!=markerCorners[i].size(); ++j){
            std::cout << markerCorners[i][j] << " ";
        }
        std::cout << "\n";
    }

}

void detectCharucoMarker(
    const cv::Mat& inputImage, std::vector<int>& markerIds,
    std::vector<std::vector<cv::Point2f>>& markerCorners,
    std::vector<std::vector<cv::Point2f>>& rejectedCandidates,
    std::vector<cv::Point2f>& charucoCorners,
    std::vector<int>& charucoIds){

    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR; // Use the subPixel Precious
    cv::aruco::detectMarkers(inputImage, dictionaryDefault, markerCorners, markerIds, params, rejectedCandidates);

}

void drawCharucoMarker(cv::Mat &inputImage){
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners;
    std::vector<std::vector<cv::Point2f>> rejectedCandidates;
    std::vector<cv::Point2f> charucoCorners;
    std::vector<int> charucoIds;

    cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_CONTOUR; // Use the subPixel Precious
    cv::aruco::detectMarkers(inputImage, dictionaryDefault, markerCorners, markerIds, params, rejectedCandidates);
    cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);

}

void generateCharucoBoard(cv::Mat &outputImage) {
    charucoBoardDefault->draw(cv::Size(1920, 1080), outputImage);
}

void generateArucoBoard(cv::Mat &outputImage) {
    arucoBoardDefault->draw(cv::Size(1920, 1080), outputImage);
}

#endif