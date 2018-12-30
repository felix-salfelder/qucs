/***************************************************************************
                          dc_sim.cpp  -  description
                             -------------------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "command.h"
#include "qucs.h"
#include "globals.h"
#include "module.h"

namespace{

class DC_Sim : public Command  {
public:
  DC_Sim();
  ~DC_Sim();
  Element* clone() const{
	  return new DC_Sim(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
}D;
Dispatcher<Command>::INSTALL p(&command_dispatcher, ".DC", &D);
Module::INSTALL pp("simulations", &D);


DC_Sim::DC_Sim()
{
  Description = QObject::tr("dc simulation");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.lastIndexOf(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (a < 8 || s.length() - b < 8) b = -1;
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, s.left(b), Qt::darkBlue, QucsSettings.largeFontSize));
  if (b != -1)
    Texts.append(new Text(0, 0, s.mid(b+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  setName("DC");

  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("reltol", "0.001", false,
		QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("abstol", "1 pA", false,
		QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vntol", "1 uV", false,
		QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("saveOPs", "no", false,
		QObject::tr("put operating points into dataset")+
		" [yes, no]"));
  Props.append(new Property("MaxIter", "150", false,
		QObject::tr("maximum number of iterations until error")));
  Props.append(new Property("saveAll", "no", false,
	QObject::tr("save subcircuit nodes into dataset")+
	" [yes, no]"));
  Props.append(new Property("convHelper", "none", false,
	QObject::tr("preferred convergence algorithm")+
	" [none, gMinStepping, SteepestDescent, LineSearch, Attenuation, SourceStepping]"));
  Props.append(new Property("Solver", "CroutLU", false,
	QObject::tr("method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
}

DC_Sim::~DC_Sim()
{
}

Element* DC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc simulation");
  BitmapFile = (char *) "dc";

  if(getNewOne)  return new DC_Sim();
  return 0;
}

}
