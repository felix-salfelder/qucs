/***************************************************************************
                                coplanar.h
                               ------------
    begin                : Thu May 05 2005
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

#ifndef CPWGAP_H
#define CPWGAP_H

#include "component.h"


class CPWgap : public Component  {
public:
  CPWgap();
  ~CPWgap();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
