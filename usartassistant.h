#ifndef USARTASSISTANT_H
#define USARTASSISTANT_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QKeyEvent>

#include "HalconCpp.h"
#include "HDevThread.h"

namespace Ui {
class UsartAssistant;
}

class UsartAssistant : public QWidget
{
    Q_OBJECT

public:
    explicit UsartAssistant(QWidget *parent = 0);
    ~UsartAssistant();
    QSerialPortInfo ComInfo;

    QSerialPort serial;//串口实例
    void initSeialPort();//初始化串口函数

    // TaoTao's Parameter...
    unsigned char ReceivedData[18] = {0};     //接受协议 17字节
    unsigned char SendData[18] = {0};
    int ReceivedDataCount = 0;

    // TaoTao's Function...
    void TakePhotoForSeachYellowBlock();    // 查找黄色木块...
    void ShowCameraPhoto();
    void ShowPictureOnUI( HalconCpp::HObject );

protected:
    void keyPressEvent(QKeyEvent *event);

    // TaoTao's Signal...
signals:
    void ReceivedData34( unsigned char* );

private slots:
    void cameraJob( unsigned char* RcvDat );
    void serialRead();

    void on_Com_comboBox_currentIndexChanged(const QString &arg1);
    void autoScroll();
    void on_OpenImage_pushButton_clicked();

    void on_OpenCamera_pushButton_clicked();

    void on_ScanSerialPort_pushButton_clicked();

    void on_FindYellowBlock_pushButton_clicked();

    void on_SerialSendTest_pushButton_clicked();

    void on_TakePhotos_pushButton_clicked();

    void on_CloseCamera_pushButton_clicked();

private:

    Ui::UsartAssistant *ui;


};

#endif // USARTASSISTANT_H
