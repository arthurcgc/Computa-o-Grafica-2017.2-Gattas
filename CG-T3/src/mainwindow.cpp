#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::on_actionRayTracer(){
  //RayTracer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSalvar_imagem_triggered()
{
    QImage img(RenderWidget.size());
    QPainter painter(&img);
    RenderWidget->render(&painter);
    img.save("/some/file.jpg");
    /*originalPixmap = QPixmap::grabWidget(window());
    QString format = "png";
    QString initialPath = QDir::currentPath() + tr("/untitled.") + format;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), initialPath,
                                                    tr("%1 Files (*.%2);;All Files (*)")
                                                    .arg(format.toUpper())
                                                    .arg(format));
    if (!fileName.isEmpty())
        originalPixmap.save(fileName, format.toLatin1().constData());*/
}

