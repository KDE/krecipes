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

#ifndef SELECTAUTHORSDIALOG_H
#define SELECTAUTHORSDIALOG_H


#include <kdialog.h>
#include <k3listview.h>
#include <kcombobox.h>

#include "datablocks/elementlist.h"

class KPushButton;

class RecipeDB;

/**
@author Unai Garro
*/
class SelectAuthorsDialog: public KDialog
{

	Q_OBJECT

public:

	SelectAuthorsDialog( QWidget *parent, const ElementList &currentAuthors, RecipeDB *db );
	~SelectAuthorsDialog();
	void getSelectedAuthors( ElementList *newAuthors );

private:

	//Widgets
	KComboBox *authorsCombo;
	K3ListView *authorListView;
	KPushButton *addAuthorButton;
	KPushButton *removeAuthorButton;

	//Variables
	ElementList authorList;
	RecipeDB *database;

	//Private methods
	void loadAuthors( const ElementList &authorList );
	void createNewAuthorIfNecessary( void );
	void reloadAuthorsCombo( void );
private slots:
	void addAuthor( void );
	void removeAuthor( void );
};

#endif
