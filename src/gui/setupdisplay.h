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

#include <qdom.h>
#include <qwidget.h>
#include <qmap.h>

#include "../recipe.h"
#include "../widgets/dragarea.h"
#include <math.h>

class KPopupMenu;

class QAction;
class QLabel;
class QWidget;

class DragArea;

typedef QMap< QWidget*, unsigned int > PropertiesMap;

// ### maybe we should move koffice's KoRect/KoPoint/KoSize to kdelibs...
class PreciseRect
{
public:
	PreciseRect(double top = 0, double left = 0, double width = 0, double height = 0) :
	  m_top(top), m_left(left), m_width(width), m_height(height)
	{}
	
	PreciseRect( const QRect &r ){ m_top = r.top(); m_left = r.left(); m_width = r.width(); m_height = r.height(); }
	
	QRect toQRect() const{ return QRect(qRound(m_top),qRound(m_left),qRound(m_width),qRound(m_height)); }
	
	double top(){ return m_top; }
	double left(){ return m_left; }
	double width(){ return m_width; }
	double height(){ return m_height; }
	
	void setTop(double d){ m_top = d; }
	void setLeft(double d){ m_left = d; }
	void setWidth(double d){ m_width = d; }
	void setHeight(double d){ m_height = d; }

private:
	double m_top, m_left, m_width, m_height;
};

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
	
	enum Properties { None = 0, BackgroundColor = 1, TextColor = 2, Font = 4, Visibility = 8, Geometry = 16, Alignment = 32, StaticHeight = 64 };

	void saveLayout( const QString & );
	void loadLayout( const QString & );
	virtual QSize sizeHint(void) const;
	QSize minimumSize() const;
	bool hasChanges() const { return has_changes; };
	
	void setItemShown( QWidget *item, bool visible );

	const PropertiesMap properties() const { return *box_properties; }
	
signals:
	void itemVisibilityChanged( QWidget *, bool );

protected slots:
	void widgetClicked( QMouseEvent *, QWidget * );
	void changeMade();

	//slots to set properties of item boxes
	void setBackgroundColor();
	void setTextColor();
	void setFont();
	void setShown(int id);
	void setAlignment( QAction * );

private:

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

	PropertiesMap *box_properties;

	bool has_changes;
	KPopupMenu *popup;

	// Methods
	void createWidgets( const Recipe &sample );
	void toAbsolute(PreciseRect *);
	void toPercentage(PreciseRect *);

	void loadFont( QWidget *, const QDomElement &tag );
	void loadGeometry( QWidget *, const QDomElement &tag );
	void loadBackgroundColor( QWidget *, const QDomElement &tag );
	void loadTextColor( QWidget *, const QDomElement &tag );
	void loadVisibility( QWidget *, const QDomElement &tag );
	void loadAlignment( QWidget *, const QDomElement &tag );
};

#endif //SETUPDISPLAY_H

