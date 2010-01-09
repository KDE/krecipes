/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef CATEGORYCOMBOBOX_H
#define CATEGORYCOMBOBOX_H

#include <kcombobox.h>

#include <qmap.h>

#include "datablocks/element.h"

class RecipeDB;
class CategoryTree;

class CategoryComboBox : public KComboBox
{
	Q_OBJECT

public:
	CategoryComboBox( QWidget *parent, RecipeDB *db );

	void reload();
	int id( int row );

public slots:
	void loadNextGroup();
	void loadPrevGroup();

protected:
	virtual void popup();

private slots:
	void createCategory( const Element &element, int /*parent_id*/ );
	void removeCategory( int id );
	void modifyCategory( const Element &element );
	void mergeCategories( int /*to_id*/, int from_id );

	int findInsertionPoint( const QString &name );

private:
	void loadCategories( CategoryTree *categoryList, int &row );

	RecipeDB *database;
	QMap<int, int> categoryComboRows; // Contains the category id for every given row in the category combobox
	int m_offset;
};

#endif //CATEGORYCOMBOBOX_H

