#include "missionacquire.h"
#include "visionclass.h"
#include <unistd.h>
#include <QApplication>
#include <QEventLoop>
//任务三：捡球

void visionClass::missionAcquire()
{
	//修改运行状态，便于主控查询
	data.m3_runningState = true;
    data.m3_green_area=0;


	//任务变量
	Mat m3_imgsrc,					//源图像
        m3_hsv,
		m3_imgblur,					//中值滤波图像
		m3_bgr_streched,			//双通道拉伸之后的RGB图像
		m3_imgranged,				//双边阈值颜色识别之后的图像
        m3g_imgranged,
		m3_imgcanny,				//边缘识别之后的图像
		m3_imgROI;

    vector<Mat> m3_bgr_channels;	//BGR双通道

	

    //bool m3_start = true;				//主控调用任务二标志位
    bool m3_camSetting1 = false;
    bool m3_camSetting2 = false;
    bool m3_camSetting3 = false;

	int m3_iteration = 0;				//程序执行次数
	int m3_pixelsum = 0;				//颜色识别后像素点计数
    int m3_camSetttingStart = 0;
    int m3_green_pixelsum=0;
    int m3_green_closenum = 0;
    int m3_green_farnum = 0;

	int ball_dx = 999;
	int ball_dy = 999;
    int ball_dx_copy = 999;
    int ball_dy_copy = 999;
	int ball_x=999;
	int ball_y=999;


    int cnt_delay=0;

    static const float m3_imgscale = 0.5;			//图像缩放比例
	static const int m3_saveinterval = 3;			//图像保存周期
    static const int m3_pixelThresh = 20000 * m3_imgscale*m3_imgscale;	//颜色识别后像素点下阈值   
    //static const int m3_green_closenum = 3;
    //static const int m3_green_farnum = 3;

	//用于颜色识别的结构体
	m3_COLORTHRESH colorThresh;
    colorThresh.RB_max = m3_RB_max;
    colorThresh.RB_min = m3_RB_min;
    colorThresh.RH_max = m3_RH_max;
    colorThresh.RH_min = m3_RH_min;

    m3_COLORTHRESH colorThreshG;
    colorThreshG.RB_max = m3G_RB_max;
    colorThreshG.RB_min = m3G_RB_min;
    colorThreshG.RH_max = m3G_RH_max;
    colorThreshG.RH_min = m3G_RH_min;

	//用于矩形拟合的结构体
	m3_RECT m3_Rect;
	m3_Rect.heightwidthratio = 3;
	m3_Rect.area = 0;
    m3_Rect.areaLowThresh = 20000 * m3_imgscale*m3_imgscale;
	m3_Rect.valid = false;

	vector<Mat> m3_TemplVec;        //模板组
	int m3_templcount = 0;          //模板组计数
    vector<Mat> m3_TemplVec_b;        //模板组
    int m3_templcount_b = 0;          //模板组计数
    vector<Mat> m3_TemplVec_c;        //模板组
    int m3_templcount_c = 0;          //模板组计数

	
    //static const int m3_templfindballcountThresh = 10;     //模板匹配累计有效帧数上限阈值


	int m3_heightscaled = 0,            //缩放后，源图像的高
		m3_widthscaled = 0;                 //缩放后，源图像的宽

    //int m3_templfindballcount = 0,     //模板匹配累计有效帧数
    //	m3_areaupcount = 0;                 //矩形拟合面积超过上阈值的帧数
	int cnt_test1=0;
	int cnt_test2=0;

    //bool m3_templfindball = false;         //模板匹配已连续数帧定位ball

	m3_MATCH m3_MatchResult;
	m3_MatchResult.valid = false;

	

	//载入模板组
	Mat m3_imgtempl;
    Mat m3_imgtempl_b;
    Mat m3_imgtempl_c;
	while (1)
	{
        char imgname[100] = "m3_";
		char count1[10];
		memset(count1, 0, sizeof(count1));
		m3_templcount++;
		sprintf(count1, "%d", m3_templcount);
		strcat(count1, ".jpg");
		strcat(imgname, count1);
		m3_imgtempl = imread(imgname);
		if (m3_imgtempl.data == 0)
		{
			m3_templcount--;
			//如果没有模板载入，提示
			if (m3_templcount == 0)
			{
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "No template has been loaded. Please check!\n"<<std::endl;
                }
                qDebug() << "No template has been loaded. Please check!";
			}
			break;
		}
		else
		{
			//如果模板载入成功，进行缩放
			resize(m3_imgtempl, m3_imgtempl, Size(), m3_imgscale, m3_imgscale);
			m3_TemplVec.push_back(m3_imgtempl);
            if(v_fout&&v_fout.is_open())
            {
                v_fout<< "模板" << m3_templcount << "\n" << "height" << m3_imgtempl.rows << " width" << m3_imgtempl.cols <<std::endl;
            }
			qDebug()<< "模板" << m3_templcount << endl << "height" << m3_imgtempl.rows << " width" << m3_imgtempl.cols << endl;
		}
	}

    while (1)
    {
        char imgname[15] = "m3_b_";
        char count1[10];
        memset(count1, 0, sizeof(count1));
        m3_templcount_b++;
        sprintf(count1, "%d", m3_templcount_b);
        strcat(count1, ".jpg");
        strcat(imgname, count1);
        m3_imgtempl_b = imread(imgname);
        if (m3_imgtempl_b.data == 0)
        {
            m3_templcount_b--;
            //如果没有模板载入，提示
            if (m3_templcount_b == 0)
            {
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "No template has been loaded. Please check!\n"<<std::endl;
                }
                qDebug() << "No template has been loaded. Please check!";
            }
            break;
        }
        else
        {
            //如果模板载入成功，进行缩放
            resize(m3_imgtempl_b, m3_imgtempl_b, Size(), m3_imgscale, m3_imgscale);
            m3_TemplVec_b.push_back(m3_imgtempl_b);
            if(v_fout&&v_fout.is_open())
            {
                v_fout<< "模板" << m3_templcount_b << "\n" << "height" << m3_imgtempl_b.rows << " width" << m3_imgtempl_b.cols <<std::endl;
            }
            qDebug()<< "模板" << m3_templcount_b << endl << "height" << m3_imgtempl_b.rows << " width" << m3_imgtempl_b.cols << endl;
        }
    }

    while (1)
    {
        char imgname[15] = "m3_c_";
        char count2[10];
        memset(count2, 0, sizeof(count2));
        m3_templcount_c++;
        sprintf(count2, "%d", m3_templcount_c);
        strcat(count2, ".jpg");
        strcat(imgname, count2);
        m3_imgtempl_c = imread(imgname);
        if (m3_imgtempl_c.data == 0)
        {
            m3_templcount_c--;
            //如果没有模板载入，提示
            if (m3_templcount_c == 0)
            {
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "No template has been loaded. Please check!"<<std::endl;
                }
                qDebug() << "No template has been loaded. Please check!";
            }
            break;
        }
        else
        {
            //如果模板载入成功，进行缩放
            resize(m3_imgtempl_c, m3_imgtempl_c, Size(), m3_imgscale, m3_imgscale);
            m3_TemplVec_c.push_back(m3_imgtempl_c);
            if(v_fout&&v_fout.is_open())
            {
                v_fout<< "模板" << m3_templcount_c << "\n" << "height" << m3_imgtempl_c.rows << " width" << m3_imgtempl_c.cols <<std::endl;
            }
            qDebug()<< "模板" << m3_templcount_c << endl << "height" << m3_imgtempl_c.rows << " width" << m3_imgtempl_c.cols << endl;
        }
    }

    //创建窗口，定义滑动
    emit createWindow("missionggg");
    emit createWindow("imgROI");
    //emit createWindow("H_ranged");
    //emit createWindow("B_ranged");
    //emit createBar("RB_max", "B_ranged", &colorThresh.RB_max, 255);
    //emit createBar("RB_min", "B_ranged", &colorThresh.RB_min, 255);
    //emit createBar("RH_max", "H_ranged", &colorThresh.RH_max, 180);
    //emit createBar("RH_min", "H_ranged", &colorThresh.RH_min, 180);
    
     //connect(control,SIGNAL(),this,SLOT(onGetHandclosed(bool ok)));

	//开启采集
    visionClass::start(Cam_Bottom);
    //AcquireCamSetting();

   // int expotime_a=0;
