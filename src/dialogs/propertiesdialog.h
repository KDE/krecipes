/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
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
