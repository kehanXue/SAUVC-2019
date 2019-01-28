#include "visionclass.h"
#include <unistd.h>

//m1多组模板匹配函数
void visionClass::templatematch_vector(const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m1_MATCH &MatchResult)
{
    Mat img_result;
    double maxVal = 0.0;
    Scalar color;
    color = Scalar(255, 0, 0);
    // qDebug() << "23333333333333333333333333333333333"<< TemplVec.size();

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
            // qDebug() << " qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq"<< maxVal;

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
                if(v_fout&&v_fout.is_open())
                {
                    v_fout << "置信度" << MatchResult.confidence << "\n";
                }

                char confidence[10];
                sprintf(confidence,"%f",MatchResult.confidence);
                putText( img_output, confidence, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

                return;

            }
        }
    }

    //如果本次匹配结果的最大置信度大于0.7，就认为该次匹配是有效的
    if (MatchResult.confidence >= 0.78)
    {
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "置信度" << MatchResult.confidence << "\n";
        }
        qDebug() << "置信度" << MatchResult.confidence << endl;
        MatchResult.valid = true;
        rectangle(img_output, Point(MatchResult.tl_x, MatchResult.tl_y), Point(MatchResult.br_x, MatchResult.br_y), color, 2, 8, 0);
    }
    else
    {
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "置信度" << MatchResult.confidence << "\n";
        }
        qDebug() << "置信度" << MatchResult.confidence << endl;
        MatchResult.valid = false;
    }

    char confidence[10];
    sprintf(confidence,"%f",MatchResult.confidence);
    putText( img_output, confidence, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

    return;
}

//m3多组模板匹配函数
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

                char confidence[10];
                sprintf(confidence,"%f",MatchResult.confidence);
                putText( img_output, confidence, Point( 20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

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

    char confidence[10];
    sprintf(confidence,"%f",MatchResult.confidence);
    putText( img_output, confidence, Point( 20, 200), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

    return;
}

//m4多组模板匹配函数
void visionClass::templatematch_vector( const Mat &img_input, Mat &img_output, const vector<Mat> &TemplVec, m4_MATCH &MatchResult)
{
    Mat img_result;
    double maxVal = 0.0;

    for( int i = 0; i < TemplVec.size(); i++)
    {
        int result_cols = img_input.cols - TemplVec[i].cols + 1;
        int result_rows = img_input.rows - TemplVec[i].rows + 1;
        if(v_fout&&v_fout.is_open())
        {
            v_fout << "the height of imgsrc" << img_input.rows << " width" << img_input.cols<<"\n";
            v_fout << "the height of templ" << TemplVec[i].rows << " width" << TemplVec[i].cols<<"\n";
        }
        qDebug() << "the height of imgsrc" << img_input.rows << " width" << img_input.cols;
        qDebug() << "the height of templ" << TemplVec[i].rows << " width" << TemplVec[i].cols;
        img_result.create( result_rows, result_cols, CV_32FC1 );
        matchTemplate( img_input, TemplVec[i], img_result, CV_TM_CCOEFF_NORMED );
        minMaxLoc( img_result, &MatchResult.minVal, &MatchResult.maxVal, &MatchResult.minLoc,
            &MatchResult.maxLoc );
        MatchResult.matchLoc = MatchResult.maxLoc;

        //如果本次匹配结果的最大值比之前匹配都大，更新数据
        if( MatchResult.maxVal > maxVal)
        {
            //切记要更新参考最大值
            maxVal = MatchResult.maxVal;

            //更新匹配结果
            MatchResult.confidence = maxVal;
            MatchResult.tl_x = MatchResult.matchLoc.x;
            MatchResult.tl_y = MatchResult.matchLoc.y;
            MatchResult.br_x = MatchResult.matchLoc.x + TemplVec[i].cols;
            MatchResult.br_y = MatchResult.matchLoc.y + TemplVec[i].rows;

            //如果某个模板的置信度大于0.85，就采用这个模板，并且停止匹配
            if( MatchResult.confidence >= 0.85)
            {
                MatchResult.valid = true;
                rectangle( img_output, Point(MatchResult.tl_x,MatchResult.tl_y), Point(MatchResult.br_x,MatchResult.br_y), Scalar(255,0,0), 2, 8, 0);
                char confidence[10];
                sprintf(confidence,"%f",MatchResult.confidence);
                putText( img_output, confidence, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);
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

    char confidence[10];
    sprintf(confidence,"%f",MatchResult.confidence);
    putText( img_output, confidence, Point( 20, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0), 2, 8);

    return;
}
