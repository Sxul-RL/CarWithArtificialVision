#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class hilo_auto; // declaracion adelantada de MyThread

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void fTimer();
    void finishThread();
private slots:
    void on_pushButton_clicked();
      void teclado_pressed();

      void on_Bderecha_pressed();
      void on_Bderecha_released();
      void on_Bizquierda_pressed();
      void on_Bizquierda_released();

      void on_Badelante_pressed();

      void on_Badelante_released();


      void on_Batras_pressed();
      void on_Batras_released();


      void on_push_punto_2_clicked();
private:
    Ui::MainWindow *ui;
    hilo_auto* _thread;
};

#endif // MAINWINDOW_H

#ifndef HILO_AUTO_H
#define HILO_AUTO_H
#include <QThread>
class hilo_auto : public QThread
{
public:
    hilo_auto(QObject*);
    void run();
};
#endif // MYTHREAD_H

