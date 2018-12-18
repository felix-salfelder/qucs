// Felix 2018, GPLv3+
// Part of QUCS

#include "qucsdoc.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "globals.h"
#include "wire.h"
#include "diagram.h" // BUG

 //BUG
Element* getComponentFromName(QString& Line, Schematic* p);

class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage() : SchematicLanguage(){ untested();
		defaultSchematicLanguage = this;
	}
private: // stuff saved from schematic_file.cpp
	Diagram* loadDiagram(QString const& Line, DocumentStream& /*, DiagramList *List */) const;
private:
	void parse(DocumentStream& stream, SchematicModel* s) const {
		QString Line;

		// mode: a throwback to the legacy format:
		//       connect legacy "parsers".
		// this is not needed in a proper SchematicLanguage
		char mode='\0';
		while(!stream.atEnd()) {
			Line = stream.readLine();
			Line = Line.trimmed();
			qDebug() << "LSL::parse" <<  Line;
			if(Line.at(0) == '<'
			  && Line.at(1) == '/'){
				qDebug() << "endtag?" << Line;
				incomplete();
			}else if(Line.isEmpty()){
			}else if(Line == "<Components>") {
				mode='C';
			}else if(Line == "<Symbols>") {
				mode='S';
			}else if(Line == "<Wires>") {
				mode='W';
			}else if(Line == "<Diagrams>") { untested();
				mode='D';
			}else if(Line == "<Properties>") { untested();
				mode='Q';
			}else if(Line == "<Paintings>") { untested();
				mode='P';
			}else{

				/// \todo enable user to load partial schematic, skip unknown components
				Element*c=nullptr;
				if(mode=='C'){
					c = getComponentFromName(Line, s /*connect ports?*/);
				}else if(mode=='S'){
					incomplete();
#if 0
					SchematicSymbol* s=new SchematicSymbol();
					try{
						qDebug() << "symbol Paintings";
						s->symbolPaintings().load(&stream);
						c = s;
					}catch(...){
						incomplete();
					}
#endif

				/// \todo enable user to load partial schematic, skip unknown components
				Element*c=NULL;
				if(mode=='C'){
					c = getComponentFromName(Line, s /*connect ports?*/);
				}else if(mode=='S'){
//					if(!symbolPaintings().load(&stream))
				}else if(mode=='W'){
					// (Node*)4 =  move all ports (later on)
					Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
					c = w->obsolete_load(Line);
					if(!c){
						qDebug() << "ERROR" << Line;
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
	}

private:
	void printSymbol(Symbol const*, stream_t&) const;
}defaultSchematicLanguage_;

static Dispatcher<DocumentLanguage>::INSTALL
    p(&doclang_dispatcher, "leg_sch", &defaultSchematicLanguage_);
// ------------------------------------------------------

Diagram* LegacySchematicLanguage::loadDiagram(QString const& line_in,
		DocumentStream& stream /*, DiagramList *List */)const
{ untested();
	Diagram *d;
	QString Line=line_in;
	QString cstr;
	if(!stream.atEnd()) { untested();
		qDebug() << "diagram?" << Line;
		if(Line.at(0) == '<') if(Line.at(1) == '/') return nullptr;
		Line = Line.trimmed();
		if(Line.isEmpty()){
			return nullptr;
		}else{
		}
		cstr = Line.section(' ',0,0);    // diagram type
		std::string what=cstr.toStdString();

		if(auto x=diagram_dispatcher[what.c_str()+1]){
			d=prechecked_cast<Diagram*>(x->clone());
			assert(d);
			qDebug() << "gotit" << what.c_str();
		}else{ untested();
			incomplete();
			// throw ...
			return nullptr;
		}

		if(!d->load(Line, &stream)) { untested();
			incomplete();
			delete d;
			return nullptr;
		}else{ untested();
		}
		return d;
	}

	return nullptr;
}
//
//
// -------------------------------------------------------
// was: void Schematic::saveComponent(QTextStream& s, Component const* c) const
void LegacySchematicLanguage::printSymbol(Symbol const* sym, stream_t& s) const
{ untested();
	Component const* c=dynamic_cast<Component const*>(sym);
	if(!c){ untested();
		incomplete();
		return;
	}else{ untested();
	}
#if XML
	QDomDocument doc;
	QDomElement el = doc.createElement (Model);
	doc.appendChild (el);
	el.setTagName (Model);
	el.setAttribute ("inst", Name.isEmpty() ? "*" : Name);
	el.setAttribute ("display", isActive | (showName ? 4 : 0));
	el.setAttribute ("cx", cx);
	el.setAttribute ("cy", cy);
	el.setAttribute ("tx", tx);
	el.setAttribute ("ty", ty);
	el.setAttribute ("mirror", mirroredX);
	el.setAttribute ("rotate", rotated);

	for (Property *pr = Props.first(); pr != 0; pr = Props.next()) { untested();
		el.setAttribute (pr->Name, (pr->display ? "1@" : "0@") + pr->Value);
	}
	qDebug << doc.toString();
#endif
	// s << "  "; ??
	s << "<" << c->obsolete_model_hack();

	s << " ";
	if(c->name().isEmpty()){
		s << "*";
	}else{
		s << c->name();
	}
	s << " ";

	int i=0;
	if(!c->showName){
		i = 4;
	}
	i |= c->isActive;
	s << QString::number(i);
	s << " "+QString::number(c->cx)+" "+QString::number(c->cy);
	s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
	s << " ";
	if(c->mirroredX){
		s << "1";
	}else{
		s << "0";
	}
	s << " " << QString::number(c->rotated);

	// write all properties
	// FIXME: ask component for properties, not for dictionary
	Component* cc=const_cast<Component*>(c); // BUGBUGBUGBUG
	// cannot access Props without this hack
	for(Property *p1 = cc->Props.first(); p1 != 0; p1 = cc->Props.next()) {
		if(p1->Description.isEmpty()){
			s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
		}else{
			s << " \""+p1->Value+"\"";
		}
		s << " ";
		if(p1->display){
			s << "1";
		}else{
			s << "0";
		}
	}

	s << ">";
} // printSymbol
