///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/****************************************************************************************
** This sample shows how to record video in Stereolabs SVO format.					   **
** SVO video files can be played with the ZED API and used with its different modules  **
*****************************************************************************************/

// ZED includes
#include <sl/Camera.hpp>

// Sample includes
#include "utils.hpp"

// Using namespace


void print(std::string msg_prefix, sl::ERROR_CODE err_code = sl::ERROR_CODE::SUCCESS, std::string msg_suffix = "");
void parseArgs(int argc, char **argv,sl::InitParameters& param);

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cout << "Usage : Only the path of the output SVO file should be passed as argument.\n";
        return EXIT_FAILURE;
    }

    // Create a ZED camera
    sl::Camera zed;

    // Set configuration parameters for the ZED
    sl::InitParameters init_parameters;
    init_parameters.camera_resolution = sl::RESOLUTION::HD720;
    init_parameters.depth_mode = sl::DEPTH_MODE::ULTRA;
    init_parameters.camera_image_flip = sl::FLIP_MODE::ON;
    init_parameters.coordinate_units = sl::UNIT::MILLIMETER;
    parseArgs(argc,argv,init_parameters);

    // Open the camera
    auto returned_state  = zed.open(init_parameters);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        print("Camera Open", returned_state, "Exit program.");
        return EXIT_FAILURE;
    }

    // Enable recording with the filename specified in argument
    sl::String path_output(argv[1]);
    returned_state = zed.enableRecording(sl::RecordingParameters(path_output, sl::SVO_COMPRESSION_MODE::H264));
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        print("Recording ZED : ", returned_state);
        zed.close();
        return EXIT_FAILURE;
    }

    // Start recording SVO, stop with Ctrl-C command
    print("SVO is Recording, use Ctrl-C to stop." );
    SetCtrlHandler();
    int frames_recorded = 0;
    sl::RecordingStatus rec_status;
    while (!exit_app) {
        if (zed.grab() == sl::ERROR_CODE::SUCCESS) {
            // Each new frame is added to the SVO file
            rec_status = zed.getRecordingStatus();
            if (rec_status.status)
                frames_recorded++;
            print("Frame count: " +std::to_string(frames_recorded));
        }
    }

    // Stop recording
    zed.disableRecording();
    zed.close();
    return EXIT_SUCCESS;
}

void print(std::string msg_prefix, sl::ERROR_CODE err_code, std::string msg_suffix) {
    std::cout <<"[Sample]";
    if (err_code != sl::ERROR_CODE::SUCCESS)
        std::cout << "[Error] ";
    else
        std::cout<<" ";
    std::cout << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS) {
        std::cout << " | " << toString(err_code) << " : ";
        std::cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        std::cout << " " << msg_suffix;
    std::cout << std::endl;
}

void parseArgs(int argc, char **argv,sl::InitParameters& param)
{
    if (argc > 2 && std::string(argv[2]).find(".svo")!=std::string::npos) {
        // SVO input mode
        std::cout<<"[sample][Warning] SVO input is not supported... switching to live mode"<<std::endl;
    } else if (argc > 2 && std::string(argv[2]).find(".svo")==std::string::npos) {
        std::string arg = std::string(argv[2]);
        unsigned int a,b,c,d,port;
        if (sscanf(arg.c_str(),"%u.%u.%u.%u:%d", &a, &b, &c, &d,&port) == 5) {
            // Stream input mode - IP + port
            std::string ip_adress = std::to_string(a)+"."+std::to_string(b)+"."+std::to_string(c)+"."+std::to_string(d);
            param.input.setFromStream(sl::String(ip_adress.c_str()),port);
            std::cout<<"[Sample] Using Stream input, IP : "<<ip_adress<<", port : "<<port<<std::endl;
        }
        else  if (sscanf(arg.c_str(),"%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
            // Stream input mode - IP only
            param.input.setFromStream(sl::String(argv[2]));
            std::cout<<"[Sample] Using Stream input, IP : "<<argv[2]<<std::endl;
        }
        else if (arg.find("HD2K")!=std::string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD2K;
            std::cout<<"[Sample] Using Camera in resolution HD2K"<<std::endl;
        } else if (arg.find("HD1080")!=std::string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD1080;
            std::cout<<"[Sample] Using Camera in resolution HD1080"<<std::endl;
        } else if (arg.find("HD720")!=std::string::npos) {
            param.camera_resolution = sl::RESOLUTION::HD720;
            std::cout<<"[Sample] Using Camera in resolution HD720"<<std::endl;
        } else if (arg.find("VGA")!=std::string::npos) {
            param.camera_resolution = sl::RESOLUTION::VGA;
            std::cout<<"[Sample] Using Camera in resolution VGA"<<std::endl;
        }
    } else {
        // Default
    }
}
