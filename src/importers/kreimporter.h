/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KREIMPORTER_H
#define KREIMPORTER_H

#include <klocale.h>
#include <kstandarddirs.h>
#include <kmdcodec.h>
#include <ktar.h>
#include <ktempfile.h>

#include <qfile.h>
#include <qstringlist.h>
#include <qdom.h>

#include "baseimporter.h"
#include "../recipe.h"

class Recipe;
/**
Import for Krecipes native file format (.kre, .kreml)

@author Cyril Bosselut
*/
class KreImporter:public BaseImporter{
public:
    KreImporter();
    virtual ~KreImporter();
    
private:
	void parseFile( const QString& filename );

private:
	void readDescription(const QDomNodeList& l, Recipe*);
	void readIngredients(const QDomNodeList& l, Recipe*);
};

#endif
