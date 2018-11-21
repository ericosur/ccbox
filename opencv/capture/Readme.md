# capture

simple sample to use capture in opencv


## note

* use cmake to build this project


## CMakeLists.txt

There are two more examples from opencv samples:
  - findContours_demo.cpp
  - HoughLines_Demo.cpp


## setting
change "video_id" if extra external webcam is inserted, for example, ZR300


```json
{
    "video_id": 0,
    "description": "will use /dev/video0"
}
```

## realsense D415

At my NB oa18, after inserting D415, use /dev/video3 for rgb images

