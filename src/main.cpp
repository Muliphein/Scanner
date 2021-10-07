#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "ScannerConfig.h"
#include "Webcam.hpp"
#include "SurfaceObject.hpp"
#include "PointCloudObject.hpp"
#include "AxisObject.hpp"
#include "CameraObject.hpp"
#include "Aruco.hpp"
#include "LaserObject.hpp"
#include "PlanarObject.hpp"
#include "MathAlgorithm.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void glfwPreProcess();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// std::string calibrationFile = CMAKE_CALIBRATION_RESULT_PATH+std::string("result.xml");
// std::string testFile = CMAKE_CALIBRATION_PICTURE_PATH+std::string("IMG_0203.jpg");
// std::string lineTestFile = CMAKE_CALIBRATION_TEST_PATH+std::string("IMG_0209.jpg");
// std::string arucoTestFile = CMAKE_LOCATION_PATH+std::string("aruco.jpg");
// std::string charucoTestFile = CMAKE_LOCATION_PATH+std::string("charuco.jpg");
// std::string charucoBoardFile = CMAKE_LOCATION_PATH+std::string("charucoBoard.jpg");
// std::string cameraTestFile = CMAKE_LOCATION_PATH+std::string("test.jpg");
std::string opencvWindow1 = "OpenCV1";
std::string opencvWindow2 = "OpenCV2";


std::string cameraCalibrationVideo = CMAKE_VIDEO_PATH + std::string("Calibration.mp4");
std::string cameraScanVideo = CMAKE_VIDEO_PATH+std::string("ObjectScan.mp4");
std::string cameraCalibrationResult = CMAKE_VIDEO_PATH + std::string("CameraResult.xml");
std::string cameraCalibrationSplitFolder = CMAKE_VIDEO_PATH + std::string("split/");
std::string laserPointsFile = CMAKE_VIDEO_PATH + std::string("LaserPointPosition.txt");

// draw objects
std::vector <BasicObject*> queue;

// opengl window
GLFWwindow* window;

cv::VideoCapture webCameraCapture;

// Set a New WebCam
Webcam myWebCam;

#define SKIPSTEP1CHECK
#define USE_SPLIT_FILE
#define SPLIT_VIDEO
#define SAVE_CALI_RESULT

#define SKIP_CALI_RESULT_DIAPLAY
#define SKIP_STEP_1_SPLIT_VIDEO
// #define SKIP_CALI_RESULT_STORE
#define SKIP_LASER_POINT_CALC

