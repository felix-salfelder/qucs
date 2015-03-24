/*
 * imagewriter.cpp - implementation of writer to image
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "schematic.h"
#include "imagewriter.h"
#include "dialogs/exportdialog.h"

#include <QtSvg>

ImageWriter::ImageWriter(QString lastfile)
{
  onlyDiagram = false;
  lastExportFilename = lastfile;
}

ImageWriter::~ImageWriter()
{
}

void
ImageWriter::noGuiPrint(QWidget *doc, QString printFile, QString color)
{
  Schematic *sch = static_cast<Schematic*>(doc);
  const int bourder = 30;
  int w,h,wsel,hsel,
      xmin, ymin, xmin_sel, ymin_sel;

  sch->getSchWidthAndHeight(w, h, xmin, ymin);
  sch->getSelAreaWidthAndHeight(wsel, hsel, xmin_sel, ymin_sel);
  w += bourder;
  h += bourder;
  wsel += bourder;
  hsel += bourder;
  float scal = 1.0;

  if (printFile.endsWith(".svg") || printFile.endsWith(".eps")) {
    QSvgGenerator* svg1 = new QSvgGenerator();

    QString tempfile = printFile + ".tmp.svg";
    if (!printFile.endsWith(".svg")) {
        svg1->setFileName(tempfile);
    } else {
        svg1->setFileName(printFile);
    }

    svg1->setSize(QSize(1.12*w, h));
    QPainter *p = new QPainter(svg1);
    p->fillRect(0, 0, svg1->size().width(), svg1->size().height(), Qt::white);
    ViewPainter *vp = new ViewPainter(p);
    vp->init(p, 1.0, 0, 0, xmin-bourder/2, ymin-bourder/2, 1.0, 1.0);

    sch->paintSchToViewpainter(vp,true,true);

    delete vp;
    delete p;
    delete svg1;

    if (!printFile.endsWith(".svg")) {
        QString cmd = "inkscape -z -D --file=";
        cmd += tempfile + " ";

        if (printFile.endsWith(".eps")) {
          cmd += "--export-eps=" + printFile;
        }

        int result = QProcess::execute(cmd);

        if (result!=0) {
            QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,"Export to image", "Inkscape start error!", QMessageBox::Ok);
            msg->exec();
            delete msg;
        }
        QFile::remove(tempfile);
    }

  } else if (printFile.endsWith(".png")) {
    QImage* img;
    if (color == "BW") {
      img = new QImage(w, h, QImage::Format_Mono);
    } else {
      img = new QImage(w, h, QImage::Format_RGB888);
    }

    QPainter* p = new QPainter(img);
    p->fillRect(0, 0, w, h, Qt::white);
    ViewPainter* vp = new ViewPainter(p);
    vp->init(p, scal, 0, 0, xmin*scal-bourder/2, ymin*scal-bourder/2, scal,scal);

    sch->paintSchToViewpainter(vp,true,true);

    img->save(printFile);

    delete vp;
    delete p;
    delete img;
  } else {
    fprintf(stderr, "Unsupported format of output file. \n"
        "Use PNG, SVG or PDF format!\n");
    return;
  }
}

QString ImageWriter::getLastSavedFile()
{
    return lastExportFilename;
}

void
ImageWriter::print(QWidget *doc)
{
  Schematic *sch = static_cast<Schematic*>(doc);
  const int border = 30;

  int w,h,wsel,hsel,
      xmin, ymin, xmin_sel, ymin_sel;

  sch->getSchWidthAndHeight(w, h, xmin, ymin);
  sch->getSelAreaWidthAndHeight(wsel, hsel, xmin_sel, ymin_sel);
  w += border;
  h += border;
  wsel += border;
  hsel += border;

  bool noselect = false;
  if ((wsel == (border+1)) && (hsel == (border+1))) {
    noselect = true;
  }

  ExportDialog *dlg = new ExportDialog(
      w, h, wsel, hsel, lastExportFilename, noselect, 0);
  
  if (onlyDiagram) {
    dlg->setDiagram();
  }

  if (dlg->exec()) {
    QString filename = dlg->FileToSave();
    lastExportFilename = filename;

    bool exportAll;
    if (dlg->isExportSelected()) {
      exportAll = false;
      w = wsel;
      h = hsel;
      xmin = xmin_sel;
      ymin = ymin_sel;
    } else {
      exportAll = true;
    }

    float scal;
    if (dlg->isOriginalSize()) {
      scal = 1.0;
    } else {
      scal = (float) dlg->Xpixels()/w;
      w = round(w*scal);
      h = round(h*scal);
    }

    if (dlg->isValidFilename()) {
      if (!dlg->isSvg()) {
        QImage* img;

        switch (dlg->getImgFormat()) {
          case ExportDialog::Coloured : 
            img = new QImage(w,h,QImage::Format_RGB888);
            break;
          case ExportDialog::Monochrome : 
            img = new QImage(w,h,QImage::Format_Mono);
            break;
          default : 
            break;
        }

        QPainter* p = new QPainter(img);
        p->fillRect(0, 0, w, h, Qt::white);
        ViewPainter* vp = new ViewPainter(p);
        vp->init(p, scal, 0, 0, 
            xmin*scal-border/2, ymin*scal-border/2, scal, scal);

        sch->paintSchToViewpainter(vp, exportAll, true);

        img->save(filename);

        delete vp;
        delete p;
        delete img;
      } 
      else {
        QSvgGenerator* svgwriter = new QSvgGenerator();

        if (dlg->needsInkscape()) {
          svgwriter->setFileName(filename+".tmp.svg");
        } else {
          svgwriter->setFileName(filename);
        }

        //svgwriter->setSize(QSize(1.12*w,1.1*h));
        svgwriter->setSize(QSize(1.12*w,h));
        QPainter *p = new QPainter(svgwriter);
        p->fillRect(0, 0, svgwriter->size().width(), svgwriter->size().height(), Qt::white);

        ViewPainter *vp = new ViewPainter(p);
        vp->init(p, 1.0, 0, 0, xmin-border/2, ymin-border/2, 1.0, 1.0);
        sch->paintSchToViewpainter(vp,exportAll,true);

        delete vp;
        delete p;
        delete svgwriter;

        if (dlg->needsInkscape()) {
            QString cmd = "inkscape -z -D --file=";
            cmd += filename+".tmp.svg ";

            if (dlg->isPdf_Tex()) {
                QString tmp = filename;
                tmp.chop(4);
                cmd = cmd + "--export-pdf="+ tmp + " --export-latex";
            }

            if (dlg->isPdf()) {
                cmd = cmd + "--export-pdf=" + filename;
            }

            if (dlg->isEps()) {
                cmd = cmd + "--export-eps=" + filename;
            }

            int result = QProcess::execute(cmd);

            if (result!=0) {
                QMessageBox::critical(0, QObject::tr("Export to image"), 
                    QObject::tr("Inkscape start error!"), QMessageBox::Ok);
            }
            QFile::remove(filename+".tmp.svg");
        }
      }

      if (QFile::exists(filename)) {
        QMessageBox::information(0, QObject::tr("Export to image"),
            QObject::tr("Successfully exported"), QMessageBox::Ok);
      } 
      else {
        QMessageBox::information(0, QObject::tr("Export to image"),
            QObject::tr("Disk write error!"), QMessageBox::Ok);
      }
    } else {
        QMessageBox::critical(0, QObject::tr("Export to image"), 
            QObject::tr("Unsupported format of graphics file. \n"
            "Use PNG, JPEG or SVG graphics!"), QMessageBox::Ok);
    }
  }
  delete dlg;
}
