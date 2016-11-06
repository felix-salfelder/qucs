/***************************************************************************
                                symbolwidget.h
                               ----------------
    begin                : Sat May 29 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYMBOLWIDGET_H
#define SYMBOLWIDGET_H

#include <QWidget>
#include <QSize>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QString>
#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QMessageBox>

#include "qucslib_common.h"
#include "libcomp.h"
#include "./../qucs/symbol.h"
/*!
 * \file symbolwidget.h
 * \brief Definition of the SymbolWidget class.
 */


class Node;
class QPaintEvent;
class QSizePolicy;
class QucsLibComponent;

class SymbolWidget : public QWidget  {
   Q_OBJECT
public:
  SymbolWidget(QWidget *parent = 0);
 ~SymbolWidget();


  // what does this do? there is no symbol here...
  int createSymbol(const QString&, const QString&);

  // component properties

  unsigned DragNDropWidth, TextHeight, TextWidth;

public:
  // "attach" means "transfer ownership"
  // we could do more stuff here, such as check bounding boxes
  // (maybe we should)
  void attachSymbol(Symbol const* s);

private: // implementation
  void AdjustWidgetSize(Symbol const& s);

public: // symbol-thru-access. slightly hackish
        // obsolete, maybe (after cleanup).
		  // lets be explicit, for now
  QString theModel() const;
  QString modelString() const;
  QString verilogModelString() const;
  QString vHDLModelString() const;

protected:
  void mouseMoveEvent(QMouseEvent*);

private:
  void  paintEvent(QPaintEvent*);

  QString PaintText;
  QString DragNDropText;
  QString Warning;

  // this is actually a Symbol, but with some headache.
  QucsLibComponent const* symbol;
};

#endif // SYMBOLWIDGET_H
