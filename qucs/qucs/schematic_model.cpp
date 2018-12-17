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

SchematicModel::SchematicModel(Schematic* s)
  :_doc(s)
{
	// presumably Q3PTRlist without this is just a QList<*> (check)
  Components.setAutoDelete(true);
  Nodes.setAutoDelete(true);
  Diagrams.setAutoDelete(true);
  Wires.setAutoDelete(true);
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

// baseclass for schematic and net languages.
class DocumentLanguage : public Object{
protected:
	DocumentLanguage() : Object(){}
public:
	virtual ~DocumentLanguage() {}
 	virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
};

class LegacySchematicLanguage : public DocumentLanguage {
	void parse(DocumentStream& stream, SchematicModel* s) const {

		QString Line;
		char mode='\0';
		while(!stream.atEnd()) {
			Line = stream.readLine();
			if(Line.at(0) == '<'
			  && Line.at(1) == '/'){
				incomplete();
				continue;
			}
			Line = Line.trimmed();
			if(Line.isEmpty()){
				continue;
			}else if(Line == "<Components>") {
				mode='C';
			}else if(Line == "<Wires>") {
				mode='W';
			}else if(Line == "<Diagrams>") { untested();
			}else if(Line == "<Paintings>") { untested();
			}

			/// \todo enable user to load partial schematic, skip unknown components
			Element*c=NULL;
			if(mode=='C'){
				c = getComponentFromName(Line, NULL /*???*/);
			}else if(mode=='W'){
				// (Node*)4 =  move all ports (later on)
				Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
				c = w->obsolete_load(Line);
				if(!c){
					delete(w);
				}else{
				}
			}else{
				incomplete();
			}

			if(c){
				s->pushBack(c);
			}else{
			}

		}
	}
}defaultSchematicLanguage;

#if 1 // not yet
  void SchematicModel::parse(DocumentStream& s, DocumentLanguage const* L){
	  if(!L){ untested();
		  L = &defaultSchematicLanguage;
	  }else{untested();
	  }
	  assert(L);
	  while(!s.atEnd()){ untested();
		  L->parse(s, this);
	  }
  }
#else
// this does not work
void SchematicModel::parse(QTextStream& stream)
{
	QString Line;
  while(!stream.atEnd()) {
    Line = stream.readLine();
	 qDebug() << "parse" << Line;
    if(Line == "<Components>") {
      if(!loadComponents(&stream)){
			incomplete();
//			throw exception...
		}
    }else if(Line == "<Wires>") {
      if(!loadWires(&stream)){
			incomplete();
//			throw exception... (in loadWires)
		}
    }else if(Line == "<Diagrams>") { untested();
      if(!loadDiagrams(&stream)){
			incomplete();
//			throw exception...
		}
    }else if(Line == "<Paintings>") { untested();
      incomplete(); // ignore Paintings fo rnow.
      PaintingList pl;
      if(!loadPaintings(&stream, &pl)){ untested();
			incomplete();
			return;
//			throw exception...
		}
    }else{ untested();
		 incomplete();
//		 throw something.
      // QMessageBox::critical(0, QObject::tr("Error"),
		//   QObject::tr("Clipboard Format Error:\nUnknown field!"));
    }
  }

}
#endif

/// ACCESS FUNCTIONS.
// these are required to move model methods over to SchematicModel
// note that _doc->...() functions still involve pointer hacks
ComponentList& SchematicModel::components()
{
	return Components;
}

void SchematicModel::pushBack(Element* what){
  if(auto c=component(what)){
	  components().append(c);
  }else{
	  incomplete();
  }

  if(doc()){
	  doc()->addToScene(what);
  }else{
  }

}

Schematic* SchematicModel::doc()
{
	return _doc;
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
	assert(_doc);
	// temporary. move stuff here....
	return _doc->symbolPaintings();
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
