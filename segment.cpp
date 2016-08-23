/*
project description:
   License plate charecter segmentation based on OpenCV 3.1.0
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

    /*
		   判断输入车牌图像是单排字符图像还是双排字符图像，然后根据判断结果进行分割
		*/
    switch(type)
    {
    	case SINGLE:
    	      cout << "Single row character license plate" << endl;
    	      //cout << input_img.cols/(float)input_img.rows << endl;
    	      segment_single(input_img);  //进行单排字符车牌分割
    	      break;

    	case DOUBLE:
    	      cout << "Double row character license plate" << endl;
    	      //cout << input_img.cols/(float)input_img.rows << endl;
    	      segment_double(input_img);  //进行双排字符车牌分割
    	      break;

    	default:
    	      return -1;
    }

    /*将分割结果写入磁盘，路径可以自定义*/
    string output_name = argv[1];
    string dir = "./results/"; //输出路径
    output_name = dir  + output_name;
    imwrite(output_name,input_img);
		
    /*显示分割结果*/
    imshow("Input image", input_img);
    cout << "Press any key to exit.." << endl;
    waitKey();
    return 0;
}