int main()
{
    // Step 1: Split the Video
    #ifndef SKIP_STEP_1_SPLIT_VIDEO
    {
        showProcessStart(" Video Split ");
        
        webCameraCapture.open(cameraCalibrationVideo);
        cv::Mat frameImage;
        int validFrameCounter = 0;
        while (webCameraCapture.read(frameImage)){
            std::vector <cv::Point2f> corners;
            if (cv::findChessboardCorners(frameImage, boardSizeDefault, corners)){
                cv::Mat frameImageCopy;
                frameImage.copyTo(frameImageCopy);
                cv::drawChessboardCorners(frameImageCopy, boardSizeDefault, corners, true);
                cv::imshow("split", frameImageCopy);
                if (cv::waitKey(0) != 27){
                    cv::imwrite(cameraCalibrationSplitFolder + std::to_string(validFrameCounter) + std::string(".jpg"), frameImage);
                    validFrameCounter++;
                    cv::destroyWindow("split");
                } else {
                    cv::destroyWindow("split");
                    continue;
                }
            }
        }
        showProcessEnd(" Video Split ");

    }
    #endif


    // Step 2: Calibration from the Split Files/Result
    if (checkFileExist(cameraCalibrationResult)){
        showProcessStart("Readin Calibration-File");
        myWebCam.cameraParaInput(cameraCalibrationResult);
        showProcessEnd("Readin Calibration-File");
        // Debug Check
        #ifndef SKIP_CALI_RESULT_DIAPLAY
        showProcessStart("Camera Calibration Result Vision");
        webCameraCapture.open(cameraCalibrationVideo);
        cv::Mat image;
        webCameraCapture >> image;
        myWebCam.iamgeUndistortShow(image);
        showProcessEnd("Camera Calibration Result Vision");
        #endif
        // Check Over
    }
    else {
        showProcessStart("Camera Calibration");
        myWebCam.cameraCalibration(boardSizeDefault, cameraCalibrationSplitFolder);
        #ifndef SKIP_CALI_RESULT_STORE
        myWebCam.cameraParaOutput(cameraCalibrationResult);
        #endif
        showProcessEnd("Camera Calibration");
    }



    std::vector <cv::Point3f> laserPoints;

        // OpenGL Init
    glfwPreProcess();
    glfwSetWindowPos(window, 960, 25);
        // Camera Show
    CameraObject myCamera;
        // Axis Show
    AxisObject myAxis;
        // Get the shader
    Shader myShader(
        (CMAKE_SHADER_PATH+std::string("3dVertexShader.glsl")).c_str(),
        (CMAKE_SHADER_PATH+std::string("3dFragmentShader.glsl")).c_str()
    );


    // Step 3: Calculate the Laser Plane
    #ifndef SKIP_LASER_POINT_CALC
    showProcessStart("Calculate the Laser Points");
    std::vector <std::string> fileNames;
    cv::glob(cameraCalibrationSplitFolder, fileNames);
    if (fileNames.empty()){
        showError("No Split File");
        return 0;
    } else {
        std::cout << "Glob " << fileNames.size() << " files" << std::endl;
    }
    queue.push_back(dynamic_cast<BasicObject*>(&myAxis));
    queue.push_back(dynamic_cast<BasicObject*>(&myCamera));

    cv::Mat image;
    int validFramePointer = 0;

    cv::namedWindow(opencvWindow1, cv::WINDOW_NORMAL);
    cv::resizeWindow(opencvWindow1, cv::Size(960, 540));
    cv::moveWindow(opencvWindow1, 0, 0);
    
    cv::namedWindow(opencvWindow2, cv::WINDOW_NORMAL);
    cv::resizeWindow(opencvWindow2, cv::Size(960, 540));
    cv::moveWindow(opencvWindow2, 0, 550);

    while (!glfwWindowShouldClose(window))
    {
        bool getinFilesFlag = true;

        // if (validFramePointer >=1) {
        //     getinFilesFlag = false;
        // }

        if (validFramePointer >= fileNames.size()) {
            // getinFilesFlag = false;
            break;
        }

        if (getinFilesFlag){
            image = cv::imread(fileNames[validFramePointer]);
            validFramePointer++;
            std::cout << "Counter = " << validFramePointer << std::endl;
            imshow(opencvWindow1, image);
            cv::Mat undistortImage, laserImage;
            myWebCam.imageUndistort(image, undistortImage);
            std::vector<cv::Point3f> Pcam;
            std::vector<cv::Point2f> Ppic;
            
            if (!myWebCam.getChessBoardPosition(undistortImage, boardSizeDefault, Pcam, Ppic)) continue;
            // for (int i=0; i<Ppic.size(); ++i){
            //     std::cout << "P " << i << " : " << Ppic[i] << std::endl;
            // }
            std::vector<cv::Point3f> laserPcam;
            myWebCam.getLaserPosition(undistortImage, laserImage, Ppic, Pcam, boardSizeDefault, laserPcam);
            for (auto item: laserPcam)
                laserPoints.push_back(item);

            LaserObject* myLaser = new LaserObject(laserPcam);
            queue.push_back(static_cast<BasicObject*>(myLaser));

            SurfaceObject *mySurface = new SurfaceObject(Pcam, boardSizeDefault);
            queue.push_back(static_cast<BasicObject*>(mySurface));

            imshow(opencvWindow2, laserImage);
        }

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get Input
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        myShader.use();

        // Set the projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        myShader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        myShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        myShader.setMat4("model", model);

        // Render
        for (int i=0; i<queue.size(); ++i){
            BasicObject * pointer = queue[i];
            pointer->preProcess();
            myShader.setVec4("color", pointer->getColor());
            glBindVertexArray(pointer->getVAOHandle());
            glDrawArrays(pointer->getDrawType(), 0, pointer->getPointNum());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (getinFilesFlag){
            BasicObject* pointer = queue[queue.size()-1];
            queue.pop_back();
            free(pointer);
            pointer=nullptr;
        }

    }

    serialize(laserPoints, laserPointsFile);

    cv::destroyWindow(opencvWindow1);
    cv::destroyWindow(opencvWindow2);

    showProcessEnd("Calculate the Laser Points");
    #endif

    // Step 4: Calculate the Laser Plane
    showProcessStart("Calculate the Laser Plane");
    deserialize(laserPoints, laserPointsFile);
    queue.clear();
    queue.push_back(dynamic_cast<BasicObject*>(&myAxis));
    queue.push_back(dynamic_cast<BasicObject*>(&myCamera));
    LaserObject* myLaserPointsCloud = new LaserObject(laserPoints);
    queue.push_back(dynamic_cast<BasicObject*>(myLaserPointsCloud));

    float planarA, planarB, planarC, planarD;
    fitPlanar(laserPoints, planarA, planarB, planarC, planarD);


    PlannarObject* myPlanarObject = new PlannarObject(planarA, planarB, planarC, planarD, cv::Size(12, 12));
    queue.push_back(dynamic_cast<BasicObject*>(myPlanarObject));


    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get Input
        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        myShader.use();

        // Set the projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        myShader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        myShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        myShader.setMat4("model", model);

        // Render
        for (int i=0; i<queue.size(); ++i){
            BasicObject * pointer = queue[i];
            pointer->preProcess();
            myShader.setVec4("color", pointer->getColor());
            glBindVertexArray(pointer->getVAOHandle());
            glDrawArrays(pointer->getDrawType(), 0, pointer->getPointNum());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Glfw terminate
    glfwTerminate();
    return 0;

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void glfwPreProcess(){
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


}

