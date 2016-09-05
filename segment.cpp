/*
project description:
   License plate character segmentation based on OpenCV 3.1.0
   基于OpenCV 3.1.0的车牌字符分割，使用方法详见README.md
version 0.1.0

*/

#include "segment.h"
using namespace std;
using namespace cv;

int main(int argc,char ** argv)
{
    Mat input_img;
    input_img = imread(argv[1]);          //读取input image
    if(!input_img.data)
    {
    	cout << "Open input image failed,please check input image!" << endl;
    	exit(0);
    }

    cout << "width = " << input_img.cols << " , height = " << input_img.rows << endl;

    int type = judge_type(input_img);    //判断input image的type
    int num_si = 0 , num_do = 0;
    /*
		   判断输入车牌图像是单排字符图像还是双排字符图像，然后根据判断结果进行分割
		*/
    switch(type)
    {
    	case SINGLE:
    	      cout << "Single row character license plate" << endl;
    	      //cout << input_img.cols/(float)input_img.rows << endl;
    	      num_si = segment_single(input_img);  //进行单排字符车牌分割
              if(num_si != 7)
              {
                  cout << "Segmentation fail" << endl;
                  return -1;
              }
              cout << "Segmentation success" << endl;
    	      break;

    	case DOUBLE:
    	      cout << "Double row character license plate" << endl;
    	      num_do = segment_double(input_img);  //进行双排字符车牌分割
              if(num_do != 7)
              {
                  cout << "Segmentation fail" << endl;
                  return -1;
              }
    	      break;

        case UNCERTAIN:
              cout << "Uncertain by ratio" << endl;
              num_si = segment_single(input_img);
              if(num_si == 7)
              {
                  cout << "Single row character license plate" << endl;
                  cout << "Segmentation success" << endl;
              }
              else
              {
                  input_img = imread(argv[1]);
                  num_do = segment_double(input_img);
                  if(num_do == 7)
                  {
                      cout << "Double row character license plate" << endl;
                      cout << "Segmentation success" << endl;
                  }
                  else
                  {
                      cout << "Segmentation fail" << endl;
                      return -1;
                  }
              }
              break;

    	default:
    	      return -1;
    }

    /*将分割结果写入磁盘，路径可以自定义*/
    string input_name = argv[1];
    unsigned long pos = input_name.find_last_of("/");
    string filename = input_name.substr(pos+1); //截取文件名
    string dir = "./results/"; //输出路径
    string output_name = dir  + filename;
    imwrite(output_name,input_img);
		
    /*显示分割结果*/
//    imshow("Input image", input_img);
//    cout << "Press any key to exit.." << endl;
//    waitKey();
    return 0;
}
