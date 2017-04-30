#include "usartassistant.h"
#include "ui_usartassistant.h"
#include <QComboBox>
#include <QDebug>
#include <QByteArray>
#include <QTextEdit>
#include <QMessageBox>
#include <QGraphicsView>
#include <QTime>
#include <QImage>
#include <QDateTime>
#include <QLabel>
#include <QImage>
#include <QScrollBar>
#include <QAbstractScrollArea>
#include <QStyleFactory>
#include <QDialog>

#include "HalconCpp.h"
#include "HDevThread.h"

QT_BEGIN_NAMESPACE
using namespace HalconCpp;
QT_END_NAMESPACE

UsartAssistant::UsartAssistant(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsartAssistant)
{
    ui->setupUi(this);
    connect( ui->SerialRead_textEdit, SIGNAL(textChanged()), this, SLOT(autoScroll())); // SerialRead_textEdit自动滚屏
    connect( ui->SerialWrite_textEdit, SIGNAL(textChanged()), this, SLOT(autoScroll())); // SerialWrite_textEdit自动滚屏
    connect( &serial, SIGNAL(readyRead()), this, SLOT(serialRead()) );   // 连接窗口读取数据
    connect( this, SIGNAL(ReceivedData34(unsigned char*)), this, SLOT(cameraJob(unsigned char*)) );
    serial.setReadBufferSize( 2048 );
    UsartAssistant::initSeialPort();

    SendData[0] = 0x34;
    SendData[1] = 0x56;
    SendData[15] = ~0x34;
    SendData[16] = ~0x56;
}

UsartAssistant::~UsartAssistant()
{
    delete ui;
}

void UsartAssistant::initSeialPort()    {
    //获取计算机上所有串口并添加到comboBox中
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty())
    {
        //ui->Com_comboBox->addItem("nothing");
        //ui->Com_comboBox->addItem(QStringLiteral(""));
        qDebug() << "no serialport" << endl;
        return;
    }
    foreach (QSerialPortInfo info, infos) {
        ui->Com_comboBox->addItem(info.portName());
        qDebug() << info.portName();
        UsartAssistant::ComInfo = info;
    }
    //UsartAssistant::on_Com_comboBox_currentIndexChanged( UsartAssistant::ComInfo.portName() );
}

void UsartAssistant::on_Com_comboBox_currentIndexChanged(const QString &arg1)
{
    //qDebug() << "Com_combobox enter" << endl;
    QSerialPortInfo info;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    int i = 0;
    foreach (info, infos) {
        if(info.portName() == arg1) break;
        i++;
    }
    if(i != infos.size ()){//can find
        //        ui->lblStatus->setText("串口打开成功");
        qDebug() << "open success";
        serial.close();
        serial.setPort(info);
        //serial.setReadBufferSize( 2048 );
        serial.open(QIODevice::ReadWrite);          //读写打开
        serial.setBaudRate(QSerialPort::Baud115200);  //波特率
        serial.setDataBits(QSerialPort::Data8);     //数据位
        serial.setParity(QSerialPort::NoParity);    //无奇偶校验
        serial.setStopBits(QSerialPort::OneStop);   //无停止位
        serial.setFlowControl(QSerialPort::NoFlowControl);  //无控制
    }else{
        serial.close();
        qDebug() << "open fail" << endl;
    }
}

void UsartAssistant::autoScroll()
{
    ui->SerialRead_textEdit->verticalScrollBar()->setValue(ui->SerialRead_textEdit->verticalScrollBar()->maximum());
    ui->SerialWrite_textEdit->verticalScrollBar()->setValue(ui->SerialWrite_textEdit->verticalScrollBar()->maximum());
}

