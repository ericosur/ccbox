# readme for resize

This simple test application will use resize function from opencv.
It will not generate specified files or results.

## link with vanilla or intel opencv

At CMakeLists.txt, modify APPLY_INTEL_OPENCV to 1, will link
intel optimized opencv library. (Of course need Intel CV SDK installed)

## benchmark

```
$ time ./resize
```

to know the time takes

