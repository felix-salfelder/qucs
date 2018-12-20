/***************************************************************************
                           schematic_model.cpp
                             ---------------
    begin                : 2018
    copyright            : Felix
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic.h"
#include "schematic_lang.h"
#include "globals.h"

SchematicModel::SchematicModel(Schematic* s)
  :_doc(s)
{
	// presumably Q3PTRlist without this is just a QList<*> (check)
  Components.setAutoDelete(true);
  Nodes.setAutoDelete(true);
  Diagrams.setAutoDelete(true);
  Wires.setAutoDelete(true);
  _symbol=new SchematicSymbol();
}

void SchematicModel::clear()
{
	// memory leak?!
  components().clear();
  nodes().clear();
  diagrams().clear();
  wires().clear();
  paintings().clear();
  //SymbolPaints.clear(); ??
}

QString const& SchematicModel::devType() const
{
	return DevType;
}

void SchematicModel::setDevType(QString const& s)
{
	DevType = s;
}

void SchematicModel::parse(DocumentStream& s, SchematicLanguage const* L){
	if(!L){
		assert(defaultSchematicLanguage);
		L = prechecked_cast<SchematicLanguage const*>(defaultSchematicLanguage);
		assert(L);
	}else{untested();
	}
	assert(L);
	while(!s.atEnd()){ untested();
		qDebug() << "entering parse";
		L->parse(s, this);
		assert(s.atEnd()); // happens with legacy lang
	}
}

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	return Components;
}

SchematicSymbol::SchematicSymbol(){
	_paint = new PaintingList();
}
SchematicSymbol::~SchematicSymbol(){
	delete _paint;
	_paint = nullptr;
}

void SchematicModel::pushBack(Element* what){
	if(auto c=component(what)){
      simpleInsertComponent(c);
	}else if(auto w=wire(what)){ untested();
		simpleInsertWire(w);
	}else if(auto s=dynamic_cast<SchematicSymbol*>(what)){ untested();
		qDebug() << "Model replacing symbol";
		delete _symbol;
		_symbol = s;
	}else{
		incomplete();
	}

	if(doc()){
		// only necessary when gui is running.
		doc()->addToScene(what);
	}else{
	}

}

Schematic* SchematicModel::doc()
{
	return _doc;
}

QFileInfo const& SchematicModel::getFileInfo ()const
{
	return FileInfo;
}

WireList& SchematicModel::wires()
{
	return Wires;
}

NodeList& SchematicModel::nodes()
{
	return Nodes;
}

PaintingList& SchematicModel::paintings()
{
	assert(_doc);
	// temporary. move stuff here....
	return _doc->paintings();
}

PaintingList& SchematicModel::symbolPaintings()
{
	assert(_symbol);
	// temporary. move stuff here....
	return _symbol->symbolPaintings();
}

PaintingList& SchematicSymbol::symbolPaintings()
{
	assert(_paint);
	return *_paint;
}

DiagramList& SchematicModel::diagrams()
{
	return Diagrams;
}

// same, but const.
ComponentList const& SchematicModel::components() const
{
	return Components;
}

WireList const& SchematicModel::wires() const
{
	return Wires;
}

NodeList const& SchematicModel::nodes() const
{
	return Nodes;
}

PaintingList const& SchematicModel::paintings() const
{
	// temporary. move stuff here....
	return _doc->paintings();
}

DiagramList const& SchematicModel::diagrams() const
{
	// temporary. move stuff here....
	return _doc->diagrams();
}

void SchematicModel::merge(SchematicModel& src)
{
  for(auto i : src.components()){ itested();
	  components().append(i);
  }
  src.components().clear();
}
