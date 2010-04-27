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

#ifndef RECIPEIMPORTDIALOG_H
#define RECIPEIMPORTDIALOG_H

#include <q3valuelist.h>
#include <qmap.h>
#include <q3listview.h>

#include <kdialog.h>

#include "datablocks/recipelist.h"

class K3ListView;


class Recipe;
class CustomCheckListItem;

/**
  * @author Jason Kivlighn
  */
class RecipeImportDialog : public KDialog
{
public:
	explicit RecipeImportDialog( const RecipeList &all_recipes, QWidget *parent = 0 );
	~RecipeImportDialog();

	RecipeList getSelectedRecipes();

protected slots:
	virtual void languageChange();

private:
	void loadListView();

	K3ListView* kListView;

	QMap<CustomCheckListItem*, RecipeList::const_iterator> *recipe_items;
	const RecipeList list_copy;
};

/** A specialized QCheckListItem that sets the state of its children to its
  * current state.
  * @author Jason Kivlighn
  */
class CustomCheckListItem : public Q3CheckListItem
{
public:
	CustomCheckListItem( Q3ListView *parent, const QString &, Type );
	CustomCheckListItem( CustomCheckListItem *parent, const QString &, Type );
	CustomCheckListItem( Q3CheckListItem *parent, Q3CheckListItem *after, const QString &, Type );

protected:
	virtual void stateChange( bool );
	bool locked() const
	{
		return m_locked;
	}
	void setLocked( bool b )
	{
		m_locked = b;
	}

private:
	bool m_locked;
};

#endif //RECIPEIMPORTDIALOG_H
