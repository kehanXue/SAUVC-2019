#include "missionugate.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>

void visionClass::missionUgate()
{

    data.m3_runningState = true;
    qDebug()<<"missionUgate state"<<data.m3_runningState<<endl;
	 
	int m3_templcount = 0;
	Mat m3_imgtempl;
    Mat m3_imgsrc;

	m3_COLORTHRESH colorThresh; //用于颜色识别的结构体
    colorThresh.RB_max = 255;
    colorThresh.RB_min = 0;
    colorThresh.RH_max = 87;
    colorThresh.RH_min = 0;
    colorThresh.RR_max = 255;
    colorThresh.RR_min = 34;//40
	 
	int blur_size = 1;
    int m3_ugate_x=999;
    int cnt_findgate=0;
    int cnt_losegate=0;

	emit createWindow("imgROI");
	emit createWindow("H_ranged");
	emit createWindow("B_ranged");
	emit createWindow("R_ranged");
	emit createWindow("blur");
    emit createBar("RB_max", "B_ranged", &colorThresh.RB_max, 255);
    emit createBar("RB_min", "B_ranged", &colorThresh.RB_min, 255);
    emit createBar("RH_max", "H_ranged", &colorThresh.RH_max, 180);
    emit createBar("RH_min", "H_ranged", &colorThresh.RH_min, 180);
    emit createBar("RR_max", "R_ranged", &colorThresh.RR_max, 255);
    emit createBar("RR_min", "R_ranged", &colorThresh.RR_min, 255);
	//createTrackbar("blur_size", "blur", &blur_size, 10);

	vector<Mat> m3_bgr_channels,		//BGR双通道
		m3_hsv_channels;				//HSV双通道

										//任务变量
	Mat m3_imgclone,
		m3_imgclone2,
		m3_imgblur,					//中值滤波图像
		m3_imgresult,				//模板匹配后图像
		m3_imghsv,					//HSV颜色空间
		m3_imgranged,				//双边阈值颜色识别之后的图像
		m3_imgcanny,				//边缘识别之后的图像
		m3_imgROI;


	int m3_pixelsum = 0;				//颜色识别后像素点计数
	static const float m3_imgscale = 0.5;			//图像缩放比例
	static const int m3_saveinterval = 3;			//图像保存周期
	static const int m3_pixelThresh = 500 * m3_imgscale*m3_imgscale;	//颜色识别后像素点下阈值
	static const int m3_vecflare_dis= 100 * m3_imgscale;


	vector<Mat> vec_templ_h;
	vector<Mat> vec_templ_v;
	int templcount_h = 0,
		templcount_v = 0;
	char count[10];
	
	m3_Locate m3_locate;

	m3_locate.templ_h_center = 999;
	m3_locate.templ_v1 = 999;
	m3_locate.templ_v2 = 999;
	m3_locate.color_h_center = 999;
	m3_locate.color_v1 = 999;
	m3_locate.color_v2 = 999;

	memset(count, 0, sizeof(count));
	while (1)
	{
		char img_name[15] = "m3_h";
		templcount_h++;
        sprintf(count, "%d", templcount_h);
        strcat(count, ".jpg");
        strcat(img_name, count);
		Mat img_templ_h = imread(img_name);
		if (img_templ_h.data == 0)
		{
			templcount_h--;
			break;
		}
		else
		{
			resize(img_templ_h, img_templ_h, Size(), m3_imgscale, m3_imgscale);
			vec_templ_h.push_back(img_templ_h);
		}
	}

	qDebug() << "h num"<<templcount_h << endl;

	memset(count, 0, sizeof(count));
	while (1)
	{
		char img_name[15] = "m3_v";
		templcount_v++;
        sprintf(count, "%d", templcount_v);
        strcat(count, ".jpg");
        strcat(img_name, count);
		Mat img_templ_v = imread(img_name);
		if (img_templ_v.data == 0)
		{
			templcount_v--;
			break;
		}
		else
		{
			resize(img_templ_v, img_templ_v, Size(), m3_imgscale, m3_imgscale);
			vec_templ_v.push_back(img_templ_v);
		}
	}

	qDebug() << "v num" << templcount_v << endl;

    //开始采集
    visionClass::start(Cam_Front);
    ugatesetting();
    int expotime_u=0;


	while (m3_start)
	{
        expotime_u++;
        if(expotime_u<10)
            ugatesetting();
		m3_iteration++;
		
        visionClass::frame2Mat(Cam_Front, m3_imgsrc);
	 
		if (m3_iteration % m3_saveinterval == 0)
		{
		    char imgnamesave[15] = "save3_";
            char countsave[10];
			sprintf( countsave, "%d", m3_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m3_imgsrc);
		}
		
														
		//用于矩形拟合的结构体
		m3_RectJudge m3_rectBoundary_out;

		vector<Mat> m3_TemplVec;        //模板组
		int m3_templcount = 0;          //模板组计数
		vector<Mat> m3_Temp2Vec;        //模板组
		int m3_temp2count = 0;          //模板组计数



		static const int m3_templfindballcountThresh = 10;     //模板匹配累计有效帧数上限阈值

		int m3_heightscaled = 0,            //缩放后，源图像的高
			m3_widthscaled = 0;                 //缩放后，源图像的宽

		int m3_templfindballcount = 0,     //模板匹配累计有效帧数
			m3_colorfindballcount = 0,         //颜色识别累计有效帧数
			m3_areaupcount = 0;                 //矩形拟合面积超过上阈值的帧数


		m3_MATCH m3_MatchResult_h;
		m3_MatchResult_h.valid = false;
		m3_MATCH m3_MatchResult_v;
		m3_MatchResult_v.valid = false;
		m3_heightscaled = m3_imgsrc.rows * m3_imgscale;
		m3_widthscaled = m3_imgsrc.cols * m3_imgscale;

		//图像缩放
		resize(m3_imgsrc, m3_imgsrc, cv::Size(), m3_imgscale, m3_imgscale);
		m3_imgclone = m3_imgsrc.clone();
		/*split(m3_imgsrc, m3_bgr_channels);
		normalize(m3_bgr_channels.at(0), m3_bgr_channels.at(0), 0, 255, NORM_MINMAX);
		normalize(m3_bgr_channels.at(1), m3_bgr_channels.at(1), 0, 255, NORM_MINMAX);
		normalize(m3_bgr_channels.at(2), m3_bgr_channels.at(2), 0, 255, NORM_MINMAX);
		merge(m3_bgr_channels, m3_imgsrc);*/

		m3_colorRecognize(m3_imgsrc, m3_imgranged, colorThresh);
		//medianBlur(m3_imgranged, m3_imgranged,2*blur_size+1);
		/*Mat element = getStructuringElement(MORPH_RECT,
			Size(2 * blur_size + 1, 2 * blur_size + 1),
			Point(blur_size, blur_size));
		dilate(m3_imgranged, m3_imgranged, element);
		imshow("blur", m3_imgranged);*/
		m3_pixelsum = countNonZero(m3_imgranged);
		if (m3_pixelsum >= m3_pixelThresh)
		{
#pragma omp parallel sections
			{
#pragma omp section
				{
					m3_templatematch_vector(m3_imgsrc, m3_imgclone, vec_templ_h, m3_MatchResult_h);
					if (m3_MatchResult_h.valid == 1)
						m3_locate.templ_h_center= (0.5*m3_MatchResult_h.br_x + 0.5*m3_MatchResult_h.tl_x) ;
					else
						m3_locate.templ_h_center= 999;
				}
#pragma omp section
				{
					m3_templatematch_vector(m3_imgsrc, m3_imgclone, vec_templ_v, m3_MatchResult_v);
					if (m3_MatchResult_v.valid == 1)
						m3_locate.templ_v1= (0.5*m3_MatchResult_v.br_x + 0.5*m3_MatchResult_v.tl_x) ;
					else
                    {
                        m3_locate.templ_v1=999;
						m3_locate.templ_v2 = 999;
                    }

				}
			}
            /*char m3_confidence_h[10];
			sprintf(m3_confidence_h, "%f", m3_MatchResult_h.confidence);
            putText(m3_imgclone, m3_confidence_h, Point(20,100 ), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);

			char m3_confidence_v[10];
			sprintf(m3_confidence_v, "%f", m3_MatchResult_v.confidence);
            putText(m3_imgclone, m3_confidence_v, Point(20,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);*/
			

			if (m3_MatchResult_v.valid == 1)
			{
				m3_imgclone2 = m3_imgsrc.clone();
				for (int i = m3_MatchResult_v.tl_x; i < m3_MatchResult_v.br_x; i++)
				{
					for (int j = m3_MatchResult_v.tl_y; j < m3_MatchResult_v.br_y; j++)
					{
						m3_imgclone2.at<Vec3b>(Point(i, j))[0] = 255;
						m3_imgclone2.at<Vec3b>(Point(i, j))[1] = 255;
						m3_imgclone2.at<Vec3b>(Point(i, j))[2] = 255;
					}
				}
				//imshow("clone2", m3_imgclone2);
				m3_templatematch_vector(m3_imgclone2, m3_imgclone, vec_templ_v, m3_MatchResult_v);
				if (m3_MatchResult_v.valid == 1)
					m3_locate.templ_v2 = (0.5*m3_MatchResult_v.br_x + 0.5*m3_MatchResult_v.tl_x) ;
				else
					m3_locate.templ_v2 = 999;
                /*sprintf(m3_confidence_v, "%f", m3_MatchResult_v.confidence);
                putText(m3_imgclone, m3_confidence_v, Point(20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 255), 1, 8);*/
			}
			
			//进行canny边缘检测，提高矩形拟合的准确度
			Canny(m3_imgranged, m3_imgcanny, 20, 50);
			m3_rectBoundary(m3_imgcanny, m3_imgclone, m3_rectBoundary_out,m3_imgscale);
            if (m3_rectBoundary_out.hor_Rect_num>0 || m3_rectBoundary_out.vec_Rect_num>0)
            {
                if (m3_rectBoundary_out.hor_Rect_num > 0)
                {
                    m3_locate.color_h_center = 0.5*m3_rectBoundary_out.hor[0].br().x + 0.5*m3_rectBoundary_out.hor[0].tl().x;
                    rectangle(m3_imgclone, m3_rectBoundary_out.hor[0].tl(), m3_rectBoundary_out.hor[0].br(), Scalar(0, 255, 255), 2, 8, 0);
                    qDebug()<<"hor"<<m3_rectBoundary_out.hor[0].area()<<endl;


                }
                else
                    m3_locate.color_h_center = 999;
                if (m3_rectBoundary_out.vec_Rect_num == 0)
                    m3_locate.color_v1 = 999;
                if (m3_rectBoundary_out.vec_Rect_num == 1)
                {
                    m3_locate.color_v1 = 0.5*m3_rectBoundary_out.vec[0].br().x + 0.5*m3_rectBoundary_out.vec[0].tl().x;
                    rectangle(m3_imgclone, m3_rectBoundary_out.vec[0].tl(), m3_rectBoundary_out.vec[0].br(), Scalar(0, 0, 255), 2, 8, 0);
                    qDebug()<<"vec"<<m3_rectBoundary_out.vec[0].area()<<endl;
                    m3_locate.color_v2 = 999;
                }
                if (m3_rectBoundary_out.vec_Rect_num == 2)
                {
                    m3_locate.color_v1 = 0.5*m3_rectBoundary_out.vec[0].br().x + 0.5*m3_rectBoundary_out.vec[0].tl().x;
                    rectangle(m3_imgclone, m3_rectBoundary_out.vec[0].tl(), m3_rectBoundary_out.vec[0].br(), Scalar(0, 0, 255), 2, 8, 0);
                    qDebug()<<"vec"<<m3_rectBoundary_out.vec[0].area()<<endl;
                    m3_locate.color_v2 = 0.5*m3_rectBoundary_out.vec[1].br().x + 0.5*m3_rectBoundary_out.vec[1].tl().x;
                    if (abs(m3_locate.color_v1 - m3_locate.color_v2) < m3_vecflare_dis)
                        m3_locate.color_v2 = 999;
                    else
                        rectangle(m3_imgclone, m3_rectBoundary_out.vec[1].tl(), m3_rectBoundary_out.vec[1].br(), Scalar(0, 0, 100), 2, 8, 0);
                        qDebug()<<"vec_1"<<m3_rectBoundary_out.vec[1].area()<<endl;
                }
                if (m3_rectBoundary_out.vec_Rect_num == 3)
                {
                    m3_locate.color_v1 = 0.5*m3_rectBoundary_out.vec[0].br().x + 0.5*m3_rectBoundary_out.vec[0].tl().x;
                    rectangle(m3_imgclone, m3_rectBoundary_out.vec[0].tl(), m3_rectBoundary_out.vec[0].br(), Scalar(0, 0, 255), 2, 8, 0);
                     qDebug()<<"vec_0"<<m3_rectBoundary_out.vec[0].area()<<endl;
                    m3_locate.color_v2 = 0.5*m3_rectBoundary_out.vec[1].br().x + 0.5*m3_rectBoundary_out.vec[1].tl().x;
                    if (abs(m3_locate.color_v1 - m3_locate.color_v2) < m3_vecflare_dis)
                    {
                        m3_locate.color_v2 = 0.5*m3_rectBoundary_out.vec[2].br().x + 0.5*m3_rectBoundary_out.vec[2].tl().x;
                        if (abs(m3_locate.color_v1 - m3_locate.color_v2) < m3_vecflare_dis)
                            m3_locate.color_v2 = 999;
                        else
                            rectangle(m3_imgclone, m3_rectBoundary_out.vec[2].tl(), m3_rectBoundary_out.vec[2].br(), Scalar(0, 0, 100), 2, 8, 0);
                            qDebug()<<"vec_2"<<m3_rectBoundary_out.vec[2].area()<<endl;
                    }
                    else
                        rectangle(m3_imgclone, m3_rectBoundary_out.vec[1].tl(), m3_rectBoundary_out.vec[1].br(), Scalar(0, 0, 100), 2, 8, 0);
                        qDebug()<<"vec_1"<<m3_rectBoundary_out.vec[1].area()<<endl;
                }
            }
            else
            {
                m3_locate.color_h_center = 999;
                m3_locate.color_v1 = 999;
                m3_locate.color_v2 = 999;

            }




			if (m3_locate.templ_h_center != 999)
                m3_ugate_x= m3_locate.templ_h_center ;
			else if (m3_locate.color_h_center != 999)
                m3_ugate_x= m3_locate.color_h_center ;
			else if (m3_locate.templ_v2 != 999)
                m3_ugate_x = (0.5*m3_locate.templ_v1 + 0.5*m3_locate.templ_v2) ;
			else if (m3_locate.color_v2 != 999)
                m3_ugate_x = (0.5*m3_locate.color_v1 + 0.5*m3_locate.color_v2) ;
			else if (m3_locate.color_v1 != 999 && m3_locate.templ_v1 != 999 && abs(m3_locate.color_v1 - m3_locate.templ_v1) > m3_vecflare_dis)
                m3_ugate_x = (0.5*m3_locate.color_v1 + 0.5*m3_locate.templ_v1);
			else
                m3_ugate_x = 999 ;
			
		}

        if (m3_ugate_x !=999)
        {
            circle(m3_imgclone,{m3_ugate_x,0.5*m3_imgsrc.rows},1,Scalar(255,0,0),2);
            data.m3_ugate_dx=(0.5*m3_imgsrc.cols-m3_ugate_x)/ m3_imgscale;
            cnt_findgate++;
            cnt_losegate=0;

        }
        else
        {
            data.m3_ugate_dx=999;
            if(cnt_findgate>15)
               {
                cnt_losegate++;

                }
        }

        if (cnt_losegate>10)
            data.m3_ugate_dx=-999;

         qDebug()<<"m3_iteration:" << m3_iteration<<endl;
         qDebug()<<"cnt_findgate:"<<cnt_findgate<<endl;
         qDebug()<<"cnt_losegate:"<<cnt_losegate<<endl;
         qDebug()<<"m3_ugate_dx:"<<data.m3_ugate_dx<<endl;
         qDebug()<<"m3_locate.color_v1:"<<m3_locate.color_v1<<endl;
         qDebug()<<"m3_locate.color_v2:"<<m3_locate.color_v2<<endl;
         qDebug()<<"m3_locate.templ_v1:"<<m3_locate.templ_v1<<endl;
         qDebug()<<"m3_locate.templ_v2:"<<m3_locate.templ_v2<<endl;
         qDebug()<<"m3_locate.color_h_center:"<<m3_locate.color_h_center<<endl;
         qDebug()<<"m3_locate.templ_h_center:"<<m3_locate.templ_h_center<<endl;


        char ugate[10];
        sprintf(ugate, "%d", data.m3_ugate_dx);
        putText(m3_imgclone, ugate, Point(20,20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1, 8);


        if (m3_iteration % m3_saveinterval == 0)
        {
            char imgnamesave[15] = "save_m3_";
            char countsave[10];
            sprintf( countsave, "%d", m3_iteration);
            strcat(imgnamesave, countsave);
            strcat(imgnamesave, ".jpg");
            imwrite(imgnamesave, m3_imgclone);
        }


        emit imageShow(&m3_imgclone,"img");

        timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
    }
    //相机停止采集
    visionClass::stop(Cam_Front);

    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m1_runningState清0,便于主控查询
    data.m3_runningState = false;
    qDebug() << "m3_runningState : " << data.m1_runningState << endl;

    return;

}

Mat m3_img_rgb,
m3_img_hsv,
m3_img_temp,
m3_img_ranged;

vector<Mat> m3_channels_RGB,
m3_channels_HSV;

//双通道双边阈值颜色识别
void visionClass::m3_colorRecognize(const Mat &img_input, Mat &img_output, m3_COLORTHRESH &ColorThresh)
{
	m3_img_rgb = img_input.clone();
	cvtColor(img_input, m3_img_hsv, COLOR_BGR2HSV);
	split(m3_img_rgb, m3_channels_RGB);
	split(m3_img_hsv, m3_channels_HSV);

	/*imshow("H", m3_channels_HSV.at(0));
	imshow("S", m3_channels_HSV.at(1));
	imshow("V", m3_channels_HSV.at(2));
	imshow("B", m3_channels_RGB.at(0));
	imshow("G", m3_channels_RGB.at(1));
	imshow("R", m3_channels_RGB.at(2));*/
	
	//鉴于H通道是个环形的，需要考虑转一圈的情形
	if (ColorThresh.RH_max >= ColorThresh.RH_min)
	{
		inRange(m3_channels_HSV.at(0), Scalar(ColorThresh.RH_min, 0, 0), Scalar(ColorThresh.RH_max, 0, 0), m3_channels_HSV.at(0));
	}
	else
	{
		inRange(m3_channels_HSV.at(0), Scalar(ColorThresh.RH_min, 0, 0), Scalar(180, 0, 0), m3_img_temp);
		inRange(m3_channels_HSV.at(0), Scalar(0, 0, 0), Scalar(ColorThresh.RH_max, 0, 0), m3_channels_HSV.at(0));
		bitwise_or(m3_channels_HSV.at(0), m3_img_temp, m3_channels_HSV.at(0));
	}

	//再考虑B通道的阈值
	inRange(m3_channels_RGB.at(0), Scalar(ColorThresh.RB_min, 0, 0), Scalar(ColorThresh.RB_max, 0, 0), m3_channels_RGB.at(0));

	//再考虑R通道的阈值
	inRange( m3_channels_RGB.at(2), Scalar(ColorThresh.RR_min, 0, 0), Scalar(ColorThresh.RR_max, 0, 0), m3_channels_RGB.at(2));

	bitwise_and(m3_channels_HSV.at(0), m3_channels_RGB.at(0), m3_img_ranged);
	bitwise_and(m3_channels_RGB.at(2), m3_img_ranged, m3_img_ranged);
    emit imageShow(&m3_channels_HSV.at(0),"H_ranged");
    emit imageShow(&m3_channels_RGB.at(0),"B_ranged");
    emit imageShow(&m3_channels_RGB.at(2),"R_ranged");
    emit imageShow(&m3_img_ranged,"inranged");
	//imshow("V_ranged", channels_HSV.at(2));

	//结果图输出
	m3_img_ranged.copyTo(img_output);

	return;
}


//矩形拟合
void m3_rectBoundary(const Mat &img_input, Mat &img_output, m3_RectJudge &m3_Boundary_out, float m3_imgscale)
{
	//记录矩形的最大面积
	double maxarea = 0.0;
	double maxarea_2 = 0.0;
	double maxarea_3 = 0.0;

	//定义findContours()需要的vector容器
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Rect maxRect, maxRect_2, maxRect_3;
	m3_RECT m3_Rect_1, m3_Rect_2, m3_Rect_3;

	m3_Rect_1.widthheightratio = 0;
	m3_Rect_1.areaLowThresh = 10 * m3_imgscale*m3_imgscale;
	m3_Rect_1.valid = false;
	m3_Rect_2.widthheightratio = 0;
	m3_Rect_2.areaLowThresh = 10 * m3_imgscale*m3_imgscale;
	m3_Rect_2.valid = false;
	m3_Rect_3.widthheightratio = 0;
	m3_Rect_3.areaLowThresh = 10 * m3_imgscale*m3_imgscale;
	m3_Rect_3.valid = false;
	m3_Boundary_out.hor_Rect_num = 0;
	m3_Boundary_out.vec_Rect_num = 0;


	//创建容器用于寻找矩形
	Rect m3_FindingRect;

	//寻找轮廓
	findContours(img_input, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//开始搜索符合条件的矩形
	for (int i = 0; i < contours.size(); i++)
	{
		if (contours.at(i).size() < 1)
		{
			continue;
		}

		m3_FindingRect = boundingRect(contours[i]);
		if (m3_FindingRect.area() > maxarea_3)
		{
			if (m3_FindingRect.area() > maxarea_2)
			{
				maxarea_3 = maxarea_2;
				if (m3_FindingRect.area() > maxarea)
				{
					maxarea_2 = maxarea;
					maxRect = m3_FindingRect;
					maxarea = m3_FindingRect.area();
				}
				else
				{
					maxRect_2 = m3_FindingRect;
					maxarea_2 = m3_FindingRect.area();
				}
			}
			else 
			{
				maxRect_3 = m3_FindingRect;
				maxarea_3 = m3_FindingRect.area();
			}
		}
	}

	int size=0;
	if (maxarea_3 > 0) size = 3;
	if (maxarea_3 == 0.0&& maxarea_2 > 0) size = 2;
	if (maxarea_3 == 0.0&& maxarea_2 == 0.0 && maxarea > 0) size = 1;


	if (size==3)
	{
		m3_Rect_1.rect_org = maxRect;
		m3_Rect_1.widthheightratio = (float)m3_Rect_1.rect_org.width / m3_Rect_1.rect_org.height;

		m3_Rect_2.rect_org = maxRect_2;
		m3_Rect_2.widthheightratio = (float)m3_Rect_2.rect_org.width / m3_Rect_2.rect_org.height;

		m3_Rect_3.rect_org = maxRect_3;
		m3_Rect_3.widthheightratio = (float)m3_Rect_3.rect_org.width / m3_Rect_3.rect_org.height;

	}

	if (size==2)
	{
		m3_Rect_1.rect_org = maxRect;
		m3_Rect_1.widthheightratio = (float)m3_Rect_1.rect_org.width / m3_Rect_1.rect_org.height;

		m3_Rect_2.rect_org = maxRect_2;
		m3_Rect_2.widthheightratio = (float)m3_Rect_2.rect_org.width / m3_Rect_2.rect_org.height;

	}

	if (size==1)
	{
		m3_Rect_1.rect_org = maxRect;
		m3_Rect_1.widthheightratio = (float)m3_Rect_1.rect_org.width / m3_Rect_1.rect_org.height;
	}


		//如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
		//maxarea > m3_areaThresh意味着当前帧图像中有非0矩形，也即m3_Rect中的数据不会是上一次的
        if (size > 0 && m3_Rect_1.rect_org.area()>1000 &&(m3_Rect_1.widthheightratio > 6 || m3_Rect_1.widthheightratio < 0.3))
		{
			m3_Rect_1.valid = true;
            //rectangle(img_output, Point(m3_Rect_1.rect_org.tl()), Point(m3_Rect_1.rect_org.br()), Scalar(0, 0, 255), 2, 8, 0);
			if (m3_Rect_1.widthheightratio > 6)
				m3_Boundary_out.hor_Rect_num++;
				m3_Boundary_out.hor.push_back(m3_Rect_1.rect_org);
			if (m3_Rect_1.widthheightratio < 0.3)
				m3_Boundary_out.vec_Rect_num++;
				m3_Boundary_out.vec.push_back(m3_Rect_1.rect_org);

		}
		else
		{
			m3_Rect_1.valid = false;
		}
	
    if (size>1 && m3_Rect_2.rect_org.area()>1000 &&(m3_Rect_2.widthheightratio > 6 || m3_Rect_2.widthheightratio < 0.3))
	{
		m3_Rect_2.valid = true;
        //rectangle(img_output, Point(m3_Rect_2.rect_org.tl()), Point(m3_Rect_2.rect_org.br()), Scalar(255, 0, 255), 2, 8, 0);

		if (m3_Rect_2.widthheightratio > 6)
			m3_Boundary_out.hor_Rect_num++;
			m3_Boundary_out.hor.push_back(m3_Rect_2.rect_org);
		if (m3_Rect_2.widthheightratio < 0.3)
			m3_Boundary_out.vec_Rect_num++;
			m3_Boundary_out.vec.push_back(m3_Rect_2.rect_org);
	}
	else
	{
		m3_Rect_2.valid = false;
	}

    if (size>2  && m3_Rect_3.rect_org.area()>1000 && (m3_Rect_3.widthheightratio > 6 || m3_Rect_3.widthheightratio < 0.3))
	{
		m3_Rect_3.valid = true;
        //rectangle(img_output, Point(m3_Rect_3.rect_org.tl()), Point(m3_Rect_3.rect_org.br()), Scalar(0, 255, 255), 2, 8, 0);
	
		if (m3_Rect_3.widthheightratio > 6)
			m3_Boundary_out.hor_Rect_num++;
			m3_Boundary_out.hor.push_back(m3_Rect_3.rect_org);

		if (m3_Rect_3.widthheightratio < 0.3)
			m3_Boundary_out.vec_Rect_num++;
			m3_Boundary_out.vec.push_back(m3_Rect_3.rect_org);
	}
	else
	{
		m3_Rect_3.valid = false;
	}

	
}

//多组模板匹配函数
void m3_templatematch_vector(const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m3_MATCH &MatchResult)
{
	Mat img_result;
	double maxVal = 0.0;
	Scalar color;
	color = Scalar(255, 0, 0);
	for (int i = 0; i < TemplVec.size(); i++)
	{
		int result_cols = img_input.cols - TemplVec[i].cols + 1;
		int result_rows = img_input.rows - TemplVec[i].rows + 1;
		img_result.create(result_rows, result_cols, CV_32FC1);
		matchTemplate(img_input, TemplVec[i], img_result, CV_TM_CCOEFF_NORMED);
		minMaxLoc(img_result, &MatchResult.minVal, &MatchResult.maxVal, &MatchResult.minLoc,
			&MatchResult.maxLoc);

		MatchResult.matchLoc = MatchResult.maxLoc;


		//如果本次匹配结果的最大值比之前匹配都大，更新数据
		if (MatchResult.maxVal > maxVal)
		{
			//切记要更新参考最大值
			maxVal = MatchResult.maxVal;

			//更新匹配结果
			MatchResult.confidence = maxVal;
			MatchResult.tl_x = MatchResult.matchLoc.x;
			MatchResult.tl_y = MatchResult.matchLoc.y;
			MatchResult.br_x = MatchResult.matchLoc.x + TemplVec[i].cols;
			MatchResult.br_y = MatchResult.matchLoc.y + TemplVec[i].rows;

			//如果某个模板的置信度大于0.9，就采用这个模板，并且停止匹配
			if (MatchResult.confidence >= 0.9)
			{
				MatchResult.valid = true;
				rectangle(img_output, Point(MatchResult.tl_x, MatchResult.tl_y), Point(MatchResult.br_x, MatchResult.br_y), color, 2, 8, 0);
				qDebug() << "置信度" << MatchResult.confidence << endl;
				return;
			}
		}
	}

	//如果本次匹配结果的最大置信度大于0.7，就认为该次匹配是有效的
	if (MatchResult.confidence >= 0.7)
	{
		qDebug() << "置信度" << MatchResult.confidence << endl;
		MatchResult.valid = true;
		rectangle(img_output, Point(MatchResult.tl_x, MatchResult.tl_y), Point(MatchResult.br_x, MatchResult.br_y), color, 2, 8, 0);
	}
	else
	{
		qDebug() << "置信度" << MatchResult.confidence << endl;
		MatchResult.valid = false;
	}
	return;
}


