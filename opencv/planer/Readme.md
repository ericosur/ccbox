# Readme

Scripts in this folder comes from opencv sample/python
```
opencv-3.4.4/samples/python
```

Main script and helper modules are also copied.

## file list

feature_homography.py
video.py
common.py
plane_tracker.py
tst_scene_render.py


## my own additional

myutil.py
simple_orb.py *_*not work*_*
readim.py

## feature_homography.py

This script comes from python sample of openCV, run it and use mouse dragging
a bounding box. It will draw keypoints and related if they are matched.

Modified version will save the frame and bounding box, if no newer bounding box
is provided, it will use last-saved setting to extract features.

> using webcam

### homo.json

This json file is temporary use for frame and its rect. Use __setting.json__ to
configure settings.
