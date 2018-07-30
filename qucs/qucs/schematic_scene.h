/*
 * schematic_scene.h - define schematic scene
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *               2018, Felix Salfelder
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

#ifndef SCHEMATICSCENE_H_
#define SCHEMATICSCENE_H_

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <assert.h>

class Element;

#if 0
class SchematicScene : public QGraphicsScene
{
Q_OBJECT
public:
  SchematicScene (QObject *);
  virtual ~SchematicScene ();

  // void removeItem(Element const*);
  void addItem(Element*);
  void addItem(QGraphicsItem* x){
	  QGraphicsScene::addItem(x);
  }

private:

protected:
  void drawBackground(QPainter *painter, const QRectF& rect);
};
#endif

#include "element.h" // TODO: move implementation to .cpp
                     // also: this relates to scene, but is this the right place?
							// (having other problems, still)

// A graphics element on the screen.
// kind of smart-pointer/proxy.
#if 1 // not yet
typedef Element* ElementGraphics;
#else
class ElementGraphics : public QObject {
private:
	ElementGraphics(){}
public:
	explicit ElementGraphics(Element* e)
		: _e(e)
	{
		assert(_e);
	}
	~ElementGraphics(){
	}
private: // later: Qgraphics virtual overrides
//  void paint() { assert(_e); _e->paint(); }
//  void paintScheme(Schematic *s) { assert(_e); _e->paintScheme(s); }
  void paintScheme(QPainter *p) { assert(_e); _e->paintScheme(p); }
//  QRectF boundingRect() const {return _e->boundingRect(); }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0){
	  assert(_e);
//	  return _e->paint(painter, option, widget);
  }

  // TODO: move coordinate stuff here.
//  void setPos(int a, int b){
//     _e->setPos(a, b);
//  }
public:
  Element* operator->(){ assert(_e); return _e; }
  Element const* operator->() const{ assert(_e); return _e; }
  void setCenter(int i, int j, bool relative=false){
	  assert(_e);
	  _e->setCenter(i, j, relative);
  }
  void getCenter(int& i, int& j){
	  assert(_e);
	  _e->getCenter(i, j);
  }
private:
  Element* _e;
}; // ElementGraphics

Component const* component(ElementGraphics const);
Wire const* wire(ElementGraphics const);
WireLabel const* wireLabel(ElementGraphics const);
Diagram const* diagram(ElementGraphics const);
Painting const* painting(ElementGraphics const);

Component* component(ElementGraphics);
Wire* wire(ElementGraphics);
WireLabel* wireLabel(ElementGraphics);
Diagram* diagram(ElementGraphics);
Painting* painting(ElementGraphics);
#endif


// a mouse action on an element.
// formerly, a mouse action was implemented by means of messing with element
// internals.

#if 0
Component const* component(ElementMouseAction const*);
Wire const* wire(ElementMouseAction const*);
WireLabel const* wireLabel(ElementMouseAction const*);
Diagram const* diagram(ElementMouseAction const*);
Painting const* painting(ElementMouseAction const*);

Component* component(ElementMouseAction*);
Wire* wire(ElementMouseAction*);
WireLabel* wireLabel(ElementMouseAction*);
Diagram* diagram(ElementMouseAction*);
Painting* painting(ElementMouseAction*);
#endif

#endif /* SCHEMATICSCENE_H_ */
