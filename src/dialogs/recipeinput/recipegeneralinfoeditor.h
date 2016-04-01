/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEGENERALINFOEDITOR_H
#define RECIPEGENERALINFOEDITOR_H


#include "backends/recipedb.h"

#include <QWidget>

class Recipe;


namespace Ui {
	class RecipeGeneralInfoEditor;
}

class RecipeGeneralInfoEditor: public QWidget
{
	Q_OBJECT

public:
	RecipeGeneralInfoEditor( QWidget * parent, RecipeDB * db );

	void loadRecipe( Recipe * recipe );
	void updateRecipe();

signals:
	void changed();
	void titleChanged(const QString & title);

public slots:
	void editCategoriesSlot();

private slots:
	void titleChangedSlot(const QString & title);

	void changePhotoSlot();
	void savePhotoAsSlot();
	void clearPhotoSlot();

	void editAuthorsSlot();

private:
	RecipeDB::IdType createNewYieldIfNecessary( const QString &yield );
	void showAuthors();
	void showCategories();

	Ui::RecipeGeneralInfoEditor * ui;

	Recipe * m_recipe;
	RecipeDB * m_database;
};

#endif
