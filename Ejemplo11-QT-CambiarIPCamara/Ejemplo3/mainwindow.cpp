#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mat2qimage.h"
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

#include<opencv2/core/core.hpp>
#include<opencv2/ml/ml.hpp>
#include<opencv/cv.h>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/video/background_segm.hpp>
#include<opencv2/videoio.hpp>
#include<opencv2/imgcodecs.hpp>
#include<qdebug.h>
#include<QTimer>

#include<opencv2/objdetect.hpp>

//variables para los botones de control manual
bool pres1 = false;
bool solt1 = true;

bool pres2 = false;
bool solt2 = true;

bool pres3 = false;
bool solt3 = true;

bool pres4 = false;
bool solt4 = true;

double area=0;
double area2=0;
bool stop = false ;
bool green_detect = false ;
QString face_cascade_name = "../pedestrian.xml";


//QString face_cascade_name = "../haarcascade_frontalface_alt2.xml";
//
//QString face_cascade_name = "../haarcascade_licence_plate_rus_16stages.xml";
QString eyes_cascade_name = "../haarcascade_eye_tree_eyeglasses.xml";
//

QSerialPort *arduino;
static const quint16 arduino_uno_vendor_id = 9025;
static const quint16 arduino_uno_product_id = 67;
//Bus 003 Device 003: ID 2341:0043 Arduino SA Uno R3 (CDC ACM)
//0x2341 = 2*16^3 + 3*16^2 + 4*16 + 1*16^0 = 9025
//0x43 = 4*16 + 3*16^0 = 67
QString arduino_port_name;
bool arduino_is_available;

//cv::VideoCapture camara(1);
int ciclos;
int i=0;
// int caso=0;
double numeros;
double numeros2;
double numeros3;
double numeros4;
QString cadnumeros;
QString cadnumeros2;
QString cadnumeros3;
QString cadnumeros4;

//cv::VideoCapture camara(1);

using namespace cv;

//Variables para la detección de objetos
CascadeClassifier traffic_cascade;
CascadeClassifier senal_cascade;


// Redireccionamiento
QString traffic_cascade_name = "../traffic_light.xml";
QString stop_cascade_name = "../stop_sign.xml";

QString dirCamara;
//cv::VideoCapture camara("http://192.168.43.243:8080/video");
cv::VideoCapture camara(600);

//HIIIIIIIIIIIIIIILOOOOOOOOOOOOOOOOOOOOOOO

hilo_auto::hilo_auto(QObject* parent):QThread(parent)
{
    qDebug() << "el hilo fue creado, fierro";
}

Mat IMAGEN;
Mat IMAGEN2;
Mat IMAGEN3;


void MainWindow::fTimer(){

    //ADQUISICIÓN DE IMAGEN
    //Mat IMAGEN(300,400,CV_8UC3,Scalar(150,255,25));

    if(camara.isOpened()){
    camara >> IMAGEN;
    cv::resize(IMAGEN,IMAGEN2,Size(400,300),0,0,0);


    QImage qImage = Mat2QImage(IMAGEN2);
QPixmap pixmap = QPixmap::fromImage(qImage);
    if (ui->tab_7->isVisible()){

    ui->label->clear();
    ui->label->setPixmap(pixmap);
   // caso = 1;

    }
    else if(ui->tab_8->isVisible()){
    ui->label_manual->clear();
    ui->label_manual->setPixmap(pixmap);
   // caso = 2;
    }
    else { //if(ui->tab_9->isVisible()){
        ui->label_automatico->clear();
        ui->label_automatico->setPixmap(pixmap);
      //  caso = 3;

    }
    }
    else{
        Mat inicial = cv::imread("../imagenes/ini.jpg");

        Mat inicialSmall;
        cv::resize(inicial,inicialSmall,Size(400,300),0,0,0);

        QImage qImage = Mat2QImage(inicialSmall);
        QPixmap pixmap = QPixmap::fromImage(qImage);

        if(ui->tab_7->isVisible()){


        ui->label->clear();
        ui->label->setPixmap(pixmap);


        }
        else if (ui->tab_8->isVisible()) {
        ui->label_manual->clear();
        ui->label_manual->setPixmap(pixmap);
        }
        else{ // if (ui->tab_9->isVisible()){
        ui->label_automatico->clear();
        ui->label_automatico->setPixmap(pixmap);
        }



    }

    // correr hilo aqui

    _thread->start();

}

