/*
    refer from https://stackoverflow.com/questions/30764222/how-to-read-ply-file-using-pcl
*/

#include <iostream>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>

#define PLY_FILE  "../1.ply"


void test()
{
    using namespace std;
    using namespace pcl;

    const string fname = PLY_FILE;
    PointCloud<pcl::PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
    PLYReader Reader;
    int ret = Reader.read(fname, *cloud);
    cout << "ret: " << ret << endl;

    std::cout << "Loaded "
            << cloud->width * cloud->height
            << " data points from " << fname
            << " with the following fields: "
            << std::endl;
    const size_t upper_bound = 10;  // cloud->points.size()
    for (size_t i = 0; i < upper_bound; ++i) {
        std::cout << "    " << cloud->points[i].x
              << " "    << cloud->points[i].y
              << " "    << cloud->points[i].z << std::endl;
    }

}

int main()
{
    test();
    return 0;
}

#if 0
#include <pcl/io/pcd_io.h>
int main (int argc, char** argv)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);

  if (pcl::io::loadPCDFile<pcl::PointXYZ> ("airplane.pcd", *cloud) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file test_pcd.pcd \n");
    return (-1);
  }
  std::cout << "Loaded "
            << cloud->width * cloud->height
            << " data points from test_pcd.pcd with the following fields: "
            << std::endl;
  for (size_t i = 0; i < cloud->points.size (); ++i)
    std::cout << "    " << cloud->points[i].x
              << " "    << cloud->points[i].y
              << " "    << cloud->points[i].z << std::endl;

  return (0);
}
#endif
