#include "symbol.h"
#include "qucslib_common.h"
/*!
 * \brief this library provides symbols. these.
 */



class QucsLibComponent : public Symbol{
private:
	QString SymbolString, LibraryName, ComponentName;
public:
	QucsLibComponent( QString& , const QString&, const QString& );
        int draw(QWidget&);
private: // implementation
  int  analyseLine(const QString&);
private: // gfx data
  int TextWidth;
  int cx, cy, x1, x2, y1, y2;
  QList<Line *> Lines;
  QList<Arc *> Arcs;
  QList<Area *> Rects, Ellips;
  QList<Text *>  Texts;
};

extern tQucsSettings QucsSettings;

QucsLibComponent::QucsLibComponent( QString& SymbolString_,
                            const QString& Lib_, const QString& Comp_)
	: SymbolString(SymbolString_), LibraryName(Lib_), ComponentName(Comp_)
{
  if (SymbolString.isEmpty())//Whenever SymbolString is empty, it tries to load the default symbol
  {
      //Load the default symbol for the current Qucs library
      ComponentLibrary parsedlib;
      QString libpath = QucsSettings.LibDir + Lib_ + ".lib";
      int result = parseComponentLibrary (libpath, parsedlib);

		// BUG: throw if there is an error. don't randomly spawn Messageboxes
		// (cleanup later)
      switch (result)//Check if the library was properly loaded
      {
        case QUCS_COMP_LIB_IO_ERROR:
            QMessageBox::critical(0, "Error", QString("Cannot open \"%1\".").arg (libpath));
            return;
        case QUCS_COMP_LIB_CORRUPT:
            QMessageBox::critical(0, "Error", "Library is corrupt.");
            return;
        default:
            break;
      }

    // copy the contents of default symbol section to a string
    SymbolString = parsedlib.defaultSymbol;
  }
}



int QucsLibComponent::draw(QWidget& w)
{
  Arcs.clear();
  Lines.clear();
  Rects.clear();
  Ellips.clear();
  Texts.clear();

  QString Line;
  ///QString foo = SymbolString;
  QTextStream stream(&SymbolString, QIODevice::ReadOnly);

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') return -1; // check for start char
    if(Line.at(Line.length()-1) != '>') return -1; // check for end char
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line);
    if(Result < 0) return -6;   // line format error
    z += Result;
  }

  x1 -= 4;   // enlarge component boundings a little
  x2 += 4;
  y1 -= 4;
  y2 += 4;
  cx  = -x1 + TextWidth;
  cy  = -y1;

  // what is this? figure out soon
  int DragNDropWidth=0;
  int TextHeight=0;

  int dx = x2-x1 + TextWidth;
  if((x2-x1) < DragNDropWidth)
    dx = (x2-x1 + DragNDropWidth)/2 + TextWidth;
  if(dx < DragNDropWidth)
    dx = DragNDropWidth;
  setMinimumSize(dx, y2-y1 + TextHeight+4);
  if(width() > dx)  dx = width();
  resize(dx, y2-y1 + TextHeight+4);
  update(); //?
  return z;      // return number of ports
}

