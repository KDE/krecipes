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

#include <qdialog.h>
#include <qmap.h>
#include <qvaluelist.h>

#include "../recipe.h"

class QPopupMenu;
class QAction;
class QLabel;

class DragArea;

/**
  * @author Jason Kivlighn
  */
class PageSetupDialog : public QDialog
{
Q_OBJECT;

public:
	PageSetupDialog( QWidget *parent, const Recipe &sample );
	~PageSetupDialog();

protected:
	virtual void accept();
	void save();

protected slots:
	void widgetClicked( QMouseEvent *, QWidget * );

	//slots to set properties of item boxes
	void setBackgroundColor();
	void setTextColor();
	void setFont();
	void setShown(int id);
	void setAlignment( QAction * );

private:
	void loadSetup();
	void createWidgets( const Recipe &sample );

	DragArea *dragArea;

	QLabel *title_box;
	QLabel *instr_box;
	QLabel *photo_box;
	QLabel *servings_box;
	QLabel *categories_box;
	QLabel *authors_box;
	QLabel *id_box;
	QLabel *ingredients_box;

	enum Properties { None = 0, BackgroundColor = 1, TextColor = 2, Font = 4, Visibility = 8, Geometry = 16, Alignment = 32 };
	QMap< QWidget*, unsigned int > *box_properties;

	QPopupMenu *popup;
};

#endif //PAGESETUPDIALOG_H
