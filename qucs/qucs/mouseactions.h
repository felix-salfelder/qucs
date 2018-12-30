/***************************************************************************
                              mouseactions.h
                             ----------------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef MOUSEACTIONS_H
#define MOUSEACTIONS_H

#include "element.h"
#include "qt_compat.h"

// a mouse action on an element.
// formerly, a mouse action was implemented by means of messing with element
// internals.
class ElementMouseAction {
public:
	explicit ElementMouseAction(Element* e)
		: _e(e)
	{
	}
public:
	Element* element() { return _e; }

	void clear(){
		_e=nullptr;
	}
public: // compat with old code
	operator bool() const{
		return _e;
	}
	void setSelected() const{
		incomplete();
	}
	bool isSelected() const{
		incomplete();
		return false;
	}

public: // access coordinates from old code.
	     // newer code might use Qt conventions...?
	int const& cx_() const { assert(_e); return _e->cx_(); }
	int const& cy_() const { assert(_e); return _e->cy_(); }
	int const& x1_() const { assert(_e); return _e->x1_(); }
	int const& y1_() const { assert(_e); return _e->y1_(); }
	int const& x2_() const { assert(_e); return _e->x2_(); }
	int const& y2_() const { assert(_e); return _e->y2_(); }
public:
	// don't use. just to compile
	int Type; // BUG BUG
	ElementMouseAction* operator->(){
		unreachable(); // complain loudly.
		return this;
	}
	void setObsoleteType(int t){
		_action_type = t;
	}

private:
	int _action_type; // the legacy way.
	                  //  might need cleanup
  Element* _e;
};

// enable access to attached elements.
// this might be temporary
inline Painting* painting(ElementMouseAction ema)
{
	return painting(ema.element());
}
inline Diagram* diagram(ElementMouseAction ema)
{
	return diagram(ema.element());
}

class Wire;
class Schematic;
class QPainter;
class QMenu;
class QMouseEvent;
class QucsApp;

extern QAction *formerAction;

class MouseActions {
public:
  MouseActions(QucsApp*);
  virtual ~MouseActions();

  void setPainter(Schematic*);
  bool pasteElements(Schematic*);
  void editElement(Schematic*, QMouseEvent*);
  void editLabel(Schematic*, WireLabel*);

  bool drawn;  // indicates whether the scheme element was drawn last time
// private: BUG.
  Element *selElem;  // component/diagram/painting selected in IconView
  Element *focusElement;
  QMouseEvent *focusMEvent;

  int  MAx1, MAy1,MAx2, MAy2, MAx3, MAy3;  // cache for mouse movements
  Q3PtrList<Element> movingElements;
  int movingRotated;

  // menu appearing by right mouse button click on component
  QMenu *ComponentMenu;

private:
  bool isMoveEqual;
  QucsApp* App;

  // -------------------------------------------------------------------
public:
  void MMoveSelect(Schematic*, QMouseEvent*);
  void MMoveElement(Schematic*, QMouseEvent*);
  void MMoveWire1(Schematic*, QMouseEvent*);
  void MMoveWire2(Schematic*, QMouseEvent*);
  void MMoveMoving(Schematic*, QMouseEvent*);
  void MMoveMoving2(Schematic*, QMouseEvent*);
  void MMovePaste(Schematic*, QMouseEvent*);
  void MMoveDelete(Schematic*, QMouseEvent*);
  void MMoveLabel(Schematic*, QMouseEvent*);
  void MMoveMarker(Schematic*, QMouseEvent*);
  void MMoveMirrorY(Schematic*, QMouseEvent*);
  void MMoveMirrorX(Schematic*, QMouseEvent*);
  void MMoveRotate(Schematic*, QMouseEvent*);
  void MMoveActivate(Schematic*, QMouseEvent*);
  void MMoveOnGrid(Schematic*, QMouseEvent*);
  void MMoveResizePainting(Schematic*, QMouseEvent*);
  void MMoveMoveText(Schematic*, QMouseEvent*);
  void MMoveMoveTextB(Schematic*, QMouseEvent*);
  void MMoveZoomIn(Schematic*, QMouseEvent*);
  void MMoveScrollBar(Schematic*, QMouseEvent*);

  void MPressSelect(Schematic*, QMouseEvent*);
  void MPressDelete(Schematic*, QMouseEvent*);
  void MPressActivate(Schematic*, QMouseEvent*);
  void MPressMirrorX(Schematic*, QMouseEvent*);
  void MPressMirrorY(Schematic*, QMouseEvent*);
  void MPressRotate(Schematic*, QMouseEvent*);
  void MPressElement(Schematic*, QMouseEvent*);
  void MPressLabel(Schematic*, QMouseEvent*);
  void MPressWire1(Schematic*, QMouseEvent*);
  void MPressWire2(Schematic*, QMouseEvent*);
  void MPressPainting(Schematic*, QMouseEvent*);
  void MPressMarker(Schematic*, QMouseEvent*);
  void MPressOnGrid(Schematic*, QMouseEvent*);
  void MPressMoveText(Schematic*, QMouseEvent*);
  void MPressZoomIn(Schematic*, QMouseEvent*);

  void MDoubleClickSelect(Schematic*, QMouseEvent*);
  void MDoubleClickWire2(Schematic*, QMouseEvent*);

  void MReleaseSelect(Schematic*, QMouseEvent*);
  void MReleaseSelect2(Schematic*, QMouseEvent*);
  void MReleaseActivate(Schematic*, QMouseEvent*);
  void MReleaseMoving(Schematic*, QMouseEvent*);
  void MReleaseResizeDiagram(Schematic*, QMouseEvent*);
  void MReleasePaste(Schematic*, QMouseEvent*);
  void MReleaseResizePainting(Schematic*, QMouseEvent*);
  void MReleaseMoveText(Schematic*, QMouseEvent*);
  void MReleaseZoomIn(Schematic*, QMouseEvent*);

  void paintElementsScheme(Schematic*);
  void rotateElements(Schematic*, int&, int&);
  void moveElements(Schematic*, int&, int&);
  void moveElements(Q3PtrList<Element>*, int, int);
  void endElementMoving(Schematic*, Q3PtrList<Element>*);
  void rightPressMenu(Schematic*, QMouseEvent*);
};

#endif
