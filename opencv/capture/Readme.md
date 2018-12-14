# capture

use opencv + librealsense to perform hough line detection, and depth measurement

## note

* use cmake to build this project
* needed libraries:
  * opencv
  * librealsense

## about 'Capture'

see [Capture.md](Capture.md)

## CMakeLists.txt

There are two more examples from opencv samples:
  - findContours_demo.cpp
  - HoughLines_Demo.cpp


## setting
change "video_id" if extra external webcam is inserted, for example, ZR300, D415


```json
{
    "video_id": 0,
    "description": "will use /dev/video0"
}
```

## realsense D415

At my NB oa18, after inserting D415, use /dev/video3 for rgb images
