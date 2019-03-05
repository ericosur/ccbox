# Readme for capture

__capture__ is a sample program that use webcam or realsense depth camera to capture video and try to perform some processing.


## running 'capture'

Just running it without any parameter.
It will search for setting.json by default values and open camera to capture images.

```
./capture
```


## keyboard shortcut

press ESC to quit program
press '**s**' to save images which is marked results
press '**c**' to save original images

> note: original images could be re-analyzed further

## saved images

Press 's' or 'c' will save below files:
The difference between these two kinds saved images is one is annotated, the other is not.

```
color${date +%d}.png
depth${date +%d}.png
depth${date +%d}.bin
edged${date +%d}.png
```

*_*depth(\d+).bin*_* is the raw depth data from realsense camera.

depth*.png is colorized depth images which are aligned with RGB images.

## command line interface

```
./capture -p color1544751310.png
```

0123456789 is time stamping. Using **-p** option will extract the number part and compose coresponding files. No need to specify each of them.

For above example, it will use search for RGB image, colorized depth image, depth raw data to do unit test.

## setting.json

```
{
    "video_id": 0,
    "description": "will use /dev/video${video_id}",
    "default_width": 320,
    "default_height": 240,

    "use_realsense": true,
    "use_edge_test": true,
    "use_hsv_test": false,

    "output_dir": "outdir",
    "debug": false,

    "realsense": {
        "find_edge": true,
        "blur_radius": 3,
        "canny_threshold_min": 25,
        "canny_threshold_max": 50,
        "hough_threshold": 70,
        "hough_minlength": 130,
        "hough_maxlinegap": 45,
        "show_dist": false,
        "show_fps": true,
        "color_scheme": 2,
        "distance_limit": 0.0,
        "apply_align": true,
        "apply_disparity": true,
        "apply_dec": true,
        "apply_spatial": true,
        "apply_temporal": true,
        "apply_holefill": true,
        "max_degree": 60
    }
}

```
