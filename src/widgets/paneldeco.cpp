/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro (ugarro@users.sourceforge.net)       *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "paneldeco.h"

#include <qbitmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpixmap.h>

#include <kglobalsettings.h>
#include <kimageeffect.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>


// Panel decoration

PanelDeco::PanelDeco(QWidget *parent, const char *name):QVBox(parent, name)
{

// Top decoration
tDeco=new TopDeco(this,"TopDecoration");

hbox=new QHBox(this);

//Left decoration
lDeco=new LeftDeco(hbox,"LeftDecoration");

//The widget stack (panels)
stack=new QWidgetStack(hbox);
stack->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));


}


PanelDeco::~PanelDeco()
{
}

void PanelDeco::childEvent(QChildEvent *e)
{
	if (e->type()==QEvent::ChildInserted)
		{
		QObject *obj=e->child();
		if (obj->inherits("QWidget"))
			{
			QWidget *w=(QWidget*)obj;
			if (w!=hbox && w!=tDeco) w->reparent(stack,QPoint(0,0));
			}
		}
}


int PanelDeco::id(QWidget* w)
{
return (stack->id(w));
}

void PanelDeco::raise(QWidget *w)
{
stack->raiseWidget(w);
}

QWidget* PanelDeco::visiblePanel(void)
{
return (stack->visibleWidget());
}

// Left part of the decoration

LeftDeco::LeftDeco(QWidget *parent, const char *name): QWidget(parent, name)
{
}

LeftDeco::~LeftDeco()
{
}

// Top part of the decoration

TopDeco::TopDeco(QWidget *parent, const char *name): QWidget(parent, name)
{
}

TopDeco::~TopDeco()
{
}
