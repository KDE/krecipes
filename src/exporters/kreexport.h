/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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
#include <qvaluelist.h>

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
    KreExporter( RecipeDB *, const QString&, const QString );

    ~KreExporter();
    void exporter( const int );
    void categoryExporter(QValueList<int>*);
    QString createKRE(const int recipeid = -1);

    QFile* file;
    QString name;
    QString type;

private:
    Recipe *recipe;
    RecipeDB *database;
    bool exportEnable;
};

#endif
