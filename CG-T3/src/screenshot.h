#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QDir>
#include <QFileDialog>

class Screenshot : public QWidget
{
    Q_OBJECT

public:
    Screenshot();

private slots:
    void newScreenshot();
    void saveScreenshot();
    void shootScreen();

private:
    QPushButton *createButton(const QString &text, QWidget *receiver, const char *member);
    QPixmap originalPixmap;

    QPushButton *saveScreenshotButton;
};

#endif // SCREENSHOT_H
