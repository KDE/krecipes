/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
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
#include <qfont.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <iostream>

#include <kcursor.h>
#include <kglobalsettings.h>
#include <kimageeffect.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>

KreMenu::KreMenu(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
std::cerr<<"Creating menu widget\n";
Menu mainMenu; 
mainMenu.childPos=10; // Initial button is on top (10px), then keep scrolling down
mainMenu.widgetNumber=0; // Initially we have no buttons
mainMenu.activeButton=0; // Button that is highlighted
menus.append(mainMenu);

currentMenu=&(*(menus.begin()));
dragging=false;
setMouseTracking(true);
}


KreMenu::~KreMenu()
{
std::cerr<<"Destroying menu widget\n";
}

void KreMenu::highlightButton(KreMenuButton *button)
{
//Deactivate the old button
if (currentMenu->activeButton) 
	{
	currentMenu->activeButton->setActive(false);
	currentMenu->activeButton->update();
	}

//Activate the new button

button->setActive(true); button->update();
currentMenu->activeButton=button;
}

void KreMenu::mousePressEvent (QMouseEvent *e)
{
std::cerr<<"Mouse clicked on menu\n";
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
int x=e->x();
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


void KreMenu::mouseReleaseEvent (QMouseEvent *)
{
std::cerr<<"Mouse released over menu\n";
dragging=false;
}

QSize KreMenu::sizeHint() const {
  return(QSize(300,150));
}

void KreMenu::paintEvent(QPaintEvent *)
{
std::cerr<<"Painting menu\n";
    // Make sure the size is bigger than the minimum necessary
    if (minimumWidth() <45) setMinimumWidth(45); // FIXME: can somehow setMinimumWidth be restricted? This may not be the best place to do this
    
    // Get gradient colors
    QColor c=colorGroup().button();
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
    painter.drawRoundRect(0,0,width(),height(),(int)(2.0/width()*height()),2);
    painter.end();
    kpm.setMask(mask);

    //Draw the border line
    painter.begin(&kpm);
    painter.setPen(c1);
    painter.drawRoundRect(0,0,width(),height(),(int)(2.0/width()*height()),2);

    //Draw the top line bordering with the first button
    if (currentMenu->activeButton) // draw only if there's a button
    {
        int w=currentMenu->activeButton->width();
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
std::cerr<<"A child event in the menu widget\n";
	if (e->type()==QChildEvent::ChildInserted)
		{
		
		QObject *child=e->child();
		if (child->inherits("KreMenuButton"))
			{
			std::cerr<<"New child in menu\n";
			KreMenuButton* button=(KreMenuButton*)(e->child());
			if (!currentMenu->activeButton)  // Highlight the button if it's the first
				{
				button->setActive(true);
				currentMenu->activeButton=button;
				}
				
			currentMenu->addButton(button); 
			connect (button,SIGNAL(clicked(KreMenuButton*)),this,SLOT(collectClicks(KreMenuButton*)));
			if (!button->isShown()) button->show();
			}
		}
	else if (e->type()==QChildEvent::ChildRemoved)
		{
		QObject *child=e->child();
		
		std::cerr<<"A child button was destroyed. Lets remove?\n";
		KreMenuButton *button=(KreMenuButton*) child;
		if (currentMenu->positionList.find(button)!=currentMenu->positionList.end()) // Ensure that what was removed was a button
			{
			std::cerr<<"Removing child from menu\n";
			// Remove the button from the list first
			int pos=currentMenu->positionList[button];
			currentMenu->widgetList.remove(pos);
			currentMenu->positionList.remove(button);
	
			// Now recalculate the position of the next button
			(currentMenu->widgetNumber)--;
			std::cerr<<"Will be inserted after widget id.:"<<(currentMenu->widgetNumber)-1<<"\n";
			KreMenuButton *lastButton=currentMenu->widgetList[(currentMenu->widgetNumber)-1];
			if (lastButton) currentMenu->childPos=lastButton->y()+lastButton->height();
			currentMenu->activeButton=0;
			}
		
		}
QWidget::childEvent(e);
}

void KreMenu::collectClicks(KreMenuButton *w)
{
std::cerr<<"Menu clicked\n";

highlightButton(w);

// Emit signal indicating button activation with button ID
int widgetn=currentMenu->positionList[w];
emit clicked(widgetn);
}

void KreMenu::resizeEvent(QResizeEvent* e)
{
std::cerr<<"Menu was resized\n";
    emit resized((e->size()).width(), (e->size()).height());
}


KreMenuButton::KreMenuButton(QWidget *parent, const char *name):QWidget(parent, name)
{
std::cerr<<"Created new button\n";
icon=0;
highlighted=false;
text=QString::null;
resize(parent->size().width(),40);
connect (parent, SIGNAL(resized(int,int)), this, SLOT(rescale()));
connect(this,SIGNAL(clicked()),this,SLOT(forwardClicks()));
setCursor(QCursor(KCursor::handCursor()));
}


KreMenuButton::~KreMenuButton()
{
std::cerr<<"Destroyed a button\n";
}

void KreMenuButton::mousePressEvent (QMouseEvent *)
{
std::cerr<<"Clicked on a button\n";
emit clicked();
}

void KreMenuButton::rescale()
{
	resize(parentWidget()->width()-10,height());
}
QSize KreMenuButton::sizeHint() const
{
	if (parentWidget()) return(QSize(parentWidget()->size().width()-10,40));
	else return QSize(100,30);
}

void KreMenuButton::paintEvent(QPaintEvent *)
{

    // First draw the gradient
    int darken=130,lighten=120;
    QColor c1,c2,c1h,c2h; //non-highlighted and highlighted versions

    	// Set the gradient colors

    	c1=colorGroup().button().dark(darken);
    	c2=colorGroup().button().light(lighten);

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
    painter.setPen(colorGroup().button().dark(darken));
    painter.drawLine(width()/5,height()-2,width()-1,height()-2);
    painter.setPen(colorGroup().button().light(lighten));
    painter.drawLine(width()/5,height()-1,width()-1,height()-1);
    painter.end();
    
    // If it's highlighted, draw a rounded area for the icon and text

    	// Text and icon area
    	
	int areaw=width()-18,areah=height()-18;// -18=-16 (border)-2 (2 lines on top)	
	int areax=8, areay=8;
	int roundy=99, roundx=(int)((float)roundy*areah/areaw); //Make corners round
	
	// Make sure the area is sensible for text
	if (areah<fontMetrics().height()) 
		{
			areah=fontMetrics().height()+6;
			
			if (areah>(height()-4)) 
			{
			areah=height()-4; // Limit to button height
			}
			areay=(height()-areah-2)/2+1;
		}
	

	
    if (highlighted)
    {
    
	// Draw the gradient
	KPixmap area; area.resize(areaw,areah);
	
	
	KPixmapEffect::gradient(area,c2h.light(150),c1h.light(150), KPixmapEffect::VerticalGradient);
	
	painter.begin(&area);
	painter.setPen(c1h);
	painter.setBrush(Qt::NoBrush);
	painter.drawRoundRect(0,0,areaw,areah,roundx,roundy);
	painter.end();
	
	// Make it round
	QBitmap mask(QSize(areaw,areah));
	mask.fill(Qt::color0);
	painter.begin(&mask);
	painter.setPen(Qt::color1);
	painter.setBrush(Qt::color1);
	painter.drawRoundRect(0,0,areaw,areah,roundx,roundy);
	painter.end();
	area.setMask(mask);
	
	// Copy it to the button
	bitBlt(&kpm,areax,areay,&area);
	
	
    }
    
    // Now Add the icon
    
	painter.begin(&kpm);
	int xPos=0, yPos=0;

	if (icon)
	{

	// Calculate the icon's desired horizontal position
	
	xPos=areaw/5-10; if (xPos<areax+10) xPos=areax+10; // Try to maintain 10 px distance minimum
	
	
	// Make sure it fits in the area
	// If not, resize and reposition horizontally to be centered
	
	QPixmap scaledIcon=*icon;
	
	if (   (icon->height()>areah )  ||  (icon->width()>areaw/2)   ) // Nice effect, make sure you take less than half in width and fit in height (try making the menu very short in width)
		{
		QImage image; image=(*icon);
		scaledIcon.convertFromImage(image.smoothScale(areaw/2,areah,QImage::ScaleMin));
		
		// Center the icon horizontally
		xPos=areax+(areaw-scaledIcon.width())/2+1;
		
		}

	// Calculate the icon's vertical position
	
	yPos=(height()-scaledIcon.height())/2-1;
		

	// Now draw it
	
	painter.drawPixmap(xPos,yPos,scaledIcon);
	xPos+=scaledIcon.width(); // Move it so that later we can easily place the text
	}

    // Finally, draw the text besides the icon
    xPos+=15;
    QRect r=rect(); r.setLeft(xPos); r.setWidth(areaw-xPos);
    
    painter.setPen(QColor(0x00,0x00,0x00));
    painter.setClipRect(r);
    painter.drawText(r,Qt::AlignVCenter,text);

    painter.end();

    // Copy the offscreen button to the widget
    bitBlt(this, 0, 0, &kpm,0,0,width(),height()); // Copy the image with correct button size (button is already smaller than parent in width to leave space for the handle, so no need to use -10)

}

void KreMenuButton::setIconSet(const QIconSet &is)
{
	icon = new QPixmap(is.pixmap(QIconSet::Small,QIconSet::Normal,QIconSet::On));
}

Menu::Menu(void)
{
}


Menu::Menu(const Menu &m)
{
std::cerr<<"New menu list\n";
activeButton=m.activeButton;
childPos=m.childPos;
widgetNumber=m.widgetNumber;

copyMap(positionList,m.positionList);
copyMap(widgetList,m.widgetList);
}

Menu::~Menu(void)
{
std::cerr<<"Destroyed menu list\n";
}

Menu& Menu::operator=(const Menu &m)
{

activeButton=m.activeButton;
childPos=m.childPos;
widgetNumber=m.widgetNumber;

copyMap(positionList,m.positionList);
copyMap(widgetList,m.widgetList);

return *this;
}


void Menu::addButton(KreMenuButton* button)
{
std::cerr<<"Adding button to menu list\n";
	button->move(0,childPos);
	button->rescale();
	childPos+=button->height();
	positionList[button]=widgetNumber; // Store index for this widget, and increment number
	widgetList[widgetNumber]=button; // Store the button in the list (the inverse mapping of the previous one)
	widgetNumber++;
}

void Menu::copyMap(QMap <int,KreMenuButton*> &destMap, const QMap <int,KreMenuButton*> &origMap)
{
	QMap<int,KreMenuButton*>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) 
	{
	destMap[it.key()]=it.data();
	}
}

void Menu::copyMap(QMap <KreMenuButton*,int> &destMap, const QMap <KreMenuButton*,int> &origMap)
{
	QMap<KreMenuButton*,int>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) 
	{
	destMap[it.key()]=it.data();
	}
}