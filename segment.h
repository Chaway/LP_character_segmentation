#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <math.h>

#define SINGLE 1  //单排字符标志
#define DOUBLE 2  //双排字符车牌
#define UNCERTAIN 3 //无法从比例确定
#define MAX_TIMES 3  //查找字符最大次数
#define ELE_WIDTH 10  //腐蚀模板的宽度
#define ELE_HEIGHT 10  //腐蚀模板的高度
#define CH 1 //中文字符标志
#define LET_NUM 2  //数字字符或英文字符标志





int judge_type(cv::Mat);  //判断输入车牌类型

int segment_single(cv::Mat&);  //单排字符车牌分割函数
//void find_ch_si(std::vector<cv::Rect> & proposal_ch,int & num_ch,float (&b)[2],float (&k)[2],int & result_index); //merge boxes of proposal
int segment_double(cv::Mat&);  //双排字符车牌分割函数


//common methods
void revise_re(cv::Rect& r,int mor_times,cv::Mat); //修正矩形框的边缘（经过腐蚀之后检测出的轮廓与实际轮廓有误差）
//void remove_rivet(cv::Mat & image_thresh,int & upline,int  & downline);

//void draw_rect_or(cv::Mat & image_or,cv::Rect &r); //在原车牌图像中画出检测出字符的外接矩形框
void draw_rect(cv::Rect & r,cv::Mat& input_img,cv::Mat& image_co,int numclass); //在二值图像上画出字符的外接矩形框
void merge(cv::Rect & subr1,cv::Rect & subr2); //合并两个矩形框
void rect_zero(cv::Mat& img,cv::Rect r);//矩形区域清零

void linear_regression(std::vector<cv::Rect> & let_num,float (&a)[2],float (&b)[2]); //最小二乘拟合直线
float numzero(std::vector<cv::Point> & contour,cv::Rect & r);   //计算矩形区域中白色像素的比例

void setzero(cv::Mat & image_co,float (&b)[2],float (&k)[2]);  //选定区域像素灰度值清零


void gamma(cv::Mat & image_gray); //图像gamma变换
//void drawHist(cv::Mat &src,std::string& s_temp);//画出灰度直方图
