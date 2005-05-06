/***************************************************************************
*   Copyright (C) 2003 by Jason Kivlighn                                  *
*   mizunoami44@users.sourceforge.net                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <qmap.h>

#include <kdialog.h>

#include "datablocks/recipe.h"
#include "setupdisplay.h"

class KPopupMenu;

/**
  * @author Jason Kivlighn
  */
class PageSetupDialog : public KDialog
{
	Q_OBJECT

public:
	PageSetupDialog( QWidget *parent, const Recipe &sample );
	~PageSetupDialog()
	{}
	QSize minimumSize( void ) const;
	virtual QSize sizeHint( void ) const;


protected:
	virtual void accept();
	virtual void reject();
	void save();

private slots:
	void loadLayout();
	void loadLayout( int );
	void loadLayout( const QString &filename );
	void reloadLayout();
	void saveLayout();
	void saveAsLayout();
	void updateItemVisibility( QWidget*, bool );
	void setItemShown( int id );

private:
	QString getIncludedLayoutDir() const;
	void setActiveFile( const QString &filename );
	bool haveWritePerm( const QString &filename );
	void initShownItems();

	SetupDisplay *setup_display;

	QString active_filename;
	bool have_write_perm;
	QMap<int, QString> included_layouts_map;
	QMap<int, QWidget*> popup_widget_map;
	QMap<QWidget*, int> widget_popup_map;
	KPopupMenu *shown_items_popup;
};

#endif //PAGESETUPDIALOG_H
