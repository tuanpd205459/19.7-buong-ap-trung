#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comserial.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pix("C:/Users/admin/Downloads/694px-Logo_Hust.png");
        if (!pix.isNull()) {
            ui->label_pix->setPixmap(pix.scaled(ui->label_pix->size(), Qt::KeepAspectRatio));
        }
        else {
            qDebug() << "Không thể tải hình ảnh";
        }


      devserial = new QSerialPort(this);

      /* Create Object the Class comserial to manipulate read/write of the my way */
      procSerial = new comserial(devserial);


          serialBuffer = "";
          parsed_data = "";
          temperature_value = "";
          humidity_value ="";


      QStringList DispSeriais = procSerial->ConnectDevices();
      ui->comboBoxPort->addItems(DispSeriais);


      if(DispSeriais.length() > 0) {
          ui->pushButtonConnect->setEnabled(true);
          ui->textEditGetData->append("###  Cổng nối tiếp đã sẵn sàng để sử dụng.");
      }
      else { ui->textEditGetData->append("### Không phát hiện thấy cổng nối tiếp!");
      }





      connect(devserial, SIGNAL(readyRead()), this, SLOT(readSerial()));


  }




MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::ReadData() {
    QByteArray data = devserial->readLine();
    QString receivedData = QString::fromUtf8(data); // Chuyển đổi từ QByteArray sang QString sử dụng mã hóa UTF-8

    qDebug() << "input: " << receivedData << endl;
    ui->textEditGetData->append(receivedData);

    qDebug() << "ReadData() is called when data is available.";

    //Kiểm tra dữ liệu từ cổng Serial
   qDebug() << "Serial data received: " << procSerial->Read();
   updateTemperature(receivedData);

}

void MainWindow::WriteData(const QByteArray data)
{
    procSerial->Write(data);
}

void MainWindow::on_pushButtonConnect_clicked()
{   bool statusOpenSerial;
    statusOpenSerial = procSerial->ConnectCom(ui->comboBoxPort->currentText());
    if(statusOpenSerial){
        ui->textEditGetData->append("### Kết nối thành công");
    }
    else {
        ui->textEditGetData->append("### Kết nối thất bại");
    }
}


void MainWindow::on_pushButtonDisconnect_clicked()
{

        bool statusCloseSerial;


        statusCloseSerial = procSerial->Disconnect();


        if (statusCloseSerial) {

            ui->textEditGetData->append("### Ngắt kết nối thành công!");
        }
        else {
            ui->textEditGetData->append("### Ngắt kết nối thất bại.");
        }

}

void MainWindow::updateTemperature(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberTemperature->display(sensor_reading);
}
void MainWindow::updateHumidity(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberHumidity->display(sensor_reading);
}
void MainWindow::dayHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberDayHatch->display(sensor_reading);
}
void MainWindow::hourHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberHourHatch->display(sensor_reading);
}
void MainWindow::minHatch(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdNumberMinHatch->display(sensor_reading);
}

/*
void MainWindow::readSerial()
{

    QStringList buffer_split = serialBuffer.split("\r\n"); //  split the serialBuffer string, parsing with '\r\n' as the separator

    if(buffer_split.length() < 3){

        serialData = devserial->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        qDebug() << serialBuffer;
        serialData.clear();
    }else{

        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        temperature_value = buffer_split[0];
        humidity_value = buffer_split[1];

        qDebug() << "Temperature: " <<  temperature_value << "\n";
        qDebug() << "Humidity: " <<  humidity_value << "\n";


        MainWindow::updateTemperature(temperature_value);
        MainWindow::updateHumidity(humidity_value);


    }

}

*/
void MainWindow::readSerial()
{

    QByteArray serialData = devserial->readAll(); // Đọc dữ liệu từ cổng serial
    serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
    qDebug() << "dữ liệu nhận được: " << serialBuffer;

    // Kiểm tra điều kiện kết thúc
    if (serialBuffer.endsWith("\r\n")) {
        // Xử lý dữ liệu đã đọc
        QStringList values = serialBuffer.split(',');

        if (values.size() >= 2) {
            QString temperature1 = values[0];
            QString humidity1 = values[1];

            if (values.size() >= 5) {
                QString value1 = values[2];
                QString value2 = values[3];
                QString value3 = values[4];

                if (value1.toInt() == 0 && value2.toInt() == 0 && value3.toInt() == 0) {
                        ui->textEditGetData->append("### Trứng đã nở...");
                       // isHatching = true; // Đánh dấu là dang ap
                }
                else {
                    if (isHatching == true) {
                        ui->textEditGetData->append("### Đang ấp...");
                        isHatching = false; // Đánh dấu là còn ấp
                    }
                }

                MainWindow::dayHatch(value1);
                MainWindow::hourHatch(value2);
                MainWindow::minHatch(value3);
            }

            MainWindow::updateTemperature(temperature1);
            MainWindow::updateHumidity(humidity1);
        }
        else {
            // Xử lý lỗi hoặc thông báo không đủ phần tử trong chuỗi đầu vào
            qDebug() << "Lỗi: Không đủ phần tử trong chuỗi đầu vào.";
        }

        // Xóa serialBuffer để chuẩn bị cho lần đọc tiếp theo
        serialBuffer.clear();
    }
}


void MainWindow::on_pushButtonStartHatch_clicked()
{
    procSerial->comserial::sendDataToPort(ui->spinBoxDayTimeHatch->value(),"dayTimeToHatch");
    procSerial->comserial::sendDataToPort(ui->spinBoxHourTimeHatch->value(),"hourTimeToHatch");
    procSerial->comserial::sendDataToPort(ui->spinBoxMinuteTimeHatch->value(),"minuteTimeToHatch");
    procSerial->comserial::sendDataToPort(1,"prState");
    ui->lcdNumberTotalDay->display(ui->spinBoxDayTimeHatch->value()) ;
    ui->lcdNumberTotalHour->display(ui->spinBoxHourTimeHatch->value()) ;
    ui->lcdNumberTotalMin->display(ui->spinBoxMinuteTimeHatch->value()) ;

}

void MainWindow::on_pushButtonStopHatch_clicked()
{
    procSerial->comserial::sendDataToPort(0,"prState");
}


void MainWindow::on_pushButtonContinue_clicked()
{
    procSerial->comserial::sendDataToPort(1,"prState");

}

void MainWindow::on_pushButtonSetTemp_clicked()
{
    int setTemperature = ui->spinBoxSetTemp->value(); // Lấy giá trị từ QSpinBox
    procSerial->comserial::sendDataToPort(setTemperature, "setTemp");
    ui->textEditGetData->append("### Nhiệt độ cài đặt: " + QString::number(setTemperature));
}



void MainWindow::on_pushButtonTurnOffBuzzer_clicked()
{
    procSerial->comserial::sendDataToPort(2,"TurnOffBuzzer");
}


void MainWindow::on_pushButtonReset_clicked()
{
    procSerial->comserial::sendDataToPort(0,"Reset");
        dayHatch(0);
        hourHatch(0);
        minHatch(0);
        ui->lcdNumberTotalDay->display(0) ;
        ui->lcdNumberTotalHour->display(0) ;
        ui->lcdNumberTotalMin->display(0) ;

}

