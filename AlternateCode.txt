

/********************备用代码区*******************

// 下位机使用printf("%X", ch );
void UsartAssistant::serialRead()
{
    //QByteArray BufferAll = serial.readAll();
    QString Str;
    static QByteArray BufferAll;
    BufferAll.append( serial.readAll() );
    if( BufferAll.isNull() == false ) {
        ReceivedDataCount = BufferAll.length();
        ui->SerialRead_textEdit->insertPlainText(QString(BufferAll));
        ui->SerialRead_textEdit->insertPlainText( QString("->    ") );
        ui->SerialRead_textEdit->insertPlainText( Str.setNum(ReceivedDataCount) );
        ui->SerialRead_textEdit->insertPlainText( QString("\r\n") );
    }
    if( ReceivedDataCount >= 34 )   {   //接受到17位数据
        //add your funciton...  QStringLiteral("中文")
        ui->SerialRead_textEdit->insertPlainText( QStringLiteral("接收-->  ") );
        for( int Index = 0; Index<17; Index++ )   { // 数据转换
            char byteH = BufferAll.at(2*Index);
            char byteL = BufferAll.at(2*Index+1);

            if( byteH>='A' && byteH<='F' )  byteH = byteH - 'A' + 10;
            else    byteH = byteH - '0';
            if( byteL>='A' && byteL<='F' )  byteL = byteL - 'A' + 10;
            else    byteL = byteL - '0';

            ReceivedData[Index] = ( byteH<<4 ) + byteL;
            qDebug( "%X", ReceivedData[Index] );
            ui->SerialRead_textEdit->insertPlainText( Str.setNum(ReceivedData[Index], 16 ) );

        }
        ui->SerialRead_textEdit->insertPlainText( QString("\r\n") );
        ReceivedData[17] = '\0';

        if( ReceivedData[0] == 0x34 && ReceivedData[1] == 0x56 && ReceivedData[15] == (~0x34) && ReceivedData[16] == (~0x56) )
            emit UsartAssistant::cameraJob( ReceivedData );
        else    {               // 错误接收 请重新发送...
            SendData[2] = 0x33;
            for( int i=3; i<15; i++ )   SendData[i] = 0x00; // clear
            serial.write( (char *) SendData, 17 );
        }
        BufferAll.clear();
        ReceivedDataCount = 0;
    }
}

    //图片显示
//    HImage image = *show_Image;       // Reading an image from a file
//    HTuple width, height;
//    HTuple hv_WindowID;
////    image.GetImageSize( &width, &height );
////    HWindow w( 0, 0, width, height );
////    w.DispImage( image );
////    w.Click();
//    //w.ClearWindow();

//    GetImageSize( *show_Image, &width, &height );
//    SetWindowAttr( "background_color", "black" );
//    Hlong winID = (Hlong) photoWindow->winId();
//    OpenWindow( 0, 0, width, height, winID, "", "", &hv_WindowID);
//    HDevWindowStack::Push( hv_WindowID );
//    if( HDevWindowStack::IsOpen() ) {
//        DispObj( *show_Image, HDevWindowStack::GetActive() );
//    }
//    photoWindow->resize( QSize( (int)width, (int)height ) );
//    photoWindow->show();

