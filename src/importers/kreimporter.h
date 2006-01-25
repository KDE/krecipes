/***************************************************************************
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
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

#include "datablocks/recipe.h"

class Recipe;
class CategoryTree;

/**
Import for Krecipes native file format (.kre, .kreml)
 
@author Cyril Bosselut, Jason Kivlighn
*/
class KreImporter: public BaseImporter
{
public:
	KreImporter();
	virtual ~KreImporter();

private:
	void parseFile( const QString& filename );

private:
	void readCategoryStructure( const QDomNodeList& l, CategoryTree *tree );
	void readDescription( const QDomNodeList& l, Recipe* );
	void readIngredients( const QDomNodeList& l, Recipe*, const QString &header = QString::null );
	void readAmount( const QDomElement& amount, double &amount, double &amount_offset );
	void readRatings( const QDomNodeList&, Recipe * );
	void readCriterion( const QDomNodeList&, RatingCriteriaList &r );
};

#endif
