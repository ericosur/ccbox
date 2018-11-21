# Readme

## NOTICE

You need extra module: tracker/tracking to make this project built/compiled.


## howto

Run

```
./tracker MIL 0
```

and frame paused, manually select ROI region and then press "space"
to continue to track. '0' will open /dev/video0 as video feed.



## notes

* MOSSE acts strangely
* GOTURN needs caffe/model ready
* KCF needs opencl/gpu


## reference

Please see the following [blog post](https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/) for more details about this code

[Object Tracking using OpenCV (C++/Python)](https://www.learnopencv.com/object-tracking-using-opencv-cpp-python/)
