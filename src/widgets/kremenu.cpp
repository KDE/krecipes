/***************************************************************************
 *   Copyright (C) 2003 by Unai Garro (ugarro@users.sourceforge.net)       *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kremenu.h"

#include <qbitmap.h>
#include <qcursor.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <iostream.h>

#include <kcursor.h>
#include <kglobalsettings.h>
#include <kimageeffect.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

KreMenu::KreMenu(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
childPos=10; // Initial button is on top (10px), then keep scrolling down
widgetNumber=0; // Initially we have no buttons
activeButton=0; // Button that is highlighted
dragging=false;
setMouseTracking(true);
}


KreMenu::~KreMenu()
{
}

void KreMenu::mousePressEvent (QMouseEvent *e)
{
int x=e->x(),y=e->y();
if (x > (width()-15))
	{
	xOrig=x;
	yOrig=y;
	}
dragging=true;
}

void KreMenu::mouseMoveEvent (QMouseEvent *e)
{


// Set the mouse cursor in any case
int x=e->x(),y=e->y();
if (x > (width()-15))
	{

	if (QT_VERSION>=0x030200)
		{
		setCursor(QCursor(Qt::SplitHCursor));
		}
	else
		{
		setCursor(QCursor(Qt::SplitVCursor));
		}
	}
else
	{
	setCursor(QCursor(Qt::ArrowCursor));
	}
// If already dragging, resize
if (dragging)
	{
	xDest=e->x();
	yDest=e->y();

	if (xDest>xOrig) // Increase menu size
		{
		int xIncrease=xDest-xOrig;
		if ((width()+xIncrease) < maximumWidth())
			{
			resize(width()+xIncrease,height());
			xOrig=xDest;yOrig=yDest;
			}
		else
			{
			resize(maximumWidth(),height());
			xOrig=xDest;yOrig=yDest;
			}
		}
	else if (xDest<xOrig) // Reduce menu size
		{
		int xDecrease=xOrig-xDest;
		if ((width()-xDecrease) > minimumWidth())
			{
			resize(width()-xDecrease,height());
			xOrig=xDest;yOrig=yDest;
			}
		else
			{
			resize(minimumWidth(),height());
			xOrig=xDest;yOrig=yDest;
			}
		}

	}
}


void KreMenu::mouseReleaseEvent (QMouseEvent *e)
{
dragging=false;
}

QSize KreMenu::sizeHint() const {
  return(QSize(300,150));
}

void KreMenu::paintEvent(QPaintEvent *e )
{
    // Get gradient colors
    QColor c=KGlobalSettings::baseColor();
    QColor c1=c.dark(130);
    QColor c2=c.light(120);

    // Draw the gradient
    KPixmap kpm;kpm.resize(size()); KPixmapEffect::unbalancedGradient (kpm,c2,c1, KPixmapEffect::HorizontalGradient,-150,-150);

    // Draw the handle
    QPainter painter(&kpm);
    painter.setPen(c1);
    painter.drawLine(width()-5,20,width()-5,height()-20);
    painter.end();

    //Set the border transparent using a mask
    QBitmap mask(kpm.size());
    mask.fill(Qt::color0);
    painter.begin(&mask);
    painter.setPen(Qt::color1);
    painter.setBrush(Qt::color1);
    painter.drawRoundRect(0,0,width(),height(),2.0/width()*height(),2);
    painter.end();
    kpm.setMask(mask);

    //Draw the border line
    painter.begin(&kpm);
    painter.setPen(c1);
    painter.drawRoundRect(0,0,width(),height(),2.0/width()*height(),2);

    //Draw the top line bordering with the first button
    if (activeButton) // draw only if there's a button
    {
        int w=activeButton->width();
	painter.setPen(c1);
	painter.drawLine(w/5,8,w-1,8);
	painter.setPen(c2);
	painter.drawLine(w/5,9,w-1,9);
	painter.end();
    }

    // Copy the pixmap to the widget
    bitBlt(this, 0, 0, &kpm);
    }

void KreMenu::childEvent (QChildEvent *e)
{

QObject *child=e->child();
if (child->inherits("KreMenuButton"))
	{
	KreMenuButton* button=(KreMenuButton*)(e->child());
	if (!activeButton)  // Highlight the button if it's the firsts
		{
		button->setActive(true);
		activeButton=button;
		}

	button->move(0,childPos);
	childPos+=button->height();
	widgetList[button]=widgetNumber; widgetNumber++; // Store index for this widget, and increment number
	connect (button,SIGNAL(clicked(KreMenuButton*)),this,SLOT(collectClicks(KreMenuButton*)));
	}

}

void KreMenu::collectClicks(KreMenuButton *w)
{

//Deactivate the old button
activeButton->setActive(false);
activeButton->update();

//Activate the new button

int widgetn=widgetList[w];
w->setActive(true); w->update();
activeButton=w;

// Emit signal indicating button activation with button ID

emit clicked(widgetn);
}

void KreMenu::resizeEvent(QResizeEvent* e)
{
    emit resized((e->size()).width(), (e->size()).height());
}


KreMenuButton::KreMenuButton(QWidget *parent, const char *name):QWidget(parent, name)
{
icon=0;
highlighted=false;
text=QString::null;
resize(parent->size().width(),40);
connect (parent, SIGNAL(resized(int,int)), this, SLOT(rescale(int,int)));
connect(this,SIGNAL(clicked()),this,SLOT(forwardClicks()));
setCursor(QCursor(KCursor::handCursor()));
}


KreMenuButton::~KreMenuButton()
{
}

void KreMenuButton::mousePressEvent (QMouseEvent *e)
{
emit clicked();
}

void KreMenuButton::rescale(int w, int h)
{
	resize(w-10,height()); // Leave space for the handle
}

QSize KreMenuButton::sizeHint() const
{
	if (parentWidget()) return(QSize(parentWidget()->size().width()-10,40));
}

void KreMenuButton::paintEvent(QPaintEvent *e )
{

    // First draw the gradient
    int darken=130,lighten=120;
    QColor c1,c2,c1h,c2h; //non-highlighted and highlighted versions

    	// Set the gradient colors

    	c1=KGlobalSettings::baseColor().dark(darken);
    	c2=KGlobalSettings::baseColor().light(lighten);

    	if (highlighted)
    	{
    	darken-=10; lighten+=10;
    	c1h=KGlobalSettings::highlightColor().dark(darken);
    	c2h=KGlobalSettings::highlightColor().light(lighten);
    	}

	// draw the gradient now

    QPainter painter; KPixmap kpm; kpm.resize(((QWidget *)parent())->size()); // use parent's same size to obtain the same gradient

    if (!highlighted)
    {

    // first the gradient
    KPixmapEffect::unbalancedGradient (kpm,c2,c1, KPixmapEffect::HorizontalGradient,-150,-150);

    }
    else
    {

    // top gradient (highlighted)
    kpm.resize(width(),height()); KPixmapEffect::unbalancedGradient (kpm,c2h,c1h, KPixmapEffect::HorizontalGradient,-150,-150);
    // low gradient besides the line (not hightlighted)
    KPixmap kpmb;kpmb.resize(width(),2); KPixmapEffect::unbalancedGradient (kpmb,c2,c1, KPixmapEffect::HorizontalGradient,-150,-150);
    // mix the two
    bitBlt(&kpm,0,height()-2,&kpmb);

    }

    // Draw the line
    painter.begin(&kpm);
    painter.setPen(KGlobalSettings::baseColor().dark(darken));
    painter.drawLine(width()/5,height()-2,width()-1,height()-2);
    painter.setPen(KGlobalSettings::baseColor().light(lighten));
    painter.drawLine(width()/5,height()-1,width()-1,height()-1);


    // Now Add the icon

    int xPos=0, yPos=0;

    if (icon)
    {


	// Make sure it fits in
	QPixmap scaledIcon=*icon;
	if (   (icon->height()>height() )  ||  (icon->width()>width()/2)   ) // Nice effect, make sure you take less than half in width and fit in height (try making the menu very short in width)
		{
		QImage image; image=(*icon);
		scaledIcon.convertFromImage(image.smoothScale(width()/2,height(),QImage::ScaleMin));
		}


	// Now draw it
	xPos=width()/5-10; if (xPos<2) xPos=2;
	yPos=(height()-scaledIcon.height())/2-1;
	painter.drawPixmap(xPos,yPos,scaledIcon);
	xPos+=scaledIcon.width(); // Move it so that later we can easily place the text
    }

    // Finally, draw the text besides the icon
    xPos+=15;
    QRect r=rect(); r.setLeft(xPos);
    painter.setPen(QColor(0x00,0x00,0x00));
    painter.drawText(r,Qt::AlignVCenter,text);




    painter.end();

    // Copy the offscreen button to the widget
    bitBlt(this, 0, 0, &kpm,0,0,width(),height()); // Copy the image with correct button size (button is already smaller than parent in width to leave space for the handle, so no need to use -10)

}

void KreMenuButton::setIconSet(const QIconSet &is)
{
	icon = new QPixmap(is.pixmap(QIconSet::Small,QIconSet::Normal,QIconSet::On));
}
