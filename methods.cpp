/*
   单排和双排字符分割过程中共同使用的函数
*/
#include "segment.h"

using namespace std;
using namespace cv;

int judge_type(Mat image)
{
	if(image.cols/(float)image.rows > 1.8)
		return SINGLE;
	else
		return DOUBLE;
}


void revise_re(Rect& r,int mtime,Mat image)
{
    r.x = ((r.x - mtime*ELE_WIDTH) > 0) ? (r.x - mtime*ELE_WIDTH/2) : 1;
    r.y = ((r.y - mtime*ELE_HEIGHT) > 0) ? (r.y - mtime*ELE_HEIGHT/2) : 1;
    r.width = ((r.x + r.width + mtime*ELE_WIDTH - 1)  <= image.cols) ? (r.width + mtime*ELE_WIDTH) : (image.cols - r.x + 1);
    r.height = ((r.y + r.height + mtime*ELE_HEIGHT - 1) <= image.rows) ? (r.height + mtime*ELE_HEIGHT) : (image.rows - r.y + 1);
}

/*画出图像的灰度值直方图，以图像形式表示*/
void drawHist(Mat& src,string& s_temp)
{
    MatND hist;
    int dims = 1;
    int bins = 256;
    int hist_size[] = {bins};
    float range[] = { 0, 256 };
    const float* ranges[] = {range};

    int channels[] = {0};

    calcHist(&src, 1, channels, Mat(),hist,dims,hist_size,ranges); //调用库函数计算灰度直方图


    int hist_height=256;

    double max_val;
    double min_val;
    int scale = 2;
    minMaxLoc(hist, &min_val, &max_val, 0, 0);

    Mat hist_img(hist_height,bins*scale,CV_8U,Scalar(0));

    //cout<<"max_val = "<<max_val<<endl;

    for(int i=0;i<bins;i++)
    {
        float bin_val = hist.at<float>(i);
        int intensity = cvRound(bin_val*hist_height/max_val);

        rectangle(hist_img,Point(i*scale,hist_height-1),
            Point((i+1)*scale - 1, hist_height - intensity),
            Scalar(255));
    }

}


/*
void remove_rivet(Mat& image_thresh,int& upline,int& downline)
{
 //cout << image_thresh.rows << " >>>>>>>>>>>" <<endl;
 upline = 0;
 downline = image_thresh.rows - 1;
 int changetimes[image_thresh.rows]={0};
 int changetimes_thresh = 8;
 for(int i = 0;i < image_thresh.rows;i++)
     {
     	//cout << image_thresh. << endl;
         for(int j = 1;j < image_thresh.cols;j++)
        {
        	int temp = abs((int)image_thresh.at<uchar>(i,j) - (int)image_thresh.at<uchar>(i,j-1));
            // if(i ==1)
            // 	cout << temp << endl;
            if( temp == 255)
            {
               changetimes[i] ++;
            }
        }
     }


 for(int start = image_thresh.rows/2;start >=1;start --)
 {
 	if (changetimes[start] < changetimes_thresh)
 	{
 		upline = start;
 		cout << "upline = " << upline << endl;

 		break;
 	}
 }

 for(int start = image_thresh.rows/2;start < image_thresh.rows;start ++)
 {
 	if (changetimes[start] < changetimes_thresh)
 	{
 		downline = start;
 		cout << "downline = " << downline << endl;
 		break;
 	}
 }

 // for(int row = 0;row <= upline;row ++)
 // {
 // 	image_thresh.rows(row).setTo(Scalar(0));
 // }
 image_thresh(Range(0,upline),Range::all()).setTo(Scalar(0));

// for(int row = downline;row <= image_thresh.rows;row ++)
//  {
//  	Range(0,100),Range(50,200).setTo(Scalar(0));
//  }
 image_thresh(Range(downline,image_thresh.rows),Range::all()).setTo(Scalar(0));

}
*/

/*在原始输入图像上画出矩形框*/
void draw_rect_or(Mat& image_or,Rect& r,int COLS,int ROWS)
{
  Rect re_temp;
  re_temp.x = (int)((float)(r.x*image_or.cols)/COLS);
  re_temp.y = (int)((float)(r.y*image_or.rows)/ROWS);
  re_temp.width = (int)((float)(r.width*image_or.cols)/COLS);
  re_temp.height = (int)((float)(r.height*image_or.rows)/ROWS);
  rectangle(image_or,re_temp,Scalar(255,0,255),1);
}

/*不同类型的字符画出不同颜色的矩形框*/
void draw_rect(Rect& r,Mat& input_img,Mat& image_co,int character_class)
{
       switch(character_class){
                     case CH:
                          rectangle(input_img, r, Scalar(0,255,255),3);
                          break;
                     case LET_NUM:
                          rectangle(input_img, r, Scalar(255,0,255),3);
                          break;
                     default:
                          cout << "draw bounding box error" << endl;
                     };

      image_co(Range(r.y - 1,r.y + r.height - 1),Range(r.x - 1,r.x + r.width - 1)).setTo(Scalar(0));
     //对矩形框内的区域清零
 }

