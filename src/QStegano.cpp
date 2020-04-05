#include "QStegano.h"
#include <QFileDialog>
#include <QStyleFactory>
#include <QMessageBox>
#include <QClipboard>
#include "Common.h"
#include "LSB.h"
#include "LSBXor.h"
#include "LSBInverted.h"
#include "OPAP.h"
#include "ImageContext.h"
#include "EnvironmentProxy.h"
#include "MethodHandler.h"
#include "CImg.h"

#include <QDebug>

auto QtImageFormat = QImage::Format_RGBA8888;

QStegano::QStegano(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.swInput->setCurrentIndex(0);

    qDebug() << "Available styles: " << QStyleFactory::keys().join(" ");
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QColor darkGray(53, 53, 53);
    QColor gray(128, 128, 128);
    QColor black(25, 25, 25);
    QColor blue(42, 130, 218);

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, darkGray);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, black);
    darkPalette.setColor(QPalette::AlternateBase, darkGray);
    darkPalette.setColor(QPalette::ToolTipBase, blue);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, darkGray);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Link, blue);
    darkPalette.setColor(QPalette::Highlight, blue);
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    darkPalette.setColor(QPalette::Active, QPalette::Button, gray.darker());
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, gray);
    darkPalette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    
    connect(ui.pbSelectFile, &QPushButton::clicked, this, &QStegano::loadImage);
    connect(ui.pbSelectNewFile, &QPushButton::clicked, this, &QStegano::loadImage);

    connect(ui.cbType, qOverload<int>(&QComboBox::currentIndexChanged), ui.swType, &QStackedWidget::setCurrentIndex);

    connect(ui.pbHide, &QPushButton::clicked, this, &QStegano::hide);
    connect(ui.pbDecode, &QPushButton::clicked, this, &QStegano::decode);

    connect(ui.pbSave, &QPushButton::clicked, this, [this]
    {
       auto file = QFileDialog::getSaveFileName(this, tr("Save file"), "./");
       QImage out(mOutput.data(), mOutput.width(), mOutput.height(), QtImageFormat);

       QFileInfo fi{ file };
       if (fi.suffix().isEmpty())
          file += ".png";

       out.save(file);
    });

    connect(ui.teInput, &QTextEdit::textChanged, this, [this]
    {
       ui.lblText->setText(QString::number(ui.teInput->toPlainText().length()));
    });

   connect(ui.pbCopyDecoded, &QPushButton::clicked, this, [this]
   {
      auto clipboard = QGuiApplication::clipboard();
      clipboard->setText(ui.teOutput->toPlainText());
   });
}

