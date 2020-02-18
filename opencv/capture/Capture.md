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
        "apply_sleep": false,
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

## note

Collecting data task from 2019-11-22.

updated date: 2019-12-04

### collection

1. There is a task that using 'capture' to capture rgb/depth
   from person located at 600/800/1000 mm distance.
2. Using 'realsense-viewer' to record a 10-second bag file.
3. Using iphone to take a selfie.
4. Using 'capture' to capture rgb/depth with these two conditions:
    a. sync pictures, put ipad at 300/400/500mm
    b. print selfie pictures to paper
        - put flat pictures at 600/800/1000mm (fingers pinch at two upper corners of paper, and behind the paper as possible, MUST NOT show any part of face)
        - cover paper on face with slightly roll (fingers pinch at two lower corners of paper, and behnd the paper as possble, MUST NOT show any part of face)

collected images are put at ```$HOME/Pictures/giveOut```

### post processing

```
rsync -av --exclude='*.bag' ./collect/ $HOME/Pictures/giveOut
```

Do not copy ```*.bag``` to output folder. Still images are
extracted from bag files and composed into mov video at
```$HOME/Videos/bag_to_png_to_mov```


```bash
rs-convert -c -p  png -i 20191118_132235.bag --

ffmpeg -framerate 8 -pattern_type glob -i 'png_Color_*.png' -c:v mjpeg -q:v 1 ./out.mov
```

related folders:
/home/rasmus/src/ccbox/opencv/capture/collect
/home/rasmus/Pictures/giveOut/20191203/
new: rmbg\d{10}.png
/home/rasmus/Videos/bag_to_png_to_mov/20191203-work

