#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE


// klasa za merenje udaljenosti i paljenje dioda
class sensorMeasure: public QObject{
    Q_OBJECT
    public:
        bool running = false; // da li merenje aktivno ili ne

    public slots:
        void startMeasurment(); // funkcija za merenje i paljenje led
        void stopMeasurment();  // funckija za zaustavljanje i gasenje svih LED

    signals:
        void updateUI(double distance, QString ledStatus);
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:

    void on_StartMeasure_clicked();

    void on_StopMeasure_clicked();

    void updateLabels(double distance, QString ledStatus);

private:
    Ui::Dialog *ui;
    QThread *workerThread;// thred za merenje
    sensorMeasure *worker; // instanca klase
};
#endif // DIALOG_H
