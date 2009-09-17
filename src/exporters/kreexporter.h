/***************************************************************************
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KREEXPORTER_H
#define KREEXPORTER_H

#include "baseexporter.h"
#include "datablocks/categorytree.h"
#include "datablocks/mixednumber.h"

class IngredientData;

/**
Export class for Krecipes native file format (.kre, .kreml)
 
@author Cyril Bosselut and Jason Kivlighn
*/
class KreExporter : public BaseExporter
{
public:
	KreExporter( CategoryTree *, const QString&, const QString&, bool compatibleNumbers = true );
	virtual ~KreExporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList & );
	virtual QString createHeader( const RecipeList & );
	virtual QString createFooter();
	
	virtual int headerFlags() const;

private:
	bool removeIfUnused( const QList<int> &cat_ids, CategoryTree *parent, bool parent_should_show = false );
	void createCategoryStructure( QString &xml, const RecipeList &recipes );
	void writeCategoryStructure( QString &xml, const CategoryTree *categoryTree );
	QString generateIngredient( const IngredientData &ing );

	CategoryTree *categories;

	MixedNumber::Format m_number_format;
	bool m_locale_aware_numbers;
};

#endif