while (m3_start)
{
    //expotime_a++;
   // if(expotime_a<10)
          //AcquireCamSetting();
     emit cameraStartedB_m(true);
	//程序执行次数+1
	m3_iteration++;

    if( m3_Rect.valid && !m3_camSetting1)
    {
        m3_camSetting1 = true;
        m3_camSetttingStart = m3_iteration;
    }
    if( m3_camSetting1 && m3_iteration-m3_camSetttingStart < 5)
    {
       DropCamSetting();
    }

    //读入图像
    //m3_imgsrc = imread("132.jpg");
    frame2Mat(Cam_Bottom, m3_imgsrc);
    transpose(m3_imgsrc,m3_imgsrc);
    flip(m3_imgsrc,m3_imgsrc,0);
     //rect(x,y,width,height) x+width<580 y+height<780
    m3_imgsrc=m3_imgsrc(Rect(0 , 100, 580, 680));


    m3_heightscaled = m3_imgsrc.rows * m3_imgscale;
    m3_widthscaled = m3_imgsrc.cols * m3_imgscale;

    char deepValue[10];
    sprintf( deepValue, "%f", visionClass::deepValue);
    //putText( m3_imgsrc, deepValue, Point( 400, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);


    //满足条件，即保存图像
    if (m3_iteration % m3_saveinterval == 0)
    {
        char imgnamesave[15] = "save3_";
        char countsave[10];
        sprintf(countsave, "%d", m3_iteration);
        strcat(imgnamesave, countsave);
        strcat(imgnamesave, ".jpg");
        imwrite(imgnamesave, m3_imgsrc);
    }
    qDebug()<<"2";
	//图像缩放
	resize(m3_imgsrc, m3_imgsrc, cv::Size(), m3_imgscale, m3_imgscale);
    m3_bgr_streched=m3_imgsrc.clone();
    //中值滤波imgROI
    medianBlur(m3_imgsrc, m3_imgblur, 31);

    if(data.m3_green_area!=999&&data.m3_green_area!=999)
    {
        //medianBlur(m3_imgsrc, m3_imgblur, 31);
        colorRecognize(m3_imgblur, m3g_imgranged, colorThreshG);
        m3_green_pixelsum = countNonZero( m3g_imgranged );

        qDebug()<<"pixelsum  "<<m3_green_pixelsum<<endl;

        if(m3_green_pixelsum>8000)
        {
            m3_green_closenum++;
            if(m3_green_closenum>3)
                data.m3_green_area=-999;
        }
        else if(m3_green_pixelsum<800)
        {
            m3_green_farnum++;
            if(m3_green_farnum>3)
                data.m3_green_area=999;
        }

        else data.m3_green_area=0;

        qDebug()<<"m3_green_area  "<<data.m3_green_area;
    }

	//未收到机械手闭合信号则发送球盆位置，收到闭合信号则检验球是否在机械手中
	if (m3_testball == 0)
	{

		cnt_test1 = 0;
		cnt_test2 = 0;

		//双通道双边阈值颜色识别：H通道、B通道
        colorRecognize(m3_imgblur, m3_imgranged, colorThresh);

		//计算像素点数
		//如果像素点数大于阈值，进行矩形拟合
		m3_pixelsum = countNonZero(m3_imgranged);
		if (m3_pixelsum >= m3_pixelThresh)
		{
			//进行canny边缘检测，提高矩形拟合的准确度
            //Canny(m3_imgranged, m3_imgcanny, 20, 50);
            rectBoundary(m3_imgranged, m3_bgr_streched, m3_Rect);
            //原图进行模板匹配，检测小球
             if(m3_downfinished==0)
            {
                  m3_camSetting2=false;
                  if( m3_Rect.valid && !m3_camSetting3)
                  {
                      m3_camSetting3 = true;
                      m3_camSetttingStart = m3_iteration;
                  }
                  if( m3_camSetting3 && m3_iteration-m3_camSetttingStart < 10)
                  {
                      DropCamSetting();
                  }
                 templatematch_vector(m3_imgsrc, m3_bgr_streched, m3_TemplVec, m3_MatchResult);
                 if (m3_MatchResult.valid == true)
                {
                        ball_dx = (0.5*m3_imgsrc.cols - 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x)) / m3_imgscale;
                        ball_dy = (0.45*m3_imgsrc.rows - 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y)) / m3_imgscale;


                       rectangle(m3_bgr_streched, Point(288*m3_imgscale, 304*m3_imgscale), Point(292*m3_imgscale,308*m3_imgscale), Scalar(255, 255, 0), 2, 8, 0);
                 }
                 else
                 {
                     //本次模板匹配无效
                     ball_dx = 999;
                     ball_dy = 999;
                 }


                 ball_dx_copy=ball_dx;
                 ball_dy_copy=ball_dy;


             }
             else if(m3_downfinished==1)
               {
                   /* m3_camSetting3=false;*/
                 putText( m3_bgr_streched, "m3_downfinished true", Point( 20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
                 if( !m3_camSetting2)
                 {
                     m3_camSetting2 = true;
                     m3_camSetttingStart = m3_iteration;
                 }
                 if( m3_camSetting2 && m3_iteration-m3_camSetttingStart < 10)
                 {
                     AcquireCamSetting();
                     putText( m3_bgr_streched, "Bottom camera has been reset", Point( 20, 250), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
                     if(v_fout&&v_fout.is_open())
                     {
                        v_fout << "Bottom camera has been reset." <<std::endl;
                     }
                     qDebug() << "Bottom camera has been reset." << endl;
                 }

                 templatematch_vector(m3_imgsrc, m3_bgr_streched, m3_TemplVec_b, m3_MatchResult);
                 if (m3_MatchResult.valid == true)
                   {

                        cnt_delay=0;
                        ball_dx = (0.5*m3_imgsrc.cols - 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x)) / m3_imgscale;
                        ball_dy = (0.5*m3_imgsrc.rows- 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y)) / m3_imgscale;
                        ball_dx_copy=ball_dx;
                        ball_dy_copy=ball_dy;
                        rectangle(m3_bgr_streched, Point(286*m3_imgscale, 336*m3_imgscale), Point(294*m3_imgscale,344*m3_imgscale), Scalar(255, 255, 0), 2, 8, 0);
                  }

                 else
                 {
                    if(ball_dx_copy!=999 && cnt_delay<2)
                    {
                        cnt_delay++;
                        //copy不变
                                }
                    else
                     {
                        //本次模板匹配无效
                     ball_dx = 999;
                     ball_dy = 999;
                     ball_dx_copy=999;
                     ball_dy_copy=999;
                     cnt_delay=0;

                    }
                 }

               }

        }

		else
		{
			//切记：如果像素点数不满足阈值条件，也是valid=fasle的条件
			m3_Rect.valid = false;
		}

        //如果本次矩形拟合有效，给主控发送数据，显示图像
            //当m3_drum_dx>0时，航行器应当向左平移；当m3_drum_dx<0时，航行器应当向右平移
        if (m3_Rect.valid == true)
            {    //程序退出之前，m3_runningState清0,便于主控查询
                data.m3_runningState = false;
                double ball_center_x = 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x);
                double ball_center_y = 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y);
                Mat ROI_left,
                    ROI_right,
                    ROI_up,
                    ROI_down;
                int point_left=0,
                    point_right=0,
                    point_up=0,
                    point_down=0;

                data.m3_rotate_cw_angle =0;

            data.m3_drum_dx = ((0.5*m3_imgsrc.cols - 0.5*(m3_Rect.tl_x + m3_Rect.br_x)) / m3_imgscale)+30;
            data.m3_drum_dy = ((0.5*m3_imgsrc.rows - 0.5*(m3_Rect.tl_y + m3_Rect.br_y)) / m3_imgscale)+30;
            data.m3_ball_dx = ball_dx_copy;
            data.m3_ball_dy = ball_dy_copy;


		}
		//如果本次矩形拟合无效，给999
		else
		{
			data.m3_drum_dx = 999;
			data.m3_drum_dy = 999;
			data.m3_ball_dx = 999;
			data.m3_ball_dy = 999;
            ball_dx = 999;
            ball_dy = 999;
            ball_dx_copy=999;
            ball_dy_copy=999;
            cnt_delay=0;
		}

        char m3_drum_dx[10],
            m3_drum_dy[10],
            m3_ball_dx[10],
            m3_ball_dy[10],
                m3_confidence[10];
        sprintf( m3_drum_dx, "%d", data.m3_drum_dx);
        sprintf( m3_drum_dy, "%d", data.m3_drum_dy);
        sprintf( m3_ball_dx, "%d", data.m3_ball_dx);
        sprintf( m3_ball_dy, "%d", data.m3_ball_dy );
        sprintf( m3_confidence, "%f", m3_MatchResult.confidence);
        putText( m3_bgr_streched, m3_drum_dx, Point( 20, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
        putText( m3_bgr_streched, m3_drum_dy, Point( 100, 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
        putText( m3_bgr_streched, m3_ball_dx, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);
        putText( m3_bgr_streched, m3_ball_dy, Point( 100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(100,0,0), 2, 8);
       // putText( m3_bgr_streched, m3_confidence, Point( 20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

        emit imageShow(&m3_bgr_streched,"missionggg");

		//输出调试信息
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "m3_iteration:" << m3_iteration <<std::endl;
            v_fout << "m3_pixelsum" << m3_pixelsum <<std::endl;
            v_fout << "m3_maxArea:" << m3_Rect.area <<std::endl;
            v_fout << "m3_drum_dx:" << data.m3_drum_dx <<std::endl;
            v_fout << "m3_drum_dy:" << data.m3_drum_dy <<std::endl;
            v_fout << "m3_ball_dx:" << data.m3_ball_dx <<std::endl;
            v_fout << "m3_ball_dy:" << data.m3_ball_dy <<std::endl;
            v_fout << "ball_dx_copy:" << ball_dx_copy <<std::endl;
            v_fout << "ball_dy_copy:" << ball_dy_copy <<std::endl;
            v_fout << "ball_dx:" << ball_dx <<std::endl;
            v_fout << "ball_dy:" << ball_dy <<std::endl;
            v_fout << "cnt_delay:" << cnt_delay <<std::endl;

        }
        qDebug() << "m3_iteration:" << m3_iteration << endl;
        qDebug() << "m3_pixelsum" << m3_pixelsum << endl;
        qDebug() << "m3_maxArea:" << m3_Rect.area << endl;
        qDebug() << "m3_drum_dx:" << data.m3_drum_dx << endl;
        qDebug() << "m3_drum_dy:" << data.m3_drum_dy << endl;
        qDebug() << "m3_ball_dx:" << data.m3_ball_dx << endl;
        qDebug() << "m3_ball_dy:" << data.m3_ball_dy << endl;
        qDebug() << "ball_dx_copy:" << ball_dx_copy << endl;
        qDebug() << "ball_dy_copy:" << ball_dy_copy << endl;
        qDebug() << "ball_dx:" << ball_dx << endl;
        qDebug() << "ball_dy:" << ball_dy << endl;
        qDebug() << "cnt_delay:" << cnt_delay << endl;

		
	}
	else
	{
         data.m3_acquireFinish=true;// 检测程序是否运行
         data.m3_ball_acquire=0;
		//创建感兴趣区域
        //rect(x,y,width,height) x+width<580 y+height<780
        m3_imgROI=m3_imgsrc(Rect(200 * m3_imgscale, 394*m3_imgscale, 227*m3_imgscale, 186*m3_imgscale));
        templatematch_vector(m3_imgROI, m3_imgROI, m3_TemplVec_c, m3_MatchResult);
        //emit imageShow(&m3_imgROI,"img_ROI");
        emit imageShow(&m3_imgROI,"img_ROI");
		if (m3_MatchResult.valid == true)
		{
			//模板匹配有效
			ball_x = 0.5*(m3_MatchResult.tl_x + m3_MatchResult.br_x);
			ball_y = 0.5*(m3_MatchResult.tl_y + m3_MatchResult.br_y);
            if(v_fout&&v_fout.is_open())
            {
                 v_fout << "ball_x" << ball_x << "   ball_y" << ball_y <<std::endl;
            }
            qDebug() << "ball_x" << ball_x << "   ball_y" << ball_y << endl;
            if (ball_x >0* m3_imgscale && ball_x < 227* m3_imgscale && ball_y>0 * m3_imgscale && ball_y < 186 * m3_imgscale)
			{
				cnt_test1++;//找到
				cnt_test2 = 0;
			}

			else
			{ 
				cnt_test2++;
				cnt_test1 = 0;
			}
				
		}
		else
		{
			cnt_test2++;
			cnt_test1 = 0;
		}
        if(v_fout&&v_fout.is_open())
        {
            v_fout<<"cnt_test1"<<cnt_test1<<std::endl;
            v_fout<<"cnt_test2"<<cnt_test2<<std::endl;
        }

        qDebug()<<"cnt_test1"<<cnt_test1;
        qDebug()<<"cnt_test2"<<cnt_test2;
		if (cnt_test1>5)
		{
            data.m3_ball_acquire=-999;
            if(v_fout&&v_fout.is_open())
            {
                 v_fout <<"抓取成功"<<std::endl;
            }
			qDebug() << "抓取成功" << endl;
            m3_start=false;

		}
        if (cnt_test2>13)
        {
            data.m3_ball_acquire=999;
            if(v_fout&&v_fout.is_open())
            {
                 v_fout <<"抓取未成功"<<std::endl;
            }
			qDebug() << "抓取未成功" << endl;        

		}

    }
    //满足条件，即保存图像
    if (m3_iteration % m3_saveinterval == 0)
    {
        char imgnamesave[15] = "save_m3_";
        char countsave[10];
        sprintf(countsave, "%d", m3_iteration);
        strcat(imgnamesave, countsave);
        strcat(imgnamesave, ".jpg");
        Mat save;
        resize(m3_bgr_streched, save, cv::Size(), 1/m3_imgscale, 1/m3_imgscale);
        imwrite(imgnamesave, save);
    }
	    timeval tempt;
        gettimeofday(&tempt,NULL);
        data.t_now=tempt.tv_sec*1000+tempt.tv_usec/1000;
        //等待！
        QEventLoop loop;
        QTimer::singleShot(50,&loop,SLOT(quit()));
        loop.exec();
        if(v_fout&&v_fout.is_open())
        {
             v_fout << "m3_start : " << m3_start<<"\n"<<"\n"<<std::endl;
        }
        qDebug() << "m3_start : " << m3_start << endl;
}

	//停止调用相机
    stop(Cam_Bottom);
    emit cameraStoppedB(true);

    //销毁所有窗口
    emit destroyWindow();

    //程序退出之前，m3_runningState清0,便于主控查询
    data.m3_runningState = false;
    if(v_fout&&v_fout.is_open())
    {
         v_fout << "m3_runningState : " << data.m3_runningState<<"\n"<<"\n"<<"\n"<<std::endl;
    }
    qDebug() << "m3_runningState : " << data.m3_runningState << endl;

	return ;

}

//双边阈值颜色识别
/*void visionClass::colorRecognize( const Mat &img_input, Mat &img_output, m3_COLORTHRESH &ColorThresh)
{
    Mat img_rgb,
    img_hsv,
    img_H,
    img_H_ranged,
    img_temp1,
    img_temp2,
    img_B,
    img_B_ranged,
    img_ranged;

    vector<Mat> channels_RGB;
    vector<Mat> channels_HSV;
    img_rgb = img_input.clone();
    cvtColor( img_rgb, img_hsv, COLOR_BGR2HSV);
    split( img_rgb, channels_RGB);
    split( img_hsv, channels_HSV);
    img_H = channels_HSV.at(0);
    img_B = channels_RGB.at(1);

    //鉴于H通道是个环形的，需要考虑转一圈的情形
    if( ColorThresh.RH_max >= ColorThresh.RH_min)
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_H_ranged);
    }
    else
    {
        inRange( img_H, Scalar( ColorThresh.RH_min, 0, 0), Scalar( 180, 0, 0), img_temp1);
        inRange( img_H, Scalar( 0, 0, 0), Scalar( ColorThresh.RH_max, 0, 0), img_temp2);
        bitwise_or( img_temp1, img_temp2, img_H_ranged);
    }

    //再考虑B通道的阈值
    inRange( img_B, Scalar( ColorThresh.RB_min, 0, 0), Scalar( ColorThresh.RB_max, 0, 0), img_B_ranged);

    //两幅图像膨胀之后再按位取与
    dilate( img_H_ranged, img_H_ranged, 5);
    dilate( img_B_ranged, img_B_ranged, 5);
    bitwise_and( img_H_ranged, img_B_ranged, img_ranged);

    emit imageShow(&img_H_ranged,"H_ranged");
    emit imageShow(&img_B_ranged,"B_ranged");

    //结果图输出
    img_ranged.copyTo( img_output);

    return;
}


//矩形拟合
void visionClass::rectBoundary(const Mat &img_input, Mat &img_output, m3_RECT &m3_Rect)
{
	//记录矩形的最大面积
	double maxarea = 0.0;

	//定义findContours()需要的vector容器
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

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
		if (m3_FindingRect.area() > maxarea)
		{
			maxarea = m3_FindingRect.area();
			m3_Rect.area = m3_FindingRect.area();
			m3_Rect.tl_x = m3_FindingRect.tl().x;
			m3_Rect.tl_y = m3_FindingRect.tl().y;
			m3_Rect.br_x = m3_FindingRect.br().x;
			m3_Rect.br_y = m3_FindingRect.br().y;
			m3_Rect.center_x = 0.5*(m3_Rect.tl_x + m3_Rect.br_x);
			m3_Rect.center_y = 0.5*(m3_Rect.tl_y + m3_Rect.br_y);
		}
	}

	//如果面积最大的矩形还满足其他的约束条件，就认为本次识别有效，并且画出这个矩形
	//maxarea > m3_areaThresh意味着当前帧图像中有非0矩形，也即m3_Rect中的数据不会是上一次的
	if (maxarea > m3_Rect.areaLowThresh)
	{
		m3_Rect.valid = true;
		rectangle(img_output, Point(m3_Rect.tl_x, m3_Rect.tl_y), Point(m3_Rect.br_x, m3_Rect.br_y), Scalar(0, 0, 255), 2, 8, 0);
	}
	else
	{
		m3_Rect.valid = false;
	}
	return;
}

//多组模板匹配函数
void visionClass::templatematch_vector(const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m3_MATCH &MatchResult)
{
	Mat img_result;
	double maxVal = 0.0;

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
				
				rectangle(img_output, Point(MatchResult.tl_x, MatchResult.tl_y), Point(MatchResult.br_x, MatchResult.br_y), Scalar(255, 0, 0), 2, 8, 0);
                if(v_fout&&v_fout.is_open())
                {
                     v_fout <<  "置信度" << MatchResult.confidence <<"\n";
                }
                qDebug() << "置信度" << MatchResult.confidence << endl;
				return;
			}
		}
	}

	//如果本次匹配结果的最大置信度大于0.7，就认为该次匹配是有效的
    if (MatchResult.confidence >= 0.7)
	{
        if(v_fout&&v_fout.is_open())
        {
             v_fout <<  "置信度" << MatchResult.confidence <<"\n";
        }
        qDebug() << "置信度" << MatchResult.confidence << endl;
		MatchResult.valid = true;
		rectangle(img_output, Point(MatchResult.tl_x, MatchResult.tl_y), Point(MatchResult.br_x, MatchResult.br_y), Scalar(255, 0, 0), 2, 8, 0);
	}
	else
	{
        if(v_fout&&v_fout.is_open())
        {
             v_fout <<  "置信度" << MatchResult.confidence <<"\n";
        }
        qDebug() << "置信度" << MatchResult.confidence << endl;
		MatchResult.valid = false;
	}

	return;
}
*/

void visionClass::onGetHandclosed(bool ok)
{
     m3_testball = ok;
}

void visionClass::onGodownFinished(bool ok)
{
     m3_downfinished = ok;
}

/*static int histSize_gate[1] = { 256 };					//直方图的点数
static float hranges_gate[2] = { 0.0, 255.0 };			//直方图的范围
static const float* ranges_gate[1] = { hranges_gate };	//指向该范围的指针
static int channels_gate[1] = { 0 };						//通道

//查找表函数
Mat m3_applyLookUp(const Mat& image, const Mat& lookup)
{
    Mat result;
    LUT(image, lookup, result);
    return result;
}

//计算直方图
Mat m3_getHistogram(const Mat &img_input)
{
    Mat hist;

    //参数为：源图像（序列）地址，输入图像的个数，通道数，掩码，输出结果，直方图维数，每一维的大小，每一维的取值范围
    calcHist(&img_input, 1, channels_gate, Mat(), hist, 1, histSize_gate, ranges_gate);

    return hist;
}

//拉伸直方图
Mat m3_strech(const Mat &img_input, const Mat &hist, int minValue, int start_offset)
{
    //左边入口
    int imin = 0;
    for (; imin< histSize_gate[0]; imin++)
    {
        if (hist.at<float>(imin) > minValue && imin >= start_offset)
            break;
    }

    //右边入口
    int imax = histSize_gate[0] - 1;
    for (; imax >= 0; imax--)
    {
        if (hist.at<float>(imax) > minValue && imax >= start_offset)
            break;
    }

    //创建查找表
    int dim(256);
    Mat lookup(1, &dim, CV_8U);

    for (int i = 0; i < 256; i++)
    {
        if (i < imin)
        {
            lookup.at<uchar>(i) = 0;
        }
        else if (i > imax)
        {
            lookup.at<uchar>(i) = 255;
        }
        else
        {
            lookup.at<uchar>(i) = static_cast<uchar>(255.0*(i - imin) / (imax - imin) + 0.5);
        }
    }

    Mat result;
    result = m3_applyLookUp(img_input, lookup);
    return result;
}

//画出直方图
Mat m3_getHistogramImage(const Mat &hist)
{
    //获取最大值和最小值
    double maxVal = 0;
    double minVal = 0;

    //minMaxLoc用来获得最大值和最小值,后面两个参数为最小值和最大值的位置，0代表不需要获取
    minMaxLoc(hist, &minVal, &maxVal, 0, 0);

    //展示直方图的画板：底色为白色
    Mat histImg(histSize_gate[0], histSize_gate[0], CV_8U, Scalar(255));

    //将最高点设为bin总数的90%
    int hpt = static_cast<int>(0.9*histSize_gate[0]);

    //为每一个bin画一条线
    for (int h = 0; h < histSize_gate[0]; h++)
    {
        float binVal = hist.at<float>(h);
        int intensity = static_cast<int>(binVal*hpt / maxVal);
        line(histImg, Point(h, histSize_gate[0]), Point(h, histSize_gate[0] - intensity), Scalar::all(0));
    }

    return histImg;
}*/
