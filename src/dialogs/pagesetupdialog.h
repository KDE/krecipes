/***************************************************************************
*   Copyright (C) 2003-2005 by Jason Kivlighn                             *
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

class KPopupMenu;

class SetupDisplay;
class KreDisplayItem;

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

protected:
	virtual void accept();
	virtual void reject();
	void save();

private slots:
	void loadFile();
	void loadLayout( int );
	void loadLayout( const QString &filename );
	void loadTemplate( int );
	void reloadLayout();
	void saveLayout();
	void saveAsLayout();
	void updateItemVisibility( KreDisplayItem*, bool );
	void setItemShown( int id );

private:
	QString getIncludedLayoutDir() const;
	void setActiveFile( const QString &filename );
	bool haveWritePerm( const QString &filename );
	void initShownItems();

	SetupDisplay *m_htmlPart;

	QString active_filename;
	QString active_template;
	bool have_write_perm;

	QMap<int, KreDisplayItem*> popup_widget_map;
	QMap<KreDisplayItem*, int> widget_popup_map;
	QMap<int, QString> included_layouts_map;
	KPopupMenu *shown_items_popup;
};

#endif //PAGESETUPDIALOG_H
