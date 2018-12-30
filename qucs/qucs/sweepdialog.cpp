/***************************************************************************
                               sweepdialog.cpp
                              -----------------
    begin                : Sat Aug 13 2005
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
#include "sweepdialog.h"
#include "schematic.h"
#include "qucs.h"

#include <QGridLayout>
#include "diagrams/graph.h"
#include "misc.h"

#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QDebug>

// SpinBoxes are used to show the calculated bias points at the given set of sweep points
mySpinBox::mySpinBox(int Min, int Max, int Step, double *Val, QWidget *Parent)
          : QSpinBox(Parent)
{
  setMinimum(Min);
  setMaximum(Max);
  setSingleStep(Step); 
  Values = Val;
  ValueSize = Max;
  //editor()->
	//	setReadOnly(true);
}


#include <iostream>
using namespace std;
QString mySpinBox::textFromValue(int Val) const
{
  if (Values == NULL) return "";

  qDebug() << "Values + Val" << *(Values+Val) << endl;
  return QString::number(*(Values+Val));
}

QValidator::State mySpinBox::validate ( QString & text, int & pos ) const
{
  if(pos>ValueSize)return QValidator::Invalid; 
  if(QString::number(*(Values+pos))==text)
  return QValidator::Acceptable;
  else return QValidator::Invalid;
}

///
/// \brief SweepDialog::SweepDialog
/// \param Doc_
/// This dialog is launched when the Calculate DC bias is requested
/// for a project which contains a sweep.
SweepDialog::SweepDialog(Schematic *Doc_)
			: QDialog(Doc_)
{
  qDebug() << "SweepDialog::SweepDialog()";

  Doc = Doc_;

  pGraph = setBiasPoints();
  // if simulation has no sweeps, terminate dialog before showing it
  if(!pGraph->numAxes()) {
    reject();
    return;
  }
  if(pGraph->numAxes() <= 1)
    if(pGraph->axis(0)->count <= 1) {
      reject();
      return;
    }


  setWindowTitle(tr("Bias Points"));

  // ...........................................................
  QGridLayout *all = new QGridLayout(this);//, pGraph->cPointsX.count()+2,2,3,3);
  all->setMargin(5);
  all->setSpacing(5);
  all->setColumnStretch(1,5);

  DataX const *pD;
  mySpinBox *Box;
  
  for(unsigned ii=0; (pD=pGraph->axis(ii)); ++ii) {
    all->addWidget(new QLabel(pD->Var, this), 0, 0);
    //cout<<"count: "<<pD->count-1<<", points: "<<*pD->Points<<endl;
    //works only for linear:
    /*double Min = pD->Points[0];
    double Max = pD->Points[pD->count-1];
    double Step = (Max-Min)/(pD->count);
    cout<<"Min: "<<Min<<", Max: "<<Max<<", Step: "<<Step<<endl;
    Box = new mySpinBox(Min, Max, Step, pD->Points, this);*/
    Box = new mySpinBox(0, pD->count-1, 1, pD->Points, this);
    Box->setValue(0);  
    all->addWidget(Box, 0, 1);
    connect(Box, SIGNAL(valueChanged(int)), SLOT(slotNewValue(int)));
    BoxList.append(Box);
  }

  // ...........................................................
  all->setRowStretch(0,5);
  QPushButton *ButtClose = new QPushButton(tr("Close"), this);
  all->addWidget(ButtClose, 1, 0, 1, 5);
  connect(ButtClose, SIGNAL(clicked()), SLOT(accept()));
  show();
}

SweepDialog::~SweepDialog()
{
  delete pGraph;

  while(!ValueList.isEmpty()) {
    delete ValueList.takeFirst();
  }
}

// ---------------------------------------------------------------
void SweepDialog::slotNewValue(int)
{
  DataX const*pD = pGraph->axis(0);

  int Factor = 1, Index = 0;
  QList<mySpinBox *>::const_iterator it;
  for(it = BoxList.constBegin(); it != BoxList.constEnd(); it++) {
    Index  += (*it)->value() * Factor;
    Factor *= pD->count;
  }
  Index *= 2;  // because of complex values

  QList<Node *>::iterator node_it;
  QList<double *>::const_iterator value_it = ValueList.begin();
  for(node_it = NodeList.begin(); node_it != NodeList.end(); node_it++) {
    qDebug() << "SweepDialog::slotNewValue:(*node_it)->Name:" << (*node_it)->name();
    (*node_it)->setName(misc::num2str(*((*value_it)+Index))
                        + QString(((*node_it)->x1_() & 0x10)? "A" : "V"));
    value_it++;
  }

  Doc->viewport()->update();
}