void hilo_auto::run()
{
    qDebug() << "fierro con  el hilo";

    //this->msleep(10000);
// CheckForIllegalCrossThreadCalls=false;
    if(ui->tab_9->isVisible()){
                 if(ui->RBAutomatico->isChecked()){
                        if(!stop){
                     QString text = QString ("a");

                   if(arduino_is_available){
                       if(arduino->isWritable())
                           arduino->write(text.toStdString().c_str()  );

                       }
                    }

                   std::vector<Rect> luz;

                   Mat frame_gray;
                   cv::cvtColor( IMAGEN2, frame_gray, COLOR_BGR2GRAY );
                   cv::equalizeHist( frame_gray, frame_gray );


               //=======================PARA IDENTIFICAR LAS LUCES DE TRAFICO==================================
                   traffic_cascade.detectMultiScale( frame_gray, luz, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

                   for ( size_t i = 0; i < luz.size(); i++ )
                       {
                           //Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
                           //ellipse( IMAGEN2, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 255, 255 ), 4, 8, 0 );

                           Point upperLeftFace1(luz[i].x, luz[i].y);
                                   Point lowerRightFace1(luz[i].x+luz[i].width, luz[i].y+luz[i].height);
                                   rectangle(IMAGEN2, upperLeftFace1, lowerRightFace1, Scalar(255, 0,0), 1, 8);


                                                       Mat faceROI = IMAGEN2( luz[i] );//asigna a la varieble las matrices

                                                       QImage qImage1 = Mat2QImage(faceROI);
                                                       QPixmap pixmap1 = QPixmap::fromImage(qImage1);


                                                       //CHECAR VERDE
                                   Mat HSV1;//declara matriz
                                   cv::cvtColor(IMAGEN,HSV1,CV_BGR2HSV);//convierte la matriz de BGR a HSV
                                   //filtro
                                   Mat COLOR1;//declara matriz
                                   cv::inRange(HSV1,Scalar(54,225 ,168),Scalar(104,255,218),COLOR1);//aplica filtros

                                   Mat MATRIZBW1;//declara matriz
                                   COLOR1.copyTo(MATRIZBW1);//copea lo que dice matriz
                                   //Determinar los contornos para el primer filtro
                                   //Contornos de imagen color1
                                   std::vector<std::vector<cv::Point> > contornos2;
                                   std::vector<Vec4i> jerarquia2;
                                   cv::findContours( MATRIZBW1,
                                   contornos2,
                                   jerarquia2,
                                   CV_RETR_TREE,
                                   CV_CHAIN_APPROX_SIMPLE,
                                   Point(0,0));
                                   // int jj = 1;
                                   int cuantosContornos11 = jerarquia2.size();//asigna valor
                                   //cuenta contornos
                                   for(int y=0; y<cuantosContornos11; y++){
                                   qDebug() << "Puntos: " << contornos2[y].size()<< endl;
                                   double area = cv::contourArea(contornos2[y],false);
                                   qDebug() << "Area: " << area << endl;
                                   //CHECA SI EL AREA ENCONTRADA ES MAYOR A UN UMBRAL

                                   if ( area > 1){
                                   //sigues en autonomo
                                   stop = false;//asigna valor
                                   //SETEA VALOR DE ETIQUETAS "LABEL_6" Y "LABEL_8"
               ui->label_12->clear();
               ui->label_12->setText("verde");
               green_detect = true;
                                   }
               else{
                                   green_detect = false;
                                   }
                                   }


                                   //MUESTRA EL RESULTADO DE APLICAR EL FILTRO
               if(!green_detect){
                                   //CHECAR ROJO SON EXACTAMENTE LOS MISMOS PASOS QUE SE REALIZARON
                                   Mat HSV2;
                                   cv::cvtColor(IMAGEN,HSV2,CV_BGR2HSV);
                                   //filtro
                                   Mat COLOR2;
                                   cv::inRange(HSV2,Scalar(149,160 ,216),Scalar(199,210,255),COLOR2);//solo los

                                   Mat MATRIZBW2;
                                   COLOR2.copyTo(MATRIZBW2);
                                   //Determinar los contornos para el primer filtro
                                   //Contornos de imagen color1
                                   std::vector<std::vector<cv::Point> > contornos3;
                                   std::vector<Vec4i> jerarquia3;
                                   cv::findContours( MATRIZBW2,
                                   contornos3,
                                   jerarquia3,
                                   CV_RETR_TREE,
                                   CV_CHAIN_APPROX_SIMPLE,
                                   Point(0,0));
                                   // int j = 1;
                                   int cuantosContornos2 = jerarquia3.size();
                                   for(int y=0; y<cuantosContornos2; y++){
                                   // qDebug() << "Puntos: " << contornos3[y].size()<< endl;
                                   double area3 = cv::contourArea(contornos3[y],false);
                                   // qDebug() << "Area: " << area3 << endl;
                                   if (area3 > 1){
                                   stop = true;
                                   ui->label_12->clear();
                                   ui->label_12->setText("rojo");
                                   }


                                   }
                                   //MUESTRA LA IMAGEN CUANDO SE DETECTA UN SEMAFORO
               }

                                   }

                                   //termina detecceionde semaforo


               //=============================MANDAR ORDENES AL SERIAL

               //=====================

                   if(stop==true) {
                       QString text = QString ("s");

                     if(arduino_is_available){
                         if(arduino->isWritable())
                             arduino->write(text.toStdString().c_str()  );

                         }
                         else{
                             arduino_is_available=false;

                         }

                   }

               else{
                       QString text_1 = QString ("a");

                     if(arduino_is_available){
                         if(arduino->isWritable())
                             arduino->write(text_1.toStdString().c_str()  );
                         }
                         else{
                             arduino_is_available=false;

                         }

                   }

    }
                       else{
                           arduino_is_available=false;


                       }

    }
//Arduino
    if(ui->tab_7->isVisible()){
    if(i==0){
       i = (ciclos-1);
        if(!arduino_is_available){

            ui->label_Arduino->clear();
            ui->label_Arduino->setText("Arduino no esta conectado");

            foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
                if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
                    if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id){
                        if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
                             arduino_port_name = serialPortInfo.portName();
                             arduino->setPortName(arduino_port_name);
                             arduino->open(QIODevice::ReadWrite);
                             arduino->setDataBits(QSerialPort::Data8);
                             arduino->setBaudRate(QSerialPort::Baud115200);
                             arduino->setParity(QSerialPort::NoParity);
                             arduino->setStopBits(QSerialPort::OneStop);
                             arduino->setFlowControl(QSerialPort::NoFlowControl);

                             arduino_is_available = true;

                             ui->label_Arduino->clear();
                             ui->label_Arduino->setText("Arduino conectado!");

                        }
                        else{

                            ui->label_Arduino->clear();
                            ui->label_Arduino->setText("El Arduino no es un Arduino UNO-Valido");

                        }
                    }
                    else{

                        ui->label_Arduino->clear();
                        ui->label_Arduino->setText("El dispositivo no es un Arduino");

                    }
                }
                else {

                    ui->label_Arduino->clear();
                    ui->label_Arduino->setText("El dispositivo no tiene identificador");

                }
            }
        }
        else{

            ui->label_Arduino->clear();
            ui->label_Arduino->setText("Arduino conectado");

            }
        }
    else
    {
    i--;
    }
}
        //==========================Modo manual==============================

