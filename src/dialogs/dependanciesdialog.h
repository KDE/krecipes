/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef DEPENDANCIESDIALOG_H
#define DEPENDANCIESDIALOG_H

#include <QLabel>
#include <QList>

#include <kdialog.h>

#include "datablocks/elementlist.h"

class QListWidget;

struct ListInfo {
	ElementList list;
	QString name;
};

/**
@author Unai Garro
*/
class DependanciesDialog: public KDialog
{
public:
	//Methods
	DependanciesDialog( QWidget *parent, const QList<ListInfo> &lists, bool deps_are_deleted = true );
	DependanciesDialog( QWidget *parent, const ListInfo &list, bool deps_are_deleted = true );

	~DependanciesDialog();

	void setCustomWarning( const QString &msg ){ m_msg = msg; }

public slots:
	void accept();

private:
	//Widgets
	QLabel *instructionsLabel;

	bool m_depsAreDeleted;
	QString m_msg;

	// Methods
	void init( const QList<ListInfo> &lists );
	void loadList( QListWidget*, const ElementList &list );
};

#endif
