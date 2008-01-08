/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RECIPEPRINTPREVIEW_H
#define RECIPEPRINTPREVIEW_H

#include <qstring.h>
#include <q3valuelist.h>

#include <kdialogbase.h>

class RecipeDB;
class RecipeViewDialog;

class RecipePrintPreview : public KDialogBase
{
Q_OBJECT

public:
	RecipePrintPreview( QWidget *parent, RecipeDB *db, const Q3ValueList<int> &ids );
	~RecipePrintPreview();

	void reload();

public slots:
	void slotOk();
	void slotUser1( void );

private:
	// Internal Variables
	RecipeViewDialog *recipeView;
};

#endif