if(ui->tab_8->isVisible()){
    if (ui->RBManual->isChecked()){
        //COMANDOS MANUAL
        //===============================PARATE=====================================================

 QString text11 = QString ("s");
  if (!pres1 && solt1 && !pres2 && solt2 && !pres3 && solt3 && !pres4 && solt4){
    if(arduino_is_available){
        if(arduino->isWritable())

            arduino->write(text11.toStdString().c_str()  );
      }
    else{
        arduino_is_available=false;

    }

  }
  //===============================PARA IR A LA DERECHA=====================================================
 else if(pres1 && !solt1){
                QString text1 = QString ("z");

              if(arduino_is_available){
                  if(arduino->isWritable())
                      arduino->write(text1.toStdString().c_str()  );
                }
              else{
                  arduino_is_available=false;
              }
            }


        //====================================================================================================
        //===================PARA IR A LA IZQUIERDA==================================

            else if(pres2 && !solt2){
                 QString text2 = QString ("l");

               if(arduino_is_available){
                   if(arduino->isWritable())
                       arduino->write(text2.toStdString().c_str()  );
                 }
               else{
                   arduino_is_available=false;
               }
             }


        //========================================================

        //==========================PARA IR AVANZANDO================

             else if(pres3 && !solt3){
                  QString text3 = QString ("f");

                if(arduino_is_available){
                    if(arduino->isWritable())
                        arduino->write(text3.toStdString().c_str()  );
                  }
                else{
                    arduino_is_available=false;
                }
              }

        //==========================================



          //==============PARA IR DE REVERSA==============================

              else{// if(pres4 && !solt4){
                   QString text4 = QString ("r");

                 if(arduino_is_available){
                     if(arduino->isWritable())
                         arduino->write(text4.toStdString().c_str()  );
                   }
                 else{
                     arduino_is_available=false;
                 }
               }

    }
}

    qDebug() << "TERMINO EL HILO ALV COMPA";
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    //Creacion de la conexión con el microcontrolador
    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;
    int milis= 100;
    ciclos = 1/(milis*0.001);
    QTimer *cronometro=new QTimer(this);
    connect(cronometro, SIGNAL(timeout()), this, SLOT(fTimer()));
    cronometro->start(milis);
     connect(cronometro, SIGNAL(timeout()), this, SLOT(fTimer()));
    _thread = new hilo_auto(this);

  //  _thread = new MyThread(MainWindow);
    connect(_thread,SIGNAL(finished()),MainWindow,SLOT(finishThread()));

    connect(ui->push_n0,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n1,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n2,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n3,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n4,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n5,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n6,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n7,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n8,SIGNAL(released()),this,SLOT(teclado_pressed()));
    connect(ui->push_n9,SIGNAL(released()),this,SLOT(teclado_pressed()));



    Mat inicial1 = cv::imread("../imagenes/Cucei.jpg");
   Mat inicialSmall1;


   cv::resize(inicial1,inicialSmall1,Size(100,110),0,0,0);

   Mat inicial2 = cv::imread("../imagenes/udg-02.jpg");
   Mat inicialSmall2;

   cv::resize(inicial1,inicialSmall1,Size(100,110),0,0,0);

   cv::resize(inicial2,inicialSmall2,Size(70,90),0,0,0);


    QImage qImage1 = Mat2QImage(inicialSmall1);
