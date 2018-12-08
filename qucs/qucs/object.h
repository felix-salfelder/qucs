/***************************************************************************
                                 symbol.h
                                -----------
    begin                : 2016
    copyright            : (C) 2016 Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** \file object.h
  * \brief base class for all sorts of objects
  *
  */

#include "io_trace.h"

#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H

class Object{
protected:
	Object(){}
public:
  virtual ~Object(){}

  // clone the object.
  virtual Object* newOne()const {return 0 /*NULL, actually*/;}
};

#endif
