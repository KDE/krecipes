/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as publishfed by *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "kremenu.h"

#include <QBitmap>
#include <QCursor>
#include <qfont.h>
#include <QImage>
#include <QObject>
#include <QPainter>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QChildEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>

#include <kapplication.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kicon.h>
#include <klocale.h>
#include <QPixmap>


KreMenu::KreMenu( QWidget *parent, const char *name ):
	QWidget( parent, Qt::WNoAutoErase )
{
	setObjectName( name );
	Menu newMenu;

	mainMenuId = menus.insert( menus.end(), newMenu );

	currentMenuId = mainMenuId;
	m_currentMenu = &( *currentMenuId );
	setMouseTracking( true );
	setFocusPolicy( Qt::StrongFocus );
	setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
}


KreMenu::~KreMenu()
{
}

void KreMenu::childEvent ( QChildEvent *e )
{
	if ( e->type() == QChildEvent::ChildInserted ) {
		QObject * child = e->child();
		if ( child->inherits( "KreMenuButton" ) ) {
			KreMenuButton * button = static_cast<KreMenuButton*>( child );
			Menu *buttonMenu = &( *( button->menuId ) );
			if ( !( buttonMenu->activeButton ) )   // Highlight the button if it's the first in the menu
			{
				button->setActive( true );
				buttonMenu->activeButton = button;
			}
			buttonMenu->addButton( button );

			if ( (buttonMenu != m_currentMenu) || !(button->isEnabled()) )
				button->hide();
			else
				button->show();

			connect ( button, SIGNAL( clicked( KreMenuButton* ) ), this, SLOT( collectClicks( KreMenuButton* ) ) );
		}
	}
	else if ( e->type() == QChildEvent::ChildRemoved ) {
		QObject * child = e->child();
		KreMenuButton *button = ( KreMenuButton* ) child;
		if ( m_currentMenu->positionList.find( button ) != m_currentMenu->positionList.end() )  // Ensure that what was removed was a button
		{
			// Remove the button from the list first
			int pos = m_currentMenu->positionList[ button ]; // FIXME: this works only if the button is removed from the main menu
			m_currentMenu->widgetList.remove( pos ); // FIXME: this works only if the button is removed from the main menu
			m_currentMenu->positionList.remove( button ); // FIXME: this works only if the button is removed from the main menu

			// Now recalculate the position of the next button
			( m_currentMenu->widgetNumber ) --; // FIXME: this works only if the button is removed from the main menu

			KreMenuButton *lastButton = m_currentMenu->widgetList[ ( m_currentMenu->widgetNumber ) - 1 ];
			if ( lastButton )
				m_currentMenu->childPos = lastButton->y() + lastButton->height();
			m_currentMenu->activeButton = 0;

			setMinimumWidth( minimumSizeHint().width() + 10 ); //update the minimum width
			}
	}
	QWidget::childEvent( e );
}

void KreMenu::collectClicks( KreMenuButton *w )
{
	setFocus();

	highlightButton( w );

	// Emit signal indicating button activation with button ID
	KrePanel panel = w->getPanel();
	emit clicked( panel );
}

MenuId KreMenu::createSubMenu( const QString &title, const QString &icon )
{

	// Create the new menu
	Menu newMenu;
	MenuId id = menus.insert( menus.end(), newMenu );

	// Add a button to the main menu for this submenu
	KreMenuButton *newMenuButton = new KreMenuButton( this );
	newMenuButton->subMenuId = id;
	newMenuButton->setTitle( title );
	newMenuButton->setIconSet( KIcon( icon ) );

	// Add a button to the submenu to go back to the top menu
	KreMenuButton *newSubMenuButton = new KreMenuButton( this );
	newSubMenuButton->menuId = id;
	newSubMenuButton->subMenuId = mainMenuId;
	newSubMenuButton->setTitle( i18nc("@action:button Up to top menu",  "Up..." ) );
	newSubMenuButton->setIconSet( KIcon( "arrow-up" ) );

	connect( newMenuButton, SIGNAL( clicked( MenuId ) ), this, SLOT( showMenu( MenuId ) ) );
	connect( newSubMenuButton, SIGNAL( clicked( MenuId ) ), this, SLOT( showMenu( MenuId ) ) );


	return id;
}

void KreMenu::highlightButton( KreMenuButton *button )
{
	MenuId buttonMenuId = button->menuId;
	Menu *buttonMenu = &( *buttonMenuId );

	//Deactivate the old button
	if ( buttonMenu->activeButton ) {
		buttonMenu->activeButton->setActive( false );
		buttonMenu->activeButton->update();
	}

	//Activate the new button

	button->setActive( true );
	button->update();
	buttonMenu->activeButton = button;
}

