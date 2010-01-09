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

#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H


//Added by qt3to4:
#include <QGridLayout>
#include <k3listview.h>
#include <kvbox.h>

class KPushButton;
class RecipeDB;
class StdPropertyListView;
class ActionsHandlerBase;
class KAction;
class PropertyActionsHandler;

/**
@author Unai Garro
*/
class PropertiesDialog: public QWidget
{
	Q_OBJECT
public:
	PropertiesDialog( QWidget *parent, RecipeDB *db );
	~PropertiesDialog();
	void reload( void );
	ActionsHandlerBase* getActionsHandler() const;
	void addAction( KAction * action );

private:
	// Variables
	RecipeDB *database;

	// Widgets
	QGridLayout* layout;
	KPushButton* addPropertyButton;
	KPushButton* removePropertyButton;
	StdPropertyListView* propertyListView;
	PropertyActionsHandler *propertyActionsHandler;

};

#endif
