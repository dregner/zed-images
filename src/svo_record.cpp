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

int main(int argc, char **argv) {

    // Create a ZED camera
    sl::Camera zed;

    // Set configuration parameters for the ZED
    sl::InitParameters init_parameters;
    init_parameters.camera_resolution = sl::RESOLUTION::HD720;
    init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_units = sl::UNIT::METER;
    init_parameters.depth_maximum_distance = 20;
    init_parameters.camera_fps = 60;

    // Open the camera
    auto returned_state  = zed.open(init_parameters);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        print("Camera Open", returned_state, "Exit program.");
        return EXIT_FAILURE;
    }

    // Enable recording with the filename specified in argument
    sl::String path_output("/home/jetson/Documents/teste.svo");
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