void UsartAssistant::serialRead()
{
    QString Str;
    static QByteArray BufferAll;
    BufferAll.append( serial.readAll() );
    if( BufferAll.isNull() == false ) {
        ui->SerialRead_textEdit->insertPlainText( BufferAll.toHex() );
        //ui->SerialRead_textEdit->insertPlainText( QString("\r\n\r\n") );
        ReceivedDataCount = BufferAll.length();
        ui->SerialRead_textEdit->insertPlainText( QString("____-> ") );
        ui->SerialRead_textEdit->insertPlainText( Str.setNum(ReceivedDataCount) );
        ui->SerialRead_textEdit->insertPlainText( QString("\r\n") );
    }
    if( ReceivedDataCount >= 17 )   {   //接受到17位数据
        for( int i=0; i<BufferAll.size(); i++ )  ReceivedData[i] = BufferAll.at(i);
        ReceivedData[17] = '\0';

        if( ReceivedData[0] == 0x34 && ReceivedData[1] == 0x56 && ReceivedData[15] == (unsigned char)~(0x34) && ReceivedData[16] == (unsigned char)~(0x56) )
                emit UsartAssistant::cameraJob( ReceivedData );
        else    {               // 错误接收 请重新发送...
//            SendData[2] = 0x33;
//            for( int i=3; i<15; i++ )   SendData[i] = 0x00; // clear
//            serial.write( (char *) SendData, 17 );
        }
        BufferAll.clear();
        ReceivedDataCount = 0;
    }
}

/**
 * @brief UsartAssistant::cameraJob
 * @param RcvDat
 */
void UsartAssistant::cameraJob(unsigned char *RcvDat)
{
    QString Str;
    ui->SerialRead_textEdit->insertPlainText( QString( "plot--->" ) );
    for( int Index = 0; Index<17; Index++ )   { // 数据转换
        ui->SerialRead_textEdit->insertPlainText( Str.setNum(RcvDat[Index], 16 ) );
    }
    ui->SerialRead_textEdit->insertPlainText( QString("\r\n") );
    switch( ReceivedData[2] )   {
        case 0x11:  TakePhotoForSeachYellowBlock();
    }
}

void UsartAssistant::on_OpenImage_pushButton_clicked()  {
    //action();
    //TakePhotoForSeachYellowBlock();
    ShowCameraPhoto();
}

void UsartAssistant::ShowCameraPhoto()  {
    int nWidhth = ui->PhotoShow_label->width();
    int nHeight = ui->PhotoShow_label->height();   // 获取图片控件的大小
    WId ViewHandle = ui->PhotoShow_label->winId(); // 获取图片空间的窗口ID

    // Local iconic variables
    HObject  ho_ImageFromCamera;

    // Local control variables
    HTuple  hv_AcqHandle;

    HTuple m_hWindowHandle;
    OpenWindow( 0, 0, nWidhth, nHeight, (Hlong)ViewHandle, "visible", "", &m_hWindowHandle );
    // Open camera on TaoTao'laptop...
//    OpenFramegrabber("DirectShow", 1, 1, 0, 0, 0, 0, "default", 8, "rgb", -1, "false",
//        "default", "[0] USB Camera", 0, -1, &hv_AcqHandle);
    // Open camera on Industrial computer...
    OpenFramegrabber("DirectShow", 1, 1, 0, 0, 0, 0, "default", 8, "rgb", -1, "false", "default", "[0] USB2.0 Camera", 0, -1, &hv_AcqHandle);
    GrabImageStart(hv_AcqHandle, -1);

    HTuple ImageWidth = nWidhth, ImageHeight = nHeight;
    WaitSeconds(1);
    GrabImageAsync(&ho_ImageFromCamera, hv_AcqHandle, -1);
        GetImageSize( ho_ImageFromCamera, &ImageWidth,&ImageHeight );
        SetPart( m_hWindowHandle, 0, 0, ImageHeight-1, ImageWidth-1 );
        DispObj( ho_ImageFromCamera, m_hWindowHandle );
    WaitSeconds(0.07);
    while( true )   {
    GrabImageAsync(&ho_ImageFromCamera, hv_AcqHandle, -1);

//    QImage image;

    HDevWindowStack::Push( m_hWindowHandle );
    if( HDevWindowStack::IsOpen() ) {
        DispObj( ho_ImageFromCamera, HDevWindowStack::GetActive() );
    }
    WaitSeconds(1);
    }
//    HTuple ImageWidth,ImageHeight;

//    GetImageSize( ho_ImageFromCamera, &ImageWidth,&ImageHeight );
//    SetPart( m_hWindowHandle, 0, 0, ImageHeight-1, ImageWidth-1 );
//    DispObj( ho_ImageFromCamera, m_hWindowHandle );
    //Image Acquisition 01: Do something
    CloseFramegrabber(hv_AcqHandle);
}

