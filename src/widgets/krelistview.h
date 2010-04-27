/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRELISTVIEW_H
#define KRELISTVIEW_H

#include <QLabel>

#include <k3listview.h>
#include <klineedit.h>
#include <kvbox.h>

class DBListViewBase;

/**
@author Unai Garro
*/

class KreListView: public KVBox
{
	Q_OBJECT
public:

	explicit KreListView( QWidget *parent, const QString &title = QString(), bool filter = false, int filterCol = 0, QWidget *embeddedWidget = 0 );
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
	void returnPressed( const QString & );

private:
	KHBox *filterBox;
	QLabel *listLabel;
	int filteredColumn;
	QLabel *filterLabel;
	KLineEdit *filterEdit;
	K3ListView *list;

private slots:
	void filter( const QString& s );
	void clearSearch();
};

#endif
