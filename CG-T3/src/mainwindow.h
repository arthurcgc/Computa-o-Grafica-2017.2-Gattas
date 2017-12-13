#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <math.h>
#include <QDir>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void on_actionRayTracer();
private:
    Ui::MainWindow *ui;
    QPixmap originalPixmap;
    void on_actionSalvar_imagem_triggered();
};

#endif // MAINWINDOW_H