void UsartAssistant::ShowPictureOnUI(HObject show_Image)
{
    ui->PhotoShow_label->clear();
    int nWidhth = ui->PhotoShow_label->width();
    int nHeight = ui->PhotoShow_label->height();   // 获取图片控件的大小
    WId ViewHandle = ui->PhotoShow_label->winId(); // 获取图片空间的窗口ID

    HTuple m_hWindowHandle;
    //SetWindowAttr("background_color", "black");
    OpenWindow( 0, 0, nWidhth, nHeight, (Hlong)ViewHandle, "", "", &m_hWindowHandle );
//    HTuple ImageWidth,ImageHeight;
//    GetImageSize( show_Image, &ImageWidth,&ImageHeight );
//    SetPart( m_hWindowHandle, 0, 0, ImageHeight-1, ImageWidth-1 );
//    DispObj( show_Image, m_hWindowHandle );
    HDevWindowStack::Push( m_hWindowHandle );
    if( HDevWindowStack::IsOpen() ) {
        DispObj( show_Image, HDevWindowStack::GetActive() );
    }
}

void UsartAssistant::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_L )    {  //ctrl+l---->clear textedit
        ui->SerialRead_textEdit->clear();
        ui->SerialWrite_textEdit->clear();
    }
}

void UsartAssistant::TakePhotoForSeachYellowBlock() {

    // Local iconic variables
    HObject  ho_Image, ho_Rectangle, ho_ImageReduced;
    HObject  ho_RED, ho_GREEN, ho_BLUE, ho_H, ho_S, ho_V, ho_Regions;
    HObject  ho_ConnectedRegions, ho_SelectedRegions;

    // Local control variables
    HTuple  hv_AcqHandle, hv_Area, hv_Row, hv_Column;
    HTuple  hv_Index;

    ReadImage( &ho_Image, "D:/SCM/17Robot/PhotoApplication/Image1.png" );
    UsartAssistant::ShowPictureOnUI( ho_Image );
    qDebug() << "cat" << endl;

//    WaitSeconds(2);

    //图像裁剪
    GenRectangle1(&ho_Rectangle, 360, 205, 555, 1190);
    ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced);

//    if( HDevWindowStack::IsOpen() ) {
//        DispObj( ho_Image, HDevWindowStack::GetActive() );
//    }

    SendData[2] = 0x21;     // 2表示给下位机发送数据，1表示执行的是第一个job 查找黄色木块
    for( int i=3; i<15; i++ )   SendData[i] = (unsigned char)(i-2)<<4;  //   默认没有黄木块

    //转换成RGB通道
    Decompose3(ho_ImageReduced, &ho_RED, &ho_GREEN, &ho_BLUE);
    //转换成HSV通道
    TransFromRgb(ho_RED, ho_GREEN, ho_BLUE, &ho_H, &ho_S, &ho_V, "hsv");
    //阈值分割
    Threshold(ho_H, &ho_Regions, 27, 39);
    //连通性分析
    Connection(ho_Regions, &ho_ConnectedRegions);
    //特征提取（面积）
    SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 481.24, 974.67);
    //得到黄物块的坐标
    AreaCenter(ho_SelectedRegions, &hv_Area, &hv_Row, &hv_Column);
    HTuple end_val30 = (hv_Row.TupleLength())-1;
    HTuple step_val30 = 1;
    for (hv_Index=0; hv_Index.Continue(end_val30, step_val30); hv_Index += step_val30)
    {
        if( 0 != (HTuple(HTuple(HTuple(HTuple(hv_Row[hv_Index])>360).TupleAnd(HTuple(hv_Row[hv_Index])<555)).TupleAnd(HTuple(hv_Column[hv_Index])>205)).TupleAnd(HTuple(hv_Column[hv_Index])<1190)) )
        {
            if( 0 != ( HTuple(hv_Row[hv_Index]) > 462 ))    {
                if( 0 != ( HTuple(hv_Column[hv_Index]) < 360 ) )    {
                    qDebug() << 7;
                    SendData[9] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 540 ) )   {
                    qDebug() << 8;
                    SendData[10] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 730 ) )   {
                    qDebug() << 9;
                    SendData[11] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 910 ) )   {
                    qDebug() << 10;
                    SendData[12] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 1070 ) )   {
                    qDebug() << 11;
                    SendData[13] += 0x01;
                }
                else    {
                    qDebug() << 12;
                    SendData[14] += 0x01;
                }
            }
            else    {
                if( 0 != ( HTuple(hv_Column[hv_Index]) < 360 ) )    {
                    qDebug() << 1;
                    SendData[3] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 540 ) )   {
                    qDebug() << 2;
                    SendData[4] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 730 ) )   {
                    qDebug() << 3;
                    SendData[5] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 910 ) )   {
                    qDebug() << 4;
                    SendData[6] += 0x01;
                }
                else if( 0 != ( HTuple(hv_Column[hv_Index]) < 1070 ) )   {
                    qDebug() << 5;
                    SendData[7] += 0x01;
                }
                else    {
                    qDebug() << 6;
                    SendData[8] += 0x01;
                }
            }
        }
    }
    QString Str;
    for( int i=0; i<17; i++ )   {
        if( i == 9) ui->SerialWrite_textEdit->insertPlainText( QString("\r\n             ") );
        ui->SerialWrite_textEdit->insertPlainText( Str.setNum( SendData[i], 16 ) );
    }
    ui->SerialWrite_textEdit->insertPlainText( QString("\r\n") );
    serial.write( (char *) SendData, 17 );  // 发送数据，自动
}

