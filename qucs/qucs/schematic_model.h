/***************************************************************************
                              schematic_model.h
                             --------------------
    copyright            : (C) 2018 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_SCHEMATIC_MODEL_H
#define QUCS_SCHEMATIC_MODEL_H

#include <QGraphicsScene>
#include <QFileInfo> // BUG
#include "object.h"
#include "wire.h"
#include "node.h"
#include "schematic_scene.h"
#include "schematic_symbol.h"
#include "components/subcircuit.h" // BUG
#include "qucsdoc.h"

class QPlainTextEdit; //??!

// TODO: refactor here
class WireList : public Q3PtrList<Wire> {
};
// TODO: refactor here
class NodeList : public Q3PtrList<Node> {
};
// TODO: refactor here
class DiagramList : public Q3PtrList<Diagram> {
};
// TODO: refactor here
class ComponentList : public Q3PtrList<Component> {
	// void first(){} // GOAL: hide, still compile.
};
// TODO: refactor here
class PaintingList : public Q3PtrList<Painting> {
public:
	bool load(QTextStream *stream);
public:
	void sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax) const;
};

// hmm, which?
class DocumentLanguage;
class SchematicLanguage;

class SchematicModel{
private:
  SchematicModel(){}
public:
  SchematicModel(Schematic* s);
public: // stuff saved from Schematic
  QString createClipboardFile();
  void sizeOfAll(int&, int&, int&, int&, float) const;
  void simpleInsertComponent(Component* c);
  void simpleInsertWire(Wire*);
  //private??
  bool giveNodeNames(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int);
  bool throughAllComps(DocumentStream&, int&, QStringList&, QPlainTextEdit *, int,
		  bool creatingLib=false // ?!
		  );
  bool createLibNetlist(DocumentStream&, QPlainTextEdit*, int);
  bool createSubNetlist(DocumentStream&, int&, QStringList&, QPlainTextEdit*, int);
  void throughAllNodes(bool, QStringList&, int&);
  void propagateNode(QStringList&, int&, Node*);
  void collectDigitalSignals(void);
  QString createNetlist(DocumentStream&, int);
  void createSubNetlistPlain(DocumentStream&, QPlainTextEdit*, int,
		  bool creatingLib=false // ??
		  );
  QFileInfo getFileInfo ()const{
	  incomplete();
	  return QFileInfo();
  }
  void print(QPrinter*, QPainter*, bool, bool);

public:
  void parse(DocumentStream& stream, SchematicLanguage const*l=nullptr);
  int  prepareNetlist(DocumentStream&, QStringList&, QPlainTextEdit*);
  bool loadDocument(QFile& /*BUG*/ file);
  bool loadPaintings(QTextStream*, PaintingList* p=NULL);
  bool loadProperties(QTextStream*);
  bool loadComponents(QTextStream*);
  bool loadDiagrams(QTextStream*);
  bool loadWires(QTextStream*);

  void clear();
  void pushBack(Element* what);
public:
  void merge(SchematicModel&);

public: // scene interaction
  void toScene(QGraphicsScene& s, QList<ElementGraphics*>* l=nullptr) const;
public: // obsolete.
  static void saveComponent(QTextStream& s, Component /* FIXME const */* c);
private: // TODO: actually store here.
  WireList& wires();
  NodeList& nodes();
  DiagramList& diagrams();
  PaintingList& paintings();
  ComponentList& components();
  PaintingList& symbolPaintings();
public:
  WireList const& wires() const;
  NodeList const& nodes() const;
  DiagramList const& diagrams() const;
  PaintingList const& paintings() const;
  ComponentList const& components() const;

  Schematic* doc();
  QString const& portType(int i) const{
	  return PortTypes[i];
  }
private: // TODO: remove. store parent in ElementGraphics.
  Schematic* _doc;
private:
  ComponentList Components;
  NodeList Nodes;
  DiagramList Diagrams;
  WireList Wires;
  SchematicSymbol* _symbol;
  QStringList PortTypes;
public: // for now.
  friend class Schematic;
};

#endif