// ---------------------------------------------------
Graph* SweepDialog::setBiasPoints()
{
  // When this function is entered, a simulation was performed.
  // Thus, the node names are still in "node->name()".

  qDebug() << "SweepDialog::setBiasPoints()";

  bool hasNoComp;
  Graph *pg = new Graph(NULL, ""); // HACK!
  QFileInfo Info(Doc->docName());
  QString DataSet = Info.path() + QDir::separator() + Doc->DataSet;

  Node *pn;
  Element *pe;

  // Note 1:
  // Invalidate it so that "Graph::loadDatFile()" does not check for the previously loaded time.
  // This is a current hack as "Graph::loadDatFile()" does not support multi-node data loading
  // from the simulation results without refreshing (changing) or invalidating the timestamp.

  NodeList.clear();
  ValueList.clear();

  // create DC voltage for all nodes
  for(pn = Doc->nodes().first(); pn != 0; pn = Doc->nodes().next()) {
    if(pn->name().isEmpty()) continue;

    pn->reset_something();
    if(pn->connectionsCount() < 2) {
      // pn->Name = "";  // no text at open nodes
		//                  // too late. d'uh
      continue; // BUG.
    }
    else {
      hasNoComp = true;
      for(auto i : pn->connections()){
        pe = i;
        if(auto w=wire(pe)) {
          if( w->isHorizontal() ) {
				 pn->set_something(2);
			 }
        }else {
          if( ((Component*)pe)->obsolete_model_hack() == "GND" ) { // BUG
            hasNoComp = true;   // no text at ground symbol
            break;
          }

		  	 // to the right is no room
          if(pn->cx_() < pe->cx_()){
			  	 pn->set_something(1);
			 }
          hasNoComp = false;
        }
		}
      if(hasNoComp) {  // text only were a component is connected
			incomplete(); // pn->Name = "";
        continue;
      }
    }

    pg->Var = pn->name() + ".V";
    pg->lastLoaded = QDateTime(); // Note 1 at the start of this function
    if(pg->loadDatFile(DataSet) == 2) {
		 incomplete(); // node names are not mutable.
      // pn->name() = misc::num2str(*(pg->cPointsY)) + "V";
      NodeList.append(pn);             // remember node ...
      ValueList.append(pg->cPointsY);  // ... and all of its values
      pg->cPointsY = 0;   // do not delete it next time !
    }else{
		 incomplete();
    //  pn->Name = "0V";
	 }


    for(auto i : pn->connections()){
		 pe = i;
      if(pe->Type == isWire) {
        if( ((Wire*)pe)->Port1 != pn )  // no text at next node
          ((Wire*)pe)->Port1->setName("");
        else  ((Wire*)pe)->Port2->setName("");
      }
	 }
  }


  // create DC current through each probe
  for(auto pc : Doc->components()){
    if(pc->obsolete_model_hack() == "IProbe") { // BUG.
      pn = pc->Ports.first()->Connection;
      if(!pn->name().isEmpty())   // preserve node voltage ?
        pn = pc->Ports.at(1)->Connection;

      pn->reset_something();
      pn->set_something(0x10);   // mark current
      pg->Var = pc->name() + ".I";
      pg->lastLoaded = QDateTime(); // Note 1 at the start of this function
      if(pg->loadDatFile(DataSet) == 2) {
        pn->setName( misc::num2str(*(pg->cPointsY)) + "A");
        NodeList.append(pn);             // remember node ...
        ValueList.append(pg->cPointsY);  // ... and all of its values
        pg->cPointsY = 0;   // do not delete it next time !
      } else{
        pn->setName("0A");
		}

      for(auto i: pn->connections()){
			pe=i;
        if(pe->Type == isWire) {
          if( ((Wire*)pe)->isHorizontal() ){
             pn->set_something(2);
          }else{
          }
        }
        else {
          if(pn->cx_() < pe->cx_()) {
            // to the right is no room
             pn->set_something(1);
          }
        }
		}
    }
  }


  Doc->showBias = 1;

  return pg;
}
