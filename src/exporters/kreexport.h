/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
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

#include <stdlib.h> // For getenv()

#include "baseexporter.h"
#include "datablocks/categorytree.h"

/**
Export class for Krecipes native file format (.kre, .kreml)

@author Cyril Bosselut and Jason Kivlighn
*/
class KreExporter : public BaseExporter{
public:
    KreExporter( CategoryTree *, const QString&, const QString& );

    virtual ~KreExporter();

    virtual QString createContent( const RecipeList & );

protected:
    virtual void saveToFile( const RecipeList & );
    virtual QString extensions() const{ return ".kre,.kreml"; }

private:
    bool removeIfUnused( const QValueList<int> &cat_ids, CategoryTree *parent, bool parent_should_show = false );
    void createCategoryStructure( QString &xml, const RecipeList &recipes );
    void writeCategoryStructure(QString &xml, const CategoryTree *categoryTree );

    CategoryTree *categories;
};

#endif