void KreMenu::keyPressEvent( QKeyEvent *e )
{
	switch ( e->key() ) {
	case Qt::Key_Up: {
		int current_index = m_currentMenu->positionList[ m_currentMenu->activeButton ];
		if ( current_index > 0 ) {
			highlightButton( m_currentMenu->widgetList[ current_index - 1 ] );

			//simulate a mouse click
			QMouseEvent me( QEvent::MouseButtonPress, QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier );
			KApplication::sendEvent( m_currentMenu->activeButton, &me );

			e->accept();
		}
		break;
	}
	case Qt::Key_Down: {
		int current_index = m_currentMenu->positionList[ m_currentMenu->activeButton ];
		if ( current_index < int( m_currentMenu->positionList.count() ) - 1 ) {
			highlightButton( m_currentMenu->widgetList[ current_index + 1 ] );

			//simulate a mouse click
			QMouseEvent me( QEvent::MouseButtonPress, QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
			KApplication::sendEvent( m_currentMenu->activeButton, &me );

			e->accept();
		}
		break;
	}
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Space: {
		//simulate a mouse click
		QMouseEvent me( QEvent::MouseButtonPress, QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier );
		KApplication::sendEvent( m_currentMenu->activeButton, &me );

		e->accept();
		break;
	}
	default:
		e->ignore();
	}
}

QSize KreMenu::sizeHint() const
{
	return minimumSizeHint();
}

//the minimum size hint will be the minimum size hint of the largest child
QSize KreMenu::minimumSizeHint() const
{
	int width = 30;

	const QList<QObject *> childElements = queryList( 0, 0, false, false ); //only first-generation children (not recursive)
	foreach ( QObject *obj, childElements )
	{
		if ( obj->isWidgetType() ) {
			int obj_width_hint = ( ( QWidget* ) obj ) ->minimumSizeHint().width();
			if ( obj_width_hint > width )
				width = obj_width_hint;
		}
	}
	return QSize(width, 150);
}

void KreMenu::paintEvent( QPaintEvent * )
{

	// Make sure the size is bigger than the minimum necessary
	//if (minimumWidth() <45) setMinimumWidth(45); // FIXME: can somehow setMinimumWidth be restricted? This may not be the best place to do this

	// Get gradient colors
	QColor c = QColorGroup( QPalette() ).color(QPalette::Button);
	QColor c1 = c.dark( 130 );
	QColor c2 = c.light( 120 );

	// Draw the handle
	QPainter painter(this );
	painter.setPen( c1 );
	painter.drawLine( width() - 5, 20, width() - 5, height() - 20 );

	QLinearGradient linearGrad(QPointF(0, 0), QPointF(width(), 0) );
	linearGrad.setColorAt(0, c1);
	linearGrad.setColorAt(1, c2);
	QBrush brush( linearGrad );
	painter.fillRect( QRect( 0, 0, width(), height() ),brush );

	/*painter.setPen( Qt::color1 );
	painter.setBrush( Qt::color1 );
	painter.drawRoundRect( 0, 0, width(), height(), ( int ) ( 2.0 / width() * height() ), 2 );

	painter.setPen( c1 );
	painter.drawRoundRect( 0, 0, width(), height(), ( int ) ( 2.0 / width() * height() ), 2 );*/

	//Draw the top line bordering with the first button
	if ( m_currentMenu->activeButton )  // draw only if there's a button
	{
		int w = m_currentMenu->activeButton->width();
		painter.setPen( c1 );
		painter.drawLine( w / 5, 8, w - 1, 8 );
		painter.setPen( c2 );
		painter.drawLine( w / 5, 9, w - 1, 9 );
	}
}

void KreMenu::resizeEvent( QResizeEvent* e )
{
	emit resized( ( e->size() ).width(), ( e->size() ).height() );
}

void KreMenu::showMenu( MenuId id )
{
	// Hide the buttons in the current menu
	// and show the ones in the new menu

	foreach ( QObject *obj, children() )
	{
		if ( obj->inherits( "KreMenuButton" ) ) {
		KreMenuButton * button = static_cast<KreMenuButton*>( obj );
		if ( button->menuId == currentMenuId )
			button->hide();
		else if ( (button->menuId == id) && (button->isEnabled()) )
			button->show();
		}
	}

	// Set the new menu as current
	currentMenuId = id;
	m_currentMenu = &( *( currentMenuId ) );
}


KreMenuButton::KreMenuButton( KreMenu *parent, KrePanel _panel, MenuId id ):
	QWidget( parent, Qt::WNoAutoErase ), panel( _panel )
{
	highlighted = false;
	text.clear();

	if ( id == Q3ValueList <Menu>::Iterator() ) // KDE4 port to be check
	{
		menuId = parent->mainMenu();
	}
	else
		menuId = id;

	subMenuId = MenuId(); // By default it's not a submenu button

	resize( parent->size().width(), 55 );
	connect ( parent, SIGNAL( resized( int, int ) ), this, SLOT( rescale() ) );
	connect( this, SIGNAL( clicked() ), this, SLOT( forwardClicks() ) );
	setCursor( QCursor( Qt::PointingHandCursor ) );
}


KreMenuButton::~KreMenuButton()
{
}

void KreMenuButton::setTitle( const QString &s )
{
	text = s;

#if 0 //this causes problems for the button to go back to editing a recipe
	//adjust text to two lines if needed
	if ( fontMetrics().width( text ) > 110 ) {
		text.replace( ' ', "\n" );
	}
#endif

	setMinimumWidth( minimumSizeHint().width() );
	if ( parentWidget() ->minimumWidth() < minimumSizeHint().width() )
		parentWidget() ->setMinimumWidth( minimumSizeHint().width() + 10 );

	update();
}

void KreMenuButton::mousePressEvent ( QMouseEvent * )
{
	emit clicked();
}

void KreMenuButton::rescale()
{
	resize( parentWidget() ->width() - 10, height() );
}
QSize KreMenuButton::sizeHint() const
{
	if ( parentWidget() )
		return ( QSize( parentWidget() ->size().width() - 10, 40 ) );
	else
		return QSize( 100, 30 );
}

QSize KreMenuButton::minimumSizeHint() const
{
	int text_width = qMax( fontMetrics().width( text.section( '\n', 0, 0 ) ), fontMetrics().width( text.section( '\n', 1, 1 ) ) );

	if ( !icon.isNull() )
		return QSize( 40 + icon.width() + text_width, 30 );
	else
		return QSize( 40 + text_width, 30 );
}

void KreMenuButton::paintEvent( QPaintEvent * )
{
	if ( !isVisible() )
		return ;
	// First draw the gradient
	int darken = 130, lighten = 120;
	QColor c1, c2, c1h, c2h; //non-highlighted and highlighted versions
	QPalette palette;

	// Set the gradient colors

	c1 = QColorGroup( QPalette() ).color(QPalette::Button).dark( darken );
	c2 = QColorGroup( QPalette() ).color(QPalette::Button).light( lighten );

	if ( highlighted ) {
		darken -= 10;
		lighten += 10;

		c1h = palette.highlight().color().dark( darken );
		c2h = palette.highlight().color().light( lighten );
	}

	QPainter painter(this);

	// draw the gradient now
	if ( !highlighted ) {

		// first the gradient
		QLinearGradient g(0,0,width(),0);
		g.setColorAt(0, c1);
		g.setColorAt(1, c2);
                    QBrush b(g);
                    painter.fillRect(rect(), b);

	}
	else {
                    {
                    // top gradient (highlighted)
                        QLinearGradient g(0,0,width(),0);
                        g.setColorAt(0, c1h);
                        g.setColorAt(1, c2h);
                        QBrush b(g);
                        painter.fillRect(QRectF(0,0,width(), height()-2), b);
                    }

	}

	// Draw the line
          painter.save();
          painter.setPen( QColorGroup( QPalette() ).color(QPalette::Button).dark( darken ) );
          painter.drawLine( width() / 5, height() - 2, width() - 1, height() - 2 );
          painter.setPen( QColorGroup( QPalette() ).color(QPalette::Button).light( lighten ) );
          painter.drawLine( width() / 5, height() - 1, width() - 1, height() - 1 );
          painter.restore();

	// Now Add the icon

	int xPos = 0, yPos = 0;
	if ( !icon.isNull() ) {
		// Set the icon's desired horizontal position

		xPos = 10;
		yPos = 0;


		// Make sure it fits in the area
		// If not, resize and reposition horizontally to be centered

		QPixmap scaledIcon;

		if ( ( icon.height() > height() ) || ( icon.width() > width() / 3 ) )  // Nice effect, make sure you take less than half in width and fit in height (try making the menu very short in width)
		{
			QImage image = icon.toImage();
			scaledIcon.fromImage( image.scaled( QSize( width() / 3, height() ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
		} else {
			scaledIcon = icon;
		}

		// Calculate the icon's vertical position

		yPos = ( height() - scaledIcon.height() ) / 2 - 1;


		// Now draw it

                    painter.drawPixmap( xPos, yPos, scaledIcon );

		xPos += scaledIcon.width(); // increase it to place the text area correctly
	}

	// If it's highlighted, draw a rounded area around the text

	// Calculate the rounded area

	int areax = xPos + 10;
	int areah = fontMetrics().height() * ( text.count( '\n' ) + 1 ) + fontMetrics().lineSpacing() * text.count( '\n' ) + 6; // Make sure the area is sensible for text and adjust for multiple lines

	int areaw = width() - areax - 10;

	if ( areah > ( height() - 4 ) ) {
		areah = height() - 4; // Limit to button height
	}

	int areay = ( height() - areah - 2 ) / 2 + 1; // Center the area vertically


	// Calculate roundness

	int roundy = 99, roundx = ( int ) ( ( float ) roundy * areah / areaw ); //Make corners round


	if ( highlighted && areaw > 0 )  // If there is no space for the text area do not draw it
	{

		// Draw the gradient
		QPixmap area(QSize( areaw, areah ));

                    {
                        QPainter highlightPainter(&area);

                        QLinearGradient linearGrad(0,0,0,area.height());
                        linearGrad.setColorAt(0, c1h.light( 150 ) );
                        linearGrad.setColorAt(1, c2h.light( 150 ) );

                        highlightPainter.fillRect(area.rect(), QBrush(linearGrad));
                        highlightPainter.setPen( c1h );
                        highlightPainter.setBrush( Qt::NoBrush );
                        highlightPainter.drawRoundRect( 0, 0, areaw, areah, roundx, roundy );
                    }
		// Make it round
		QBitmap mask( QSize( areaw, areah ) );
		mask.fill( Qt::color0 );

                    QPainter maskPainter(&mask);
		maskPainter.begin( &mask );
		maskPainter.setPen( Qt::color1 );
		maskPainter.setBrush( Qt::color1 );
		maskPainter.drawRoundRect( 0, 0, areaw, areah, roundx, roundy );
		maskPainter.end();
		area.setMask( mask );

                    painter.drawPixmap(areax, areay,area);
	}

	// Finally, draw the text besides the icon
	QRect r = rect();
	r.setLeft( areax + 5 );
	r.setWidth( areaw - 10 );

	if ( highlighted )
          {
                    painter.setPen(palette.highlight().color() );
          }
	else
          {
                    painter.setPen(palette.text().color() );
          }
          painter.setClipRect(r);
          painter.drawText(r, Qt::AlignVCenter, text);

}

void KreMenuButton::setIconSet( const QIcon &is )
{
	icon = is.pixmap((style()->pixelMetric(QStyle::PM_SmallIconSize) + style()->pixelMetric(QStyle::PM_LargeIconSize))/2 , QIcon::Normal, QIcon::On );

	setMinimumWidth( minimumSizeHint().width() );
	if ( parentWidget() ->minimumWidth() < minimumSizeHint().width() )
		parentWidget() ->setMinimumWidth( minimumSizeHint().width() + 10 );
}

Menu::Menu( void )
{
	childPos = 10; // Initial button is on top (10px), then keep scrolling down
	widgetNumber = 0; // Initially we have no buttons
	activeButton = 0; // Button that is highlighted
}


Menu::Menu( const Menu &m )
{
	activeButton = m.activeButton;
	childPos = m.childPos;
	widgetNumber = m.widgetNumber;

	copyMap( positionList, m.positionList );
	copyMap( widgetList, m.widgetList );
}

Menu::~Menu( void )
{}

Menu& Menu::operator=( const Menu &m )
{

	activeButton = m.activeButton;
	childPos = m.childPos;
	widgetNumber = m.widgetNumber;

	copyMap( positionList, m.positionList );
	copyMap( widgetList, m.widgetList );

	return *this;
}


void Menu::addButton( KreMenuButton* button )
{
	button->move( 0, childPos );
	button->rescale();
	childPos += button->height();
	positionList[ button ] = widgetNumber; // Store index for this widget, and increment number
	widgetList[ widgetNumber ] = button; // Store the button in the list (the inverse mapping of the previous one)
	widgetNumber++;
}


void Menu::copyMap( QMap <int, KreMenuButton*> &destMap, const QMap <int, KreMenuButton*> &origMap )
{
	QMap<int, KreMenuButton*>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) {
		destMap[ it.key() ] = it.value();
	}
}

void Menu::copyMap( QMap <KreMenuButton*, int> &destMap, const QMap <KreMenuButton*, int> &origMap )
{
	QMap<KreMenuButton*, int>::ConstIterator it;
	destMap.clear();
	for ( it = origMap.begin(); it != origMap.end(); ++it ) {
		destMap[ it.key() ] = it.value();
	}
}

#include "kremenu.moc"