void QStegano::loadImage()
{
   QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"), "./");
   if (!fileNames.isEmpty())
   {
      ui.swInput->setCurrentWidget(ui.pPreview);
      mInputFile = fileNames.first();

      QPixmap preview{ fileNames.first() };
      ui.imageLabel->setPixmap(preview.scaled(ui.imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

      QFileInfo fi{ mInputFile };
      ui.imageName->setText(fi.fileName());

      QImage src{ mInputFile };
      ui.lblSize->setText(tr("Image size: %1 x %2").arg(src.width()).arg(src.height()));
   }
   else if(mInputFile.isEmpty())
   {
      ui.swInput->setCurrentWidget(ui.pSelect);
   }

   ui.pbSave->setEnabled(false);
   ui.pbCopyDecoded->setEnabled(false);
}

void QStegano::hide()
{
   cimg::exception_mode(0); //quiet mode

   std::string method = ui.cbMethod->currentText().toStdString();
   auto usedBits = ui.cbBits->currentText().toInt();

   try
   {
      QImage src{ mInputFile };
      src.convertTo(QtImageFormat);
      CImg<unsigned char> source(src.bits(), src.width(), src.height(), 1, src.depth() / 8);

      ImageContext context(source);
      mOutput = source;

      double timeSpent = 0;
      std::string secretMessage;

      secretMessage = ui.teInput->toPlainText().toStdString();

      if ("basic" == method)
      {
         timeSpent = context.hideUsingBasicLsb(secretMessage, usedBits);
         mOutput = context.getBasicLsb();
      }
      else if ("xor" == method)
      {
         timeSpent = context.hideUsingLsbXor(secretMessage);
         mOutput = context.getXorLsb();
      }
      else if ("inverted" == method)
      {
         timeSpent = context.hideUsingLsbInverted(secretMessage);
         mOutput = context.getInvertedLsb();
      }
      else if ("opap" == method)
      {
         timeSpent = context.hideUsingOpap(secretMessage, usedBits);
         mOutput = context.getOpap();
      }

      ui.pbSave->setEnabled(true);
      QMessageBox::information(this, tr("Success"), tr("Successfully hidden the text in the image!\nTime spent: %1us\nPixels changed: %2\n").arg(timeSpent).arg(context.getNumberOfDifferentPixels(mOutput)));

      qDebug() << "{{Image size: " << source.width() << " x " << source.height() << "}}";// << std::endl;
      qDebug() << "{{Channels: " << source.spectrum() << "}}";// << std::endl;
      qDebug() << "{{Secret message: " << QString::fromStdString(secretMessage) << "}}";// << std::endl;
      qDebug() << "{{Method: " << QString::fromStdString(method) << "}}";//<< std::endl;
      qDebug() << "{{Used bits: " << usedBits << "}}";// << std::endl;
      qDebug() << "{{Time spent hiding: " << timeSpent << " us" << "}}";// << std::endl;
      qDebug() << "{{Pixels changed: " << context.getNumberOfDifferentPixels(mOutput) << "}}";// << std::endl;
      qDebug() << "{{Mean Square Error (MSE): " << context.getMSE(source, mOutput) << "}}";//<< std::endl;
      qDebug() << "{{Peak Signal to Noise Ratio (PSNR): " << context.getPSNR(source, mOutput) << " dB" << "}}";//<< std::endl;
   }
   catch (CImgIOException & e)
   {
      std::cout << e.what() << "Please provide correct image file!\n";
   }
}

void QStegano::decode()
{
   cimg::exception_mode(0); //quiet mode

   try
   {
      QImage src{ mInputFile };
      src.convertTo(QtImageFormat);
      CImg<unsigned char> source(src.bits(), src.width(), src.height(), 1, src.depth() / 8);

      ImageContext context(source);

      std::string secretMessage = context.decode(source);
      auto methodType = MethodHandler::getMethodType(source);
      QString method;
      auto usedBits = 0;
      switch (methodType)
      {
      case MethodHandler::TYPE_BASIC_LSB:     method = "Basic LSB";    usedBits = MethodHandler::getUsedBits(source); break;
      case MethodHandler::TYPE_INVERTED_LSB:  method = "Inverted LSB"; usedBits = 1;                                  break;
      case MethodHandler::TYPE_XOR_LSB:       method = "XOR LSB";      usedBits = 1;                                  break;
      case MethodHandler::TYPE_OPAP:          method = "OPAP";         usedBits = MethodHandler::getUsedBits(source); break;
      default:                                method = "Unknown";
      }

      if(secretMessage != "CRC check failed!!!")
      {
         QMessageBox::information(this, tr("Success"), tr("Successfully decoded the text from the image!\n"));
         ui.teOutput->setText(QString::fromStdString(secretMessage));
         ui.lblDecodedMethod->setText(method);
         ui.lblDecodedBits->setText(QString::number(usedBits));
         ui.pbCopyDecoded->setEnabled(true);
      }
      else
      {
         QMessageBox::warning(this, tr("Error"), tr("Text decoding failed!\n"));
      }

      qDebug() << "{{Image size: " << source.width() << "x" << source.height() << "}}";// << std::endl;
      qDebug() << "{{Channels: " << source.spectrum() << "}}";// << std::endl;
      qDebug() << "{{Secret message: " << QString::fromStdString(secretMessage) << "}}";// << std::endl;
      qDebug() << "{{Method: " << method << "}}";// << std::endl;
      qDebug() << "{{Used bits: " << usedBits << "}}";// << std::endl;
   }
   catch (CImgIOException & e)
   {
      std::cout << e.what() << "Please provide correct image file!\n";
   }
}