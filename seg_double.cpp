/*
  双排字符车牌分割
*/
#include "segment.h"

#define WIDTH_DO 450 //车牌归一化处理宽度
#define HEIGHT_DO 300  //车牌归一化处理高度
#define THRESHOLD_DO 75 //二值化处理阈值

using namespace std;
using namespace cv;


void preprocess_do(Mat &input_img)
{
    cvtColor(input_img,input_img,COLOR_BGR2GRAY); //RGB图像转为灰度图像
    resize(input_img,input_img,Size(WIDTH_DO,HEIGHT_DO)); //线性插值大小归一化
    equalizeHist(input_img,input_img);  //直方图均衡化处理
    threshold(input_img,input_img,THRESHOLD_DO,255,CV_THRESH_BINARY_INV); //阈值化处理,与单排车牌黑白相反
    //imwrite("./thresh_do.jpg",input_img);
}

void filter_do(Mat& image_co,Mat& input_img,vector< vector< Point> >& contours,vector<Rect>& proposal_ch,vector<Rect>& let_num,int& num_ch,int mor_times,int times)
{
    for (int i = 0;i < contours.size();i++)
    {
        Rect r = boundingRect(Mat(contours[i]));  //获取轮廓的bounding box
        if(times != 0)
        {
           revise_re(r,mor_times,input_img);
        }

        /*
             根据位置分为三个区域进行筛选（1.第一排的中文字符区域，
         2.第一排的英文字符区域，3.第二排的字符区域），去除干扰区
         域的筛选条件与单排字符分割相似，主要用到了位置信息，大小
         信息，比例信息以及黑白面积比信息。
        */
        if(times == 0)
        {
            if(r.y + r.height/2 < HEIGHT_DO/2 && r.x + r.width/2 < WIDTH_DO/2 - 30)
            {
                if(r.x + r.width < WIDTH_DO/2 && r.width >= 10 && r.height >= 10)//get Chinese Character
                {
                    if(r.x > 50)
               	    {
                  	    //case 1:part of Chinese character
                  	    proposal_ch.push_back(r);
                        rect_zero(image_co,r);
                    }
                }
            }
        }

        if(r.y + r.height/2 < HEIGHT_DO/2 && r.x + r.width/2 > WIDTH_DO/2 && r.width < WIDTH_DO/2 && r.height < HEIGHT_DO/2)
        {
            //case 2:Letter in up line
            //第一排的英文字符情况比较简单，无需存入let_num
            if(r.width > 80 && r.height > 80 && r.height/r.width < 3)
            {
                draw_rect(r,input_img,image_co,LET_NUM); //画出英文或数字字符的边框
                //draw_rect_or(image_or,r);
                num_ch++;
            }
        }
        else if(r.y + r.height/2 > HEIGHT_DO/2 && WIDTH_DO/60 < r.width && r.width < WIDTH_DO/4.0 && r.height > HEIGHT_DO/3 && r.height < HEIGHT_DO - 100)
        {
            if((r.x + r.width) > WIDTH_DO - 30 || r.x < 30)
            {
                //case 3.1&3.2:left & right side letter or number character
                if(r.height/r.width < 8 && r.width > WIDTH_DO/27 && numzero(contours[i],r) > 0.25 )
                {
                    if(r.height/r.width < 3 || numzero(contours[i],r) < 0.8)
                    {
                        let_num.push_back(r);
                        draw_rect(r,input_img,image_co,LET_NUM);
                        //draw_rect_or(image_or,r);
                        num_ch++;
                    }
                }
            }
            else if(r.height/(float)(r.width) < 8.0)
            {
                    //case 3.3:down center letter or number character
                let_num.push_back(r);
                draw_rect(r,input_img,image_co,LET_NUM);
                //draw_rect_or(image_or,r);
                num_ch++;
                //cout << num_ch << endl;
            }
        }
    }
}

void find_ch_do(vector<Rect>& proposal_ch,int& num_ch,float (&b)[2],float (&k)[2],int& result_index)
{
    int size = proposal_ch.size();
    result_index = 0;
    int flag = 0;

    for(int i = 0 ; i < size ; i++ )
    {    /*候选轮廓与回归直线的位置比较*/
        if((proposal_ch[i].y  + proposal_ch[i].height - 1) >= (int)(k[0]*proposal_ch[i].x + b[0]))
            if((proposal_ch[i].y  + proposal_ch[i].height - 1) >= (int)(k[0]*(proposal_ch[i].x + proposal_ch[i].width - 1) + b[0]))
            {
                continue;
            }

        flag ++ ;

        if(flag == 1)
        {
            result_index = i;
            continue;
        }

      	merge(proposal_ch[i],proposal_ch[result_index]);  //合并后的结果储存在下标result_index的单元中
        result_index = i;
    }
    /*如果没有合并出汉字，则返回proposal_ch的size值*/
    if(flag == 0)
    {
   	    //cout << "Can't find Chinese character!" << endl;
   	    result_index = size;
    }
    num_ch ++;
}

/*
  双排字符图像分割，与单排分割的主要区别在于filter_do筛选方法，总的分割流程相同
*/
int segment_double(Mat& input_img)
{
    preprocess_do(input_img); //图像预处理

    Mat image_co = input_img.clone();
    cvtColor(input_img,input_img,COLOR_GRAY2BGR);//将预处理后的图像重新转换为RGB三通道（方便在图像上画出彩色的矩形框）

    vector< vector< Point> >  contours;//存放检测到的轮廓
    Mat element = getStructuringElement(MORPH_CROSS,Size(ELE_WIDTH,ELE_HEIGHT));//定义腐蚀模板
    vector<Rect> proposal_ch;//存放汉字的候选子区域
    vector<Rect> let_num; //存放第二排的数字和英文字符

    int flag_ch = 0;//进行过中文字符合并的标志
    int num_ch = 0;//找到的字符个数
    int mor_times = 0; //腐蚀次数
    
    for(int times = 0 ; times < MAX_TIMES ; times ++)
    {
        Mat temp_co = image_co.clone(); //下一步调用findContours时会对输入图像进行变换，为了保留image_co的数据
        findContours(temp_co,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); //寻找输入图像的轮廓，轮廓存放在contours中（调用库函数）


        filter_do(image_co,input_img,contours,proposal_ch,let_num,num_ch,mor_times,times);//筛选字符区域

        if(num_ch >= 3 && flag_ch == 0)
        {
            flag_ch = 1;
            int result_index;//记录中文字符bounding box的位置
            float b[2]={0};//字符区域上下边界直线的截距
            float k[2]={0};//字符区域上下边界直线的斜率
            
            linear_regression(let_num,b,k);//最小二乘法拟合出第二排字符区域上下边界
            
            find_ch_do(proposal_ch,num_ch,b,k,result_index); //得出中文字符的外接矩形框

            if(result_index != proposal_ch.size())
            {
                draw_rect(proposal_ch[result_index],input_img,image_co,CH);//在图像上画出中文字符的边框
                //draw_rect_or(image_or,proposal_ch[result_index]);
            }
            else
            {
                 cout << "Can't find Chinese character!" << endl;
                 return num_ch;
            }
        }
        else
        {
            Mat image_mor = image_co.clone();
            morphologyEx(image_mor,image_co,MORPH_ERODE,element);//对图像进行形态学腐蚀处理
            mor_times ++;
        }

        if(num_ch == 7)
        {
            //cout << "times = " << times << endl;
            break;
        }
    }
    return num_ch;
}
