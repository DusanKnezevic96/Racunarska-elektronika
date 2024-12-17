#include "dialog.h"
#include "ui_dialog.h"
#include "wiringPi.h"
#include "QMetaObject"
#include "QDebug"
#include "QString"
#include "QObject"

#define TRIG 7
#define ECHO 0
#define RED 2
#define YELLOW 3
#define GREEN 4

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    workerThread = new QThread(this);
    worker = new sensorMeasure;
    worker->moveToThread(workerThread);

    connect(worker,&sensorMeasure::updateUI,this,&Dialog::updateLabels);
    connect(workerThread,&QThread::finished,worker,&QObject::deleteLater);

}

Dialog::~Dialog()
{
    workerThread->quit();
    workerThread->wait();
    delete ui;
}


void sensorMeasure::startMeasurment()
{
    wiringPiSetup();
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(RED, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(GREEN, OUTPUT);

    // set TRIG pin to low
    digitalWrite(TRIG, LOW);

    while (running) {
        // sent TRIG pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(30);
        digitalWrite(TRIG, LOW);

        while (digitalRead(ECHO) == LOW);
        long startMeas = micros();
        while (digitalRead(ECHO) == HIGH);
        long endMeas = micros();

        double distance = (endMeas - startMeas)*0.0343/2;

        // set all LED to LOW
        digitalWrite(RED, LOW);
        digitalWrite(YELLOW, LOW);
        digitalWrite(GREEN, LOW);

        QString ledStatus;

        if (distance < 10){
            digitalWrite(RED, HIGH);
            digitalWrite(YELLOW, LOW);
            digitalWrite(GREEN, LOW);
            ledStatus = "Upaljena CRVENA dioda";
        }else if (distance > 10 && distance < 20){
            digitalWrite(YELLOW, HIGH);
            digitalWrite(RED, LOW);
            digitalWrite(GREEN, LOW);
            ledStatus = "Upaljena ZUTA dioda";
        }else {
            digitalWrite(GREEN, HIGH);
            digitalWrite(RED, LOW);
            digitalWrite(YELLOW, LOW);
            ledStatus = "Upaljena ZELENA dioda";
        }

        emit updateUI(distance, ledStatus);
        delay(500);
    }
}


void sensorMeasure::stopMeasurment(){
    running = false;

    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
}

void Dialog::updateLabels(double distance, QString ledStatus){

    ui->distanceLabel->setText(QString("Izmerena udaljenost: %1 cm").arg(distance));
    ui->ledLabel->setText(QString("LED status: %1").arg(ledStatus));
}

void Dialog::on_StartMeasure_clicked()
{
    worker->running = true;
    workerThread->start();
    QMetaObject::invokeMethod(worker, "startMeasurment", Qt::QueuedConnection);
    qDebug()<<"Merenje startovano";
}


void Dialog::on_StopMeasure_clicked()
{
    worker->stopMeasurment();
    workerThread->quit();
    workerThread->wait();
    qDebug()<<"Merenje zavrseno";
}


