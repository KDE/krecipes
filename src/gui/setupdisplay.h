/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SETUPDISPLAY_H
#define SETUPDISPLAY_H

#include <qwidget.h>
#include <qmap.h>
#include <qptrlist.h>

#include "../recipe.h"
#include "../widgets/dragarea.h"

class QAction;
class QPopupMenu;
class QLabel;
class QWidget;

class DragArea;

/** @brief A very specialized @ref DragArea for editing the recipe setup
  *
  * Set up the items of a recipe for display.
  *
  * @author Jason Kivlighn
  */
class SetupDisplay : public DragArea
{
Q_OBJECT

public:
	SetupDisplay( const Recipe &, QWidget *parent );
	~SetupDisplay();

	void save();

	static void createSetupIfNecessary();

protected slots:
	void widgetClicked( QMouseEvent *, QWidget * );

	//slots to set properties of item boxes
	void setBackgroundColor();
	void setTextColor();
	void setFont();
	void setShown(int id);
	void setAlignment( QAction * );

protected:
	virtual QSize sizeHint() const{ return m_size; }
	virtual QSize minimumSizeHint() const{ return QSize(1,1); }

private:
	void loadSetup();
	void createWidgets( const Recipe &sample );

	QLabel *title_box;
	QLabel *instr_box;
	QLabel *photo_box;
	QLabel *servings_box;
	QLabel *categories_box;
	QLabel *authors_box;
	QLabel *id_box;
	QLabel *ingredients_box;
	QLabel *properties_box;

	QSize m_size;

	enum Properties { None = 0, BackgroundColor = 1, TextColor = 2, Font = 4, Visibility = 8, Geometry = 16, Alignment = 32, StaticHeight = 64 };
	QMap< QWidget*, unsigned int > *box_properties;

	QPopupMenu *popup;
};

#endif //SETUPDISPLAY_H