void UsartAssistant::on_OpenCamera_pushButton_clicked()
{
    HObject ho_Image;
    ReadImage( &ho_Image, "D:/SCM/17Robot/PhotoApplication/7.tif" );
    DispObj( ho_Image, HDevWindowStack::GetActive() );
    //ui->PhotoShow_label->setNum(120);
//    // Local iconic variables
//    HObject  ho_ImageFromCamera;

//    // Local control variables
//    HTuple  hv_AcqHandle;

//    //Image Acquisition 01: Code generated by Image Acquisition 01
//    OpenFramegrabber("DirectShow", 1, 1, 0, 0, 0, 0, "default", 8, "rgb", -1, "false",
//        "default", "[0] USB Camera", 0, -1, &hv_AcqHandle);
}

void UsartAssistant::on_ScanSerialPort_pushButton_clicked() {
    ui->Com_comboBox->clear();
    UsartAssistant::initSeialPort();
}

void UsartAssistant::on_FindYellowBlock_pushButton_clicked()
{
    TakePhotoForSeachYellowBlock();
    qDebug() << "Yellow Block" ;
}

void UsartAssistant::on_SerialSendTest_pushButton_clicked()
{
    QString Str;
    SendData[2] = 0x11;
    for( int i=3; i<15; i++ )   SendData[i] = 0xFF;
    serial.write( (char *) SendData, 17 );
    for( int Index = 0; Index<17; Index++ )   { // 数据转换
        ui->SerialWrite_textEdit->insertPlainText( Str.setNum(SendData[Index], 16 ) );
    }
    ui->SerialWrite_textEdit->insertPlainText( QString("\r\n") );
}

void UsartAssistant::on_TakePhotos_pushButton_clicked()
{
    QDateTime timeNow = QDateTime::currentDateTime();//获取系统现在的时间
    QString timeNowStr = timeNow.toString("yyyy-MM-dd hh.mm.ss"); //设置显示格式
    timeNowStr = "C:/Users/HaDiSi/Desktop/TakePhotos/" + timeNowStr + ".png";
    ui->SerialWrite_textEdit->insertPlainText( timeNowStr );
    std::string str = timeNowStr.toStdString();

    // Local iconic variables
    HObject  ho_Image;

    // Local control variables
    HTuple  hv_AcqHandle;
    HTuple fillName = str.c_str();

    //Image Acquisition 01: Code generated by Image Acquisition 01
    OpenFramegrabber("DirectShow", 1, 1, 0, 0, 0, 0, "default", 8, "rgb", -1, "false",
          "default", "[0] USB2.0 Camera", 0, -1, &hv_AcqHandle);
    GrabImageStart(hv_AcqHandle, -1);
    WaitSeconds(1);
    GrabImageAsync(&ho_Image, hv_AcqHandle, -1);
    //Image Acquisition 01: Do something
    CloseFramegrabber(hv_AcqHandle);
    WriteImage(ho_Image, "png", 0, fillName );
}

void UsartAssistant::on_CloseCamera_pushButton_clicked()
{

}