/*合并两个矩形框*/
void merge(Rect & subr1,Rect & subr2)//merge two rectangle
{

	int x1 = (subr1.x < subr2.x) ? subr1.x :subr2.x;
	int y1 = (subr1.y < subr2.y) ? subr1.y :subr2.y;
	int x2 = (subr1.x + subr1.width > subr2.x + subr2.width) ? subr1.x + subr1.width :subr2.x + subr2.width;
	int y2 = (subr1.y + subr1.height > subr2.y + subr2.height) ? subr1.y + subr1.height :subr2.y + subr2.height;
  subr1.x = x1;
  subr1.y = y1;
  subr1.width = x2 - x1;
  subr1.height = y2 - y1;
}


/*最小二乘拟合直线*/
void linear_regression(vector <Rect> & let_num,float (&a)[2],float (&b)[2])
{
   int size = let_num.size();
   int sum_xy[2]= {0};
   int sum_x_sq[2] = {0};
   int sum_x[2] = {0};
   int sum_y[2] = {0};
   float x_aver[2];
   float y_aver[2];
   //forlefttup points and leftdown points
   for(int i = 0;i < size;i++)
   {
   	 sum_xy[0] += (let_num[i].x )*(let_num[i].y);
   	 sum_xy[1] += (let_num[i].x + let_num[i].width - 1)*(let_num[i].y + let_num[i].height - 1);
   	 sum_x_sq[0] += (let_num[i].x)*(let_num[i].x);
   	 sum_x_sq[1] += (let_num[i].x  + let_num[i].width - 1)*(let_num[i].x  + let_num[i].width - 1);
   	 sum_x[0] += let_num[i].x;
   	 sum_x[1] += let_num[i].x + let_num[i].width - 1;
   	 sum_y[0] += let_num[i].y;
   	 sum_y[1] += let_num[i].y + let_num[i].height - 1;
   }

    x_aver[0] = sum_x[0]/(float)size;
    x_aver[1] = sum_x[1]/(float)size;
    y_aver[0] = sum_y[0]/(float)size;
    y_aver[1] = sum_y[1]/(float)size;

   b[0] = (sum_xy[0] - size*x_aver[0]*y_aver[0])/(sum_x_sq[0] - size*x_aver[0]*x_aver[0]);
   a[0] = y_aver[0] - b[0]*x_aver[0];

   b[1] = (sum_xy[1] - size*x_aver[1]*y_aver[1])/(sum_x_sq[1] - size*x_aver[1]*x_aver[1]);
   a[1] = y_aver[1] - b[1]*x_aver[1];


}


/*计算黑白面积比*/
float numzero(vector< Point> & contour,Rect & r)   //area of while
{
	//rectangle(imagedraw_rect_or_thresh, r, Scalar(0,255,255),2);
	// int num = 0;
 //    int numwhite = 0;
 //   	for(int row = r.y + 1  ;row < r.y + r.height - 2;row ++)
 //   	{
 //   		for (int col = r.x + 1  ;col < r.x + r.width - 2;col ++ )
 //   		{
 //   		  //cout << "row = " << row <<" col = " << col << endl;
 //   		  //cout << image_thresh.at<uchar>(row,col)/255
 //   		  if(image_thresh.at<Vec3b>(row,col)[0] == 0)
 //   		  num ++;
 //   	    }
 //   	}
   // cout << num  << endl;
   //	cout << numwhite << endl;
	int num = contourArea(contour); //调用库函数计算轮廓包围的额面积
    //cout << num/(float)(r.width*r.height) << endl;
   	return num/(float)(r.width*r.height);
}





/*边界两侧清零*/
void setzero(Mat& image_co,float (&b)[2],float (&k)[2])
{
  for(int cols = 0 ; cols < image_co.cols ; cols ++)
  {
        for(int rows = 0 ; rows <= (cols*k[0] + b[0]) ; rows ++)
        {
            image_co.at<uchar>(rows,cols) = 0;
        }

        for(int rows = (int)(cols*k[1] + b[1]) ; rows < image_co.rows ; rows ++)
        {
             image_co.at<uchar>(rows,cols) = 0;
        }
  }

}

/*gamma变换*/
void gamma(Mat & image_gray)
{
  int cor[256];
  float gamma = 1.5;
  float max = pow(255,gamma);
  for(int i = 0;i < 256;i++)
  {
    cor[i] = (int)(i*pow(i,gamma)/max);
  }
  for(int row = 0 ;row < image_gray.rows ;row++ )
    for(int col = 0;col < image_gray.cols; col++)
    {
      int intensity = image_gray.at<uchar>(row,col);
      image_gray.at<uchar>(row,col) = cor[intensity];
    }
}
