/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRELISTVIEW_H
#define KRELISTVIEW_H

#include <qlabel.h>
#include <q3vbox.h>
#include <k3listview.h>
#include <klineedit.h>

class DBListViewBase;

/**
@author Unai Garro
*/

class KreListView: public Q3VBox
{
	Q_OBJECT
public:

	KreListView( QWidget *parent, const QString &title = QString::null, bool filter = false, int filterCol = 0, QWidget *embeddedWidget = 0 );
	~KreListView();
	K3ListView *listView()
	{
		return list;
	}

	void setListView( K3ListView *list_view )
	{
		delete list;
		list = list_view;
	}
	void setListView( DBListViewBase *list_view );

	void setCustomFilter( QObject *receiver, const char *slot );
	QString filterText() const { return filterEdit->text(); }

public slots:
	void refilter();

signals:
	void textChanged( const QString & );

private:
	Q3HBox *filterBox;
	QLabel *listLabel;
	int filteredColumn;
	QLabel *filterLabel;
	KLineEdit *filterEdit;
	K3ListView *list;

private slots:
	void filter( const QString& s );
};

#endif
