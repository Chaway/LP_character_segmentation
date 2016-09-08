/*
  单排字符车牌分割
*/

#include "segment.h"

#define WIDTH_SI 1050 //车牌归一化处理宽度
#define HEIGHT_SI 300  //车牌归一化处理高度
#define THRESHOLD_SI 180  //二值化处理阈值
#define THRESHOLD_SI_INV 75 //二值化处理阈值
using namespace std;
using namespace cv;


/*
   图像预处理
*/
void preprocess_si(Mat& input_img)
{
    cvtColor(input_img,input_img,COLOR_BGR2GRAY);  //RGB图像转为灰度图像
    resize(input_img,input_img,Size(WIDTH_SI,HEIGHT_SI)); //线性插值大小归一化
    equalizeHist(input_img,input_img); //直方图均衡化处理
    //int color_type = judge_color(input_img);
    //threshold(input_img, input_img, color_type == BLUEWHITE ? THRESHOLD_SI :THRESHOLD_SI_INV,
    //                   255,color_type == BLUEWHITE ? CV_THRESH_BINARY : CV_THRESH_BINARY_INV);
    threshold(input_img,input_img,THRESHOLD_SI,255,CV_THRESH_BINARY);
    //imwrite("./thresh_si.jpg",input_img);
}


/*
  字符区域筛选
*/
void filter_si(Mat& image_co,Mat& input_img,vector< vector< Point> >& contours,vector<Rect>& proposal_ch,vector<Rect>& let_num,int& num_ch,int mor_times,int times)
{
    for (int i = 0;i < contours.size();i++)
    {
        Rect r = boundingRect(Mat(contours[i]));  //获取轮廓的bounding box（调用库函数）

        if(times != 0)
        {
           revise_re(r,mor_times,input_img);   //如果经过腐蚀则要修正矩形框位置
        }


        /*筛选中文字符的可能子区域*/
        if(times == 0)
        {
            if(r.x + r.width < WIDTH_SI/5 && r.width >= 10 && r.height >= 10)  //大小先验信息
            {
                if(r.x < 30)
                {
                    if((r.y + r.height) > HEIGHT_SI - 10 || r.y < 10)  //位置先验信息
                    {
         		           //case 1.1 & 1.2:left down & up
              	        if(r.height/r.width < 3 && numzero(contours[i],r) > 0.35)  //比例和面积比先验信息
                        {
                            proposal_ch.push_back(r);
                            rect_zero(image_co,r);
              	        }
                    }
                    else
                    {
                        	//case 1.3:left center
                        if(r.height/r.width < 3)
                        {
                            proposal_ch.push_back(r);
                            rect_zero(image_co,r);
                        }
                    }
                }
                else
                {
                    //case 1.4:proposal region
                	proposal_ch.push_back(r);
                    rect_zero(image_co,r);
                    continue; //防止与右边字符重复分割
                }
            }
        }
        /*筛选英文和数字字符区域*/
        if(r.x > 100 && WIDTH_SI/60 < r.width && r.width < WIDTH_SI/5 && r.height > HEIGHT_SI/3 && r.height < HEIGHT_SI - 5)
        {
            if((r.x + r.width) > WIDTH_SI - 30)
            {
         	    //case 2.1:right
                if(r.height/r.width < 8 && r.width > HEIGHT_SI/27 && numzero(contours[i],r) > 0.25)
                {
                    if(r.height/r.width < 3 || numzero(contours[i],r) < 0.8)
                    {
                        let_num.push_back(r);
                        draw_rect(r,input_img,image_co,LET_NUM);
                        //draw_rect_or(input_img,r);
                        num_ch++;
                    }
                }
            }
            else if(r.height/r.width < 8)
            {
         	    //case 2.2:normal
                let_num.push_back(r);
                draw_rect(r,input_img,image_co,LET_NUM);
                //draw_rect_or(input_img,r);
                num_ch++;
                //cout << num_ch << endl;
            }
        }
    }
}

