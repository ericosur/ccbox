# readme of openpose output

## commands

```
TOP=/src/github/openpose/

${TOP}/build/examples/openpose/openpose.bin
        --image_dir="../rs_data/" \
        --model_pose="COCO"       \
        --write_json="output_json" \
        --write_images="output_json"
```

* image_dir: relative dir path of source images
* model_pose: using COCO
* write_json: relative dir to output json results
* write_images: relative dir to output result images

## COCO outputs

https://github.com/CMU-Perceptual-Computing-Lab/openpose/blob/master/doc/output.md#pose-output-format-coco

## NOTE

need sync to $HOME/src/github/openpose/output_json

```
$ bcompare $HOME/src/ccbox/rs/rs_openpose/output_json \
    $HOME/src/github/openpose/output_json &
```
