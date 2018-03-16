# readme for setting.json

explain each variable setting function/effect

## variables

| key | value | description |
| --- | --- | --- |
| model_file | "/path/to/model_file" | specify full path for model file
| weights_file | "/path/to/weight_file" | specify full path for weight file |
| file_type | "webcam" or "realsense" | all new features are implemented into "realsense" part |
| confidence_threshold | 0.3 | NOT RECOMMEND to change |
| label_id | 15 | just for back-compability |
| label_name | "person" | just for back-compability |
| direct_use_realsense | 1 or 0 | if 0, ssd_detect will not open realsense camera to capture |
| wait_msgq | 1 or 0 | if 1, will use msgq to receive filename to process |
| wait_myipc | 1 or 0 | if 1, will use MY_IPC to receive image index, for target that installed MY_IPC kernel module OR ssd_detect crashes |
| wait_myipc_tag | "photo_index" | DO NOT change, need follow PM config |
| dog_warning_tag | "dog_warning" | DO NOT change need follow PM config |
| audience_vol_tag | "audience_vol" | DO NOT change need follow PM config |
| do_imshow | 1 or 0 | will use opencv imshow to show result at GUI |
| show_fps | 1 or 0 | will show fps info at GUI, need do_imshow is 1 |
| testraw | 1 or 0 | if wait_msgq is 1, testraw is 1, will take msgq command as index to fetch images |
| rawbin_dir | "/tmp" | for target integration, open image/depth raw bin from this path |
| test_crop | 1 or 0 | crop for small object detection test, such function is integrated, will not change behavior |
| show_debug | 1 or 0 | use 1 will see lots of debug message at CLI |
| max_crop_try | 3 | integer value to tell ssd_detect need to try how many times to detect small objects |

**model_file**, **weights_file** will be various at deployed system

## suggest values for different cases

### standalone case

show captured images, detection result and fps, use **"direct_use_realsense": 1**

### fully-integrated case
**"direct_use_realsense": 0**
**"wait_myipc": 1**