/*
  合并中文字符子区域，得到完整中文字符的外接矩形框
*/
void find_ch_si(vector<Rect>& proposal_ch,int& num_ch,float (&b)[2],float (&k)[2],int& result_index)
{
    int size = proposal_ch.size();
    result_index = 0;

    int flag = 0;

    for(int i = 0 ; i < size ; i++ )
    {
       /*候选轮廓与两条字符区域边界的回归直线的位置比较*/
        if((proposal_ch[i].y  + proposal_ch[i].height - 1) <= (int)(k[0]*proposal_ch[i].x + b[0]))
            if((proposal_ch[i].y  + proposal_ch[i].height - 1) <= (int)(k[0]*(proposal_ch[i].x + proposal_ch[i].width - 1) + b[0]))
            {
                continue;
            }

        if((proposal_ch[i].y - 1) >= (int)(k[1]*proposal_ch[i].x + b[1]))
            if((proposal_ch[i].y - 1) >= (int)(k[1]*(proposal_ch[i].x + proposal_ch[i].width - 1) + b[1]))
            {
                continue;
            }

        flag ++ ;

        if(flag == 1)
        {
            result_index = i;
            continue;
        }

      	merge(proposal_ch[i],proposal_ch[result_index]); //合并后的结果储存在下标result_index的单元中
        result_index = i;
    }

    /*如果没有合并出汉字，则返回proposal_ch的size值*/
    if(flag == 0)
    {
   	    //cout << "can't find Chinese character!" << endl;
        result_index = size;
    }
    else
        num_ch ++;
}


/*
  单排字符图像分割
*/
int segment_single(Mat& input_img)
{
    preprocess_si(input_img);   //图像预处理
    Mat image_co = input_img.clone();  //复制图像数据
    cvtColor(input_img,input_img,COLOR_GRAY2BGR); //将预处理后的图像重新转换为RGB三通道（方便在图像上画出彩色的矩形框）

    vector< vector< Point> >  contours;  //存放检测到的轮廓
    Mat element = getStructuringElement(MORPH_CROSS,Size(ELE_WIDTH,ELE_HEIGHT)); //定义腐蚀模板
    vector<Rect> proposal_ch;  //存放汉字的候选子区域
    vector<Rect> let_num; //存放数字和英文字

    int flag_ch = 0; //进行过中文字符合并的标志
    int mor_times = 0; //腐蚀次数
    int num_ch = 0;  //找到的字符个数

    for(int times = 0;times < MAX_TIMES;times ++)
    {
        Mat temp_co = image_co.clone();  //下一步调用findContours时会对输入图像进行变换，为了保留image_co的数据
        findContours(temp_co,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); //寻找输入图像的轮廓，轮廓存放在contours中（调用库函数）

        filter_si(image_co,input_img,contours,proposal_ch,let_num,num_ch,mor_times,times); //筛选字符区域

        if(num_ch >= 4  && flag_ch == 0)
        {
            flag_ch = 1;
            int result_index;//记录中文字符bounding box的位置
            float b[2]={0}; //字符区域上下边界直线的截距
            float k[2]={0}; //字符区域上下边界直线的斜率

            linear_regression(let_num,b,k);  //最小二乘法拟合出字符区域上下边界
	        //cout << "(b,k) = " << b[0] <<" " << k[0] << "  " <<  b[1] <<" " << k[1] <<endl;
            setzero(image_co,b,k); //字符区域外清零

            find_ch_si(proposal_ch,num_ch,b,k,result_index); //得出中文字符的外接矩形框

            if(result_index != proposal_ch.size())
            {
                //rectangle(input_img,proposal_ch[result_index], Scalar(0,255,255),3);
                //draw_rect_or(input_img,proposal_ch[result_index]);
                draw_rect(proposal_ch[result_index],input_img,image_co,CH); //在图像上画出中文字符的边框
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
            morphologyEx(image_mor,image_co,MORPH_ERODE,element);   //对图像进行形态学腐蚀处理
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