QPixmap pixmap1 = QPixmap::fromImage(qImage1);

QImage qImage2 = Mat2QImage(inicialSmall2);
QPixmap pixmap2 = QPixmap::fromImage(qImage2);



ui->label_cucei->clear();
ui->label_cucei->setPixmap(pixmap1);

ui->label_udg->clear();
ui->label_udg->setPixmap(pixmap2);



    if( !traffic_cascade.load( traffic_cascade_name.toUtf8().constData() ) ){ qDebug() << "Error al cargar el detector de semaforos"; }


}

MainWindow::~MainWindow()
{
    delete ui;
}


//=========================================Introducción de la clave IP===============================

void MainWindow::teclado_pressed(){
    QPushButton * button = (QPushButton*)sender();


if(ui->radioButton->isChecked()){
numeros=(ui->labelnumeros->text()+button->text()).toDouble();

cadnumeros =QString::number(numeros,'g',3);

        ui->labelnumeros->setText(cadnumeros);
}

else if(ui->radioButton_2->isChecked()){
numeros2=(ui->labelnumeros_2->text()+button->text()).toDouble();

cadnumeros2 =QString::number(numeros2,'g',3);

        ui->labelnumeros_2->setText(cadnumeros2);
}

else if(ui->radioButton_5->isChecked()){
numeros3=(ui->labelnumeros_3->text()+button->text()).toDouble();

cadnumeros3 =QString::number(numeros3,'g',3);

        ui->labelnumeros_3->setText(cadnumeros3);
}

else if(ui->radioButton_6->isChecked()){
numeros4=(ui->labelnumeros_4->text()+button->text()).toDouble();

cadnumeros4 =QString::number(numeros4,'g',3);

ui->labelnumeros_4->clear();
        ui->labelnumeros_4->setText(cadnumeros4);
}



}

