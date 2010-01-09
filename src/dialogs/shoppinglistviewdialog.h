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

#ifndef SHOPPINGLISTVIEWDIALOG_H
#define SHOPPINGLISTVIEWDIALOG_H


#include <qwidget.h>

#include <khtml_part.h>
#include <khtmlview.h>
#include <kdialog.h>
#include <kvbox.h>

class IngredientList;

class ShoppingListViewDialog: public KDialog
{
	Q_OBJECT

public:
	ShoppingListViewDialog( QWidget *parent, const IngredientList &ingredientList );
	~ShoppingListViewDialog();

public slots:
	void print();

private:

	// Widgets
	KHTMLPart *shoppingListView;

	// Internal Methods
	void display( const IngredientList &ingredientList );
};

#endif
