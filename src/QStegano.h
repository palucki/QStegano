#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QStegano.h"
#include <CImg.h>

class QStegano : public QMainWindow
{
    Q_OBJECT

public:
    QStegano(QWidget *parent = Q_NULLPTR);

private:
    void loadImage();
    void hide();
    void decode();

    QString mInputFile;
    cimg_library::CImg<unsigned char> mOutput;
    Ui::QStegano ui;
};