void MainWindow::on_pushButton_clicked()
{
    QString comando = "http://" +cadnumeros+"."+cadnumeros2+"." +cadnumeros3 +"." +cadnumeros4+":8080/video";
    QString camaraDIR = comando;
    if(!camaraDIR.isEmpty()){

        camara.open(camaraDIR.toUtf8().constData());

    }


}


void MainWindow::on_push_punto_2_clicked()
{


    if(ui->radioButton->isChecked()){
        ui->labelnumeros->clear();

    ui->labelnumeros->setText(0);


    }
    else if(ui->radioButton_2->isChecked()){
       ui->labelnumeros_2->clear();

               ui->labelnumeros_2->setText(0);

    }

    else if(ui->radioButton_5->isChecked()){
                    ui->labelnumeros_3->clear();



                    ui->labelnumeros_3->setText(0);


    }
    else if(ui->radioButton_6->isChecked()){
                        ui->labelnumeros_4->clear();

                                  ui->labelnumeros_4->setText(0);
    }



}


//PRESIONAR BOTON DERECHO
void MainWindow::on_Bderecha_pressed()
{
  if (ui->RBManual->isChecked()){

  pres1 = true;
   solt1 = false;

}
}




//SOLTAR BOTON DERECHA

void MainWindow::on_Bderecha_released()
{
  if (ui->RBManual->isChecked()){

  pres1 = false;
   solt1 = true;

}
}


//PRESIONAR BOTON IZQUIERDA


void MainWindow::on_Bizquierda_pressed()
{
  if (ui->RBManual->isChecked()){
    pres2 = true;
   solt2 = false;
}
}


//SOLTAR BOTON IZQUIERDA

void MainWindow::on_Bizquierda_released()
{
  if (ui->RBManual->isChecked()){

  pres2 = false;
   solt2 = true;
}
}

//PRESIONAR BOTON ADELANTE
void MainWindow::on_Badelante_pressed()
{
  if (ui->RBManual->isChecked()){

  pres3 = true;
   solt3 = false;
}
}

//SOLTAR BOTON ADELANTE
void MainWindow::on_Badelante_released()
{
  if (ui->RBManual->isChecked()){


  pres3 = false;
   solt3 = true;


}
}


// PRESIONAR BOTON ATRAS
void MainWindow::on_Batras_pressed()
{
  if (ui->RBManual->isChecked()){

   pres4 = true;
   solt4 = false;

}
}

//SOLTAR BOTON ATRAS

void MainWindow::on_Batras_released()
{
  if (ui->RBManual->isChecked()){

  pres4 = false;
   solt4 = true;

}
}
