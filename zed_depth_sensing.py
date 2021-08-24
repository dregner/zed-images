import sys
import ogl_viewer.viewer as gl
import pyzed.sl as sl
import cv2
if __name__ == "__main__":
    init = sl.InitParameters(camera_resolution=sl.RESOLUTION.HD720,
                                 depth_mode=sl.DEPTH_MODE.PERFORMANCE,
                                 coordinate_units=sl.UNIT.METER,
                                 coordinate_system=sl.COORDINATE_SYSTEM.RIGHT_HANDED_Y_UP)

    init.depth_minimum_distance = 0.3
    init.depth_maximum_distance = 5
    zed = sl.Camera()

    status = zed.open(init)
    if status != sl.ERROR_CODE.SUCCESS:
        print(repr(status))
        exit()

    res = sl.Resolution()
    res.width = 640
    res.height = 480

    camera_model = zed.get_camera_information().camera_model
    # Create OpenGL viewer
    #viewer = gl.GLViewer()
    #viewer.init(len(sys.argv), sys.argv, camera_model, res)

    point_cloud = sl.Mat(res.width, res.height, sl.MAT_TYPE.F32_C4, sl.MEM.CPU)
    depth_zed = sl.Mat(zed.get_camera_information().camera_resolution.width, zed.get_camera_information().camera_resolution.height, sl.MAT_TYPE.F32_C1)
    image = sl.Mat()
    key = ''
    while True:
        if zed.grab() == sl.ERROR_CODE.SUCCESS:

            zed.retrieve_measure(depth_zed, sl.MEASURE.DEPTH, sl.MEM.CPU)
            #zed.retrieve_measure(point_cloud, sl.MEASURE.XYZRGBA,sl.MEM.CPU, res)
            #viewer.updateData(point_cloud)
            zed.retrieve_image(image, sl.VIEW.DEPTH)

            cv2.imshow("ZED", image.get_data())
            key = cv2.waitKey(5)

            # Load depth data into a numpy array
            depth_ocv = depth_zed.get_data()
            # Print the depth value at the center of the image
            print(depth_ocv[int(len(depth_ocv)/2)][int(len(depth_ocv[0])/2)])

    viewer.exit()
    zed.close()
