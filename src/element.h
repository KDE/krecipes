/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro                                      *
 *   uga@ee.ed.ac.uk                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ELEMENT_H
#define ELEMENT_H
#include <qstring.h>
/**
@author Unai Garro
*/
class Element{
public:
    Element();

    ~Element();
    int id;
    QString name;

};

#endif
