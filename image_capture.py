import pyzed.sl as sl
import argparse
import os
import cv2
from datetime import datetime
 
  # Initialize parser
parser = argparse.ArgumentParser(description="Acquire images of ZED 2 camera")

  # Adding optional arguments
parser.add_argument("--resolution", "-r", help = "Image resolution: 2000, 1080, 720, 640 (default: 720)", default=720, type=int)
parser.add_argument("--type", "-t", help = "Image Type: raw or depth (default: raw)", default="raw", type=str)
parser.add_argument("--path", "-p", help = "Saving path (default: /home/nvidia/Pictures/)", default="/home/nvidia/Pictures/", type=str)

  # Read arguments from command line
args = parser.parse_args()


    
def main():
    folder_count = 0
    key_pressed = "p" 
    created = False 
    while created == False:
        if not os.path.isdir(args.Path + datetime.today().strftime('%Y-%m-%d')+"-E"+str(folder_count)): 
            folder = os.path.join(args.Path + datetime.today().strftime('%Y-%m-%d')+"-E"+str(folder_count)) # create folder /home/nvidia/Pictures/YYYY-mm-dd-Ex/
            os.makedirs(folder)
            print(folder)
            created = True
        else:
            folder_count += 1


    # Create a Camera object
    zed = sl.Camera()

    # Define parameters based on resolution arg
    init_params = sl.InitParameters()
    if args.Resolution == 2000:
        init_params.camera_resolution = sl.RESOLUTION.HD2K  # Use HD1080 video mode
        init_params.camera_fps = 30  # Set fps at 30
    elif args.Resolution == 1080:
        init_params.camera_resolution = sl.RESOLUTION.HD1080  # Use HD1080 video mode
        init_params.camera_fps = 30  # Set fps at 30
    elif args.Resolution == 720:
        init_params.camera_resolution = sl.RESOLUTION.HD720  # Use HD720 video mode
        init_params.camera_fps = 60  # Set fps at 60
    elif args.Resolution == 640:
        init_params.camera_resolution = sl.RESOLUTION.VGA  # Use VGa video mode
        init_params.camera_fps = 60  # Set fps at 60

    # Open the camera
    err = zed.open(init_params)
    if err != sl.ERROR_CODE.SUCCESS:
        exit(1)

    image_count = 0
    runtime_parameters = sl.RuntimeParameters()
        
    while key_pressed != "q":
        # Grab an image, a RuntimeParameters object must be given to grab()
        if zed.grab(runtime_parameters) == sl.ERROR_CODE.SUCCESS: # A new image is available if grab() returns SUCCESS
            # If Type arg brings raw option
            if args.Type == "raw":
              image_left = sl.Mat()
              image_right = sl.Mat()
              zed.retrieve_image(image_left, sl.VIEW.LEFT)
              zed.retrieve_image(image_right, sl.VIEW.RIGHT)
              img_L = cv2.imwrite(os.path.join(folder,"left_"+str(image_count)+".png"), image_left.get_data())
              img_R = cv2.imwrite(os.path.join(folder, "right_"+str(image_count)+".png"),image_right.get_data())
            # If Type arg brings depth option  
            elif args.Type == "depth":
              depth_image = sl.Mat()
              zed.retrieve_image(depth_image, sl.VIEW.DEPTH)
              depth_img = cv2.imwrite(os.path.join(folder,"depth_"+str(image_count)+".png"), depth_image.get_data()) 
              
            #timestamp = zed.get_timestamp(sl.TIME_REFERENCE.CURRENT)  # Get the timestamp at the time the image was captured
            #print("Image resolution: {0} x {1} || Image timestamp: {2}\n".format(image.get_width(), image.get_height(),
                  #timestamp.get_milliseconds()))
            image_count += 1
            print("Image captured")
            key_pressed = input("Press Enter to continue..(Q to quit)\n")
            

    # Close the camera
    zed.close()

if __name__ == "__main__":
        
    main()
