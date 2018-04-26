#ifndef __SSD_DETECTOR_H__
#define __SSD_DETECTOR_H__

#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <utility>

#include <caffe/caffe.hpp>
#include <opencv2/opencv.hpp>

using std::string;
using std::vector;
using std::shared_ptr;
using caffe::Net;

class Detector {
 public:
  Detector(const string& model_file,
           const string& weights_file,
           const string& mean_file,
           const string& mean_value);

  ~Detector() {
    net_.reset();
  }

  std::vector<vector<float> > Detect(const cv::Mat& img);

 private:
  void SetMean(const string& mean_file, const string& mean_value);

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);

  void Preprocess(const cv::Mat& img,
                  std::vector<cv::Mat>* input_channels);

 private:
  shared_ptr<Net<float> > net_;
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
};

#endif  // __SSD_DETECTOR_H__
