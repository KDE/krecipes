/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KREEXPORTER_H
#define KREEXPORTER_H

#include <kglobal.h>
#include <klocale.h>
#include <ktempfile.h>
#include <kstandarddirs.h>
#include <kmdcodec.h>
#include <ktar.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qimage.h>

#include "../recipe.h"
#include "../DBBackend/recipedb.h"

class Recipe;
class RecipeDB;
/**
Export classe for Krecipes native file format (.kre, .kreml)

@author Cyril Bosselut
*/
class KreExporter{
public:
    KreExporter( RecipeDB *, const int , const QString&, const QString );

    ~KreExporter();
    void exporter( );
    QString createKRE();

    QFile* file;
    QString name;
    QString type;

private:
    Recipe *recipe;
    RecipeDB *database;
    bool exportEnable;
};

#endif
