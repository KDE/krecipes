/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "setupdisplay.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kfontdialog.h>
#include <kcolordialog.h>
#include <klocale.h>

#include <qsimplerichtext.h>
#include <qaction.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>

#include "../image.h"
#include "../mixednumber.h"

#include <cmath>

SetupDisplay::SetupDisplay( const Recipe &sample, QWidget *parent ) : DragArea( parent ),
  box_properties(new QMap< QWidget*, unsigned int >)
{
	resize(300,400);
	setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );

	box_properties->insert( this, BackgroundColor | Geometry );

	connect( this, SIGNAL(widgetClicked(QMouseEvent*,QWidget*)), SLOT(widgetClicked(QMouseEvent*,QWidget*)) );

	createWidgets( sample );
	loadSetup();

}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
}

void SetupDisplay::createSetupIfNecessary()
{
	KConfig *config=kapp->config();

	if ( !config->hasGroup("BackgroundSetup") )
	{
		config->setGroup( "BackgroundSetup" );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "Geometry", QRect(0,0,300,400) );
	}

	if ( !config->hasGroup("TitleSetup") )
	{
		config->setGroup( "TitleSetup" );
		config->writeEntry( "Geometry", QRect(31,6,68,5) );
		QFont font = kapp->font();
		font.setBold( true );
		font.setPointSize( font.pointSize() * 2 );
		config->writeEntry( "Font", font );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignHCenter | Qt::WordBreak );
	}

	if ( !config->hasGroup("InstructionsSetup") )
	{
		config->setGroup("InstructionsSetup");
		config->writeEntry( "Geometry", QRect(31,28,68,60) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::WordBreak );
	}

	if ( !config->hasGroup("ServingsSetup") )
	{
		config->setGroup("ServingsSetup");
		config->writeEntry( "Geometry", QRect(31,22,68,5) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::WordBreak );
	}

	if ( !config->hasGroup("PhotoSetup") )
	{
		config->setGroup("PhotoSetup");
		config->writeEntry( "Geometry", QRect(0,0,28,15) );
		config->writeEntry( "Visibility", true );
	}

	if ( !config->hasGroup("AuthorsSetup") )
	{
		config->setGroup("AuthorsSetup");
		config->writeEntry( "Geometry", QRect(31,17,68,4) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak );
	}

	if ( !config->hasGroup("CategoriesSetup") )
	{
		config->setGroup("CategoriesSetup");
		config->writeEntry( "Geometry", QRect(31,12,68,4) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", Qt::white );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak );
	}

	if ( !config->hasGroup("HeaderSetup") )
	{
		config->setGroup("HeaderSetup");
		config->writeEntry( "Geometry", QRect(73,0,25,5) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", QColor(238,218,156) );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignRight | Qt::WordBreak );
	}

	if ( !config->hasGroup("IngredientsSetup") )
	{
		config->setGroup("IngredientsSetup");
		config->writeEntry( "Geometry", QRect(0,17,29,18) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", QColor(119,109,78) );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak );
	}

	if ( !config->hasGroup("PropertiesSetup") )
	{
		config->setGroup("PropertiesSetup");
		config->writeEntry( "Geometry", QRect(0,36,29,18) );
		config->writeEntry( "Font", kapp->font() );
		config->writeEntry( "BackgroundColor", QColor(238,218,156) );
		config->writeEntry( "TextColor", Qt::black );
		config->writeEntry( "Visibility", true );
		config->writeEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak );
	}
}

//do not call until createWidgets() has been called!
void SetupDisplay::loadSetup()
{

QSize newsize;
QPoint newposition;

	SetupDisplay::createSetupIfNecessary();

	//TODO: we have just one setup available for now. Later setup may be a separate file
	KConfig *config=kapp->config();

	//=========================this=======================//
	config->setGroup("BackgroundSetup");
 	setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));

	QRect r=config->readRectEntry( "Geometry" );
	m_size =r.size() ;

	//=========================TITLE=======================//
	config->setGroup("TitleSetup");

	r=config->readRectEntry( "Geometry" );toAbsolute(&r);
	title_box->setGeometry(r);
	title_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	title_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	title_box->setFont(config->readFontEntry( "Font" ) );
	title_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	title_box->setAlignment(config->readNumEntry( "Alignment" ) );

	//======================INSTRUCTIONS===================//

	config->setGroup("InstructionsSetup");
	r=config->readRectEntry( "Geometry" ); toAbsolute(&r);
	instr_box->setGeometry(r);
	instr_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	instr_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	instr_box->setFont(config->readFontEntry( "Font" ) );
	instr_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	instr_box->setAlignment(config->readNumEntry( "Alignment" ));

	//=======================SERVINGS======================//
	config->setGroup("ServingsSetup");

	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	servings_box->setGeometry(r);
	servings_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	servings_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	servings_box->setFont(config->readFontEntry( "Font" ) );
	servings_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	servings_box->setAlignment(config->readNumEntry( "Alignment" ));

	//========================PHOTO========================//
	config->setGroup("PhotoSetup");

	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	photo_box->setGeometry(r);
	photo_box->setEnabled(config->readBoolEntry( "Visibility" ) );

	//=======================AUTHORS======================//
	config->setGroup("AuthorsSetup");
	r=config->readRectEntry("Geometry"); toAbsolute(&r);

	authors_box->setGeometry(r);
	authors_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	authors_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	authors_box->setFont(config->readFontEntry( "Font" ) );
	authors_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	authors_box->setAlignment(config->readNumEntry( "Alignment" ));

	//=======================CATEGORIES======================//
	config->setGroup("CategoriesSetup");
	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	categories_box->setGeometry(r);
	categories_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	categories_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	categories_box->setFont(config->readFontEntry( "Font" ) );
	categories_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	categories_box->setAlignment(config->readNumEntry( "Alignment" ));

	//=======================ID======================//
	config->setGroup("HeaderSetup");
	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	id_box->setGeometry(r);
	id_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	id_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	id_box->setFont(config->readFontEntry( "Font" ) );
	id_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	id_box->setAlignment(config->readNumEntry( "Alignment" ));

	//=======================INGREDIENTS======================//
	config->setGroup("IngredientsSetup");
	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	ingredients_box->setGeometry(r);
	ingredients_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	ingredients_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	ingredients_box->setFont(config->readFontEntry( "Font" ) );
	ingredients_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	ingredients_box->setAlignment(config->readNumEntry( "Alignment" ));

	//=======================INGREDIENTS======================//
	config->setGroup("PropertiesSetup");
	r=config->readRectEntry("Geometry"); toAbsolute(&r);
	properties_box->setGeometry(r);
	properties_box->setPaletteForegroundColor(config->readColorEntry( "TextColor" ));
	properties_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor" ));
	properties_box->setFont(config->readFontEntry( "Font" ) );
	properties_box->setEnabled(config->readBoolEntry( "Visibility" ) );
	properties_box->setAlignment(config->readNumEntry( "Alignment" ));
}

void SetupDisplay::createWidgets( const Recipe &sample )
{
//=========================TITLE=======================//
	QString title;
	if ( sample.title.isNull() )
		title = i18n("Recipe Title");
	else
		title = sample.title;

	title_box = new QLabel(title,this,"TitleSetup");
	title_box->setFrameShape( QFrame::Box );
	title_box->setMinimumSize(5,5);
	title_box->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	QToolTip::add(title_box,i18n("Title"));

	box_properties->insert( title_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//======================INSTRUCTIONS===================//
	QString instr;
	if ( sample.instructions.isNull() )
		instr = i18n("Instructions");
	else
		instr = sample.instructions;

	instr.replace("\n","<BR>");
	instr_box = new QLabel(instr,this,"InstructionsSetup");
	instr_box->setMinimumSize(5,5);
	instr_box->setFrameShape( QFrame::Box );
	instr_box->setAlignment( Qt::AlignTop );
	instr_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(instr_box,i18n("Instructions"));

	box_properties->insert( instr_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================SERVINGS======================//
	servings_box = new QLabel(QString(i18n("Servings: %1")).arg(sample.persons),this,"ServingsSetup");
	servings_box->setFrameShape( QFrame::Box );
	QToolTip::add(servings_box,i18n("Servings"));

	box_properties->insert( servings_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================PHOTO========================//
	photo_box = new QLabel(this,"PhotoSetup");
	photo_box->setFrameShape( QFrame::Box );
	photo_box->setMinimumSize(5,5);
	photo_box->setScaledContents( true );
	if ( !sample.photo.isNull() )
		photo_box->setPixmap(sample.photo);
	else
		photo_box->setPixmap(defaultPhoto);
	QToolTip::add(photo_box,i18n("Photo"));

	box_properties->insert( photo_box, Visibility | Geometry | StaticHeight );

	//========================AUTHORS========================//
	QString authors;
	QPtrListIterator<Element> author_it( sample.authorList );
	Element *author_el;
	while ( (author_el = author_it.current()) != 0 )
	{
        	++author_it;

		if (authors != QString::null) authors += ",";
		authors += author_el->name;
	}

	if ( authors.isNull() )
		authors = i18n("Author 1, Author 2, ...");

	authors_box = new QLabel(authors,this,"AuthorsSetup");
	authors_box->setFrameShape( QFrame::Box );
	QToolTip::add(authors_box,i18n("Authors"));

	box_properties->insert( authors_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================CATEGORIES========================//
	QString categories;
	QPtrListIterator<Element> cat_it( sample.categoryList );
	Element *cat_el;
	while ( (cat_el = cat_it.current()) != 0 )
	{
        	++cat_it;

		if (categories != QString::null) categories += ",";
		categories += cat_el->name;
	}

	if ( categories.isNull() )
		categories = i18n("Category 1, Category 2, ...");

	categories_box = new QLabel(categories,this,"CategoriesSetup");
	categories_box->setFrameShape( QFrame::Box );
	QToolTip::add(categories_box,i18n("Categories"));

	box_properties->insert( categories_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================ID======================//
	id_box = new QLabel(QString(i18n("Recipe: #%1")).arg(sample.recipeID),this,"HeaderSetup");
	id_box->setFrameShape( QFrame::Box );
	QToolTip::add(id_box,i18n("Recipe ID"));

	box_properties->insert( id_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================INGREDIENTS========================//
	QString ingredients;

	KConfig *config=kapp->config();
	config->setGroup("Numbers");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;

	QPtrListIterator<Ingredient> ing_it( sample.ingList );
	Ingredient *ing;
	while ( (ing = ing_it.current()) != 0 )
	{
        	++ing_it;
		if (ingredients.isNull()) ingredients += "<ul>";

		QString amount_str = MixedNumber(ing->amount).toString( number_format );
		if (amount_str == "0")
			amount_str = "";

		ingredients += QString("<li>%1: %2 %3</li>")
			    .arg(ing->name)
			    .arg(amount_str)
			    .arg(ing->units);
	}

	if ( !ingredients.isNull() )
		ingredients += "</ul>";
	else
		ingredients = i18n("<ul><li>Ingredient 1</li><li>Ingredient 2</li><li>...</li></ul>");

	ingredients_box = new QLabel(ingredients,this,"IngredientsSetup");
	ingredients_box->setFrameShape( QFrame::Box );
	QToolTip::add(ingredients_box,i18n("Ingredients"));

	box_properties->insert( ingredients_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================PROPERTIES========================//
	properties_box = new QLabel(i18n("<ul><li>Property 1</li><li>Property 2</li><li>...</li></ul>"),this,"PropertiesSetup");
	properties_box->setFrameShape( QFrame::Box );
	QToolTip::add(properties_box,i18n("Properties"));

	box_properties->insert( properties_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );
}

void SetupDisplay::save()
{

	QRect r;
	KConfig *config=kapp->config();

	const QObjectList *list = this->children();
	QObjectListIt it( *list );
	QObject * obj;
	while ( (obj=it.current()) != 0 )
	{
		++it;
		if ( obj->isWidgetType() )
		{
			QWidget *w = static_cast<QWidget*>(obj);
			config->setGroup( w->name() );

			unsigned int properties = (*box_properties->find(w));

			if ( properties & Geometry )
				{
				r=w->geometry(); toPercentage(&r);
				config->writeEntry( "Geometry",r);
				}

			if ( properties & Font )
				config->writeEntry( "Font", static_cast<QLabel*>(w)->font() );

			if ( properties & BackgroundColor )
				config->writeEntry( "BackgroundColor", w->backgroundColor() );

			if ( properties & TextColor )
				config->writeEntry( "TextColor", w->foregroundColor() );

			if ( properties & Visibility )
				config->writeEntry( "Visibility", w->isEnabled() );

			if ( properties & Alignment )
				config->writeEntry( "Alignment", static_cast<QLabel*>(w)->alignment() );
		}
	}

	config->setGroup( "BackgroundSetup" );
	config->writeEntry( "BackgroundColor", backgroundColor() );
	config->writeEntry( "Geometry", geometry() );
}

void SetupDisplay::widgetClicked( QMouseEvent *e, QWidget *w )
{
	if ( e->button() == QMouseEvent::RightButton )
	{
		popup = new QPopupMenu( w ); //parent _must_ be widget acting on

		unsigned int properties = *box_properties->find(w);

		if ( properties & BackgroundColor )
			popup->insertItem( i18n("Background color..."), this, SLOT(setBackgroundColor()) );

		if ( properties & TextColor )
			popup->insertItem( i18n("Text color..."), this, SLOT(setTextColor()) );

		if ( properties & Font )
			popup->insertItem( i18n("Font..."), this, SLOT(setFont()) );

		if ( properties & Visibility )
		{
			int id = popup->insertItem( i18n("Show"), this, SLOT(setShown(int)) );
			popup->setItemChecked( id, w->isEnabled() );
		}

		if ( properties & Alignment )
		{
			QPopupMenu *sub_popup = new QPopupMenu( popup );

			QActionGroup *alignment_actions = new QActionGroup( this );
			alignment_actions->setExclusive( true );

			new QAction( i18n("Center"), i18n("Center"), 0, alignment_actions, 0, true );
			new QAction( i18n("Left"), i18n("Left"), 0, alignment_actions, 0, true );
			new QAction( i18n("Right"), i18n("Right"), 0, alignment_actions, 0, true );

			connect( alignment_actions, SIGNAL(selected(QAction*)), SLOT(setAlignment(QAction*)) );

			popup->insertItem( i18n("Alignment"), sub_popup );

			alignment_actions->addTo( sub_popup );
		}

		popup->popup( mapToGlobal(e->pos()) );

		e->accept();
	}
}

void SetupDisplay::setBackgroundColor()
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QWidget") )
	{
		QWidget *box = static_cast<QWidget*>(obj);

		QColor new_color = box->backgroundColor();
		if ( KColorDialog::getColor( new_color, this ) == QDialog::Accepted )
			box->setPaletteBackgroundColor( new_color );
	}
}

void SetupDisplay::setTextColor()
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QWidget") )
	{
		QWidget *box = static_cast<QWidget*>(obj);

		QColor new_color = box->foregroundColor();
		if ( KColorDialog::getColor( new_color, this ) == QDialog::Accepted )
			box->setPaletteForegroundColor( new_color );
	}
}

void SetupDisplay::setFont()
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QLabel") )
	{
		QLabel *box = static_cast<QLabel*>(obj);

		QFont new_font = box->font();
		if ( KFontDialog::getFont( new_font, false, this ) == QDialog::Accepted )
			box->setFont( new_font );
	}
}

void SetupDisplay::setShown( int id )
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QWidget") )
	{
		QWidget *box = static_cast<QWidget*>(obj);
		box->setEnabled( !popup->isItemChecked(id) );

		if ( popup->isItemChecked(id) )
			this->setWidget( 0 ); //clears the selection
	}
}QRect scaled;

void SetupDisplay::setAlignment( QAction *action )
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QLabel") )
	{
		QLabel *box = static_cast<QLabel*>(obj);

		int align = box->alignment();

		//TODO: isn't there a simpler way to do this...
		//preserve non-horizontal alignment flags
		if ( align & Qt::AlignRight )
			align ^= Qt::AlignRight;
		if ( align & Qt::AlignHCenter )
			align ^= Qt::AlignHCenter;
		if ( align & Qt::AlignLeft )
			align ^= Qt::AlignLeft;

		if ( action->text() == i18n("Center") )
			align |= Qt::AlignHCenter;
		else if ( action->text() == i18n("Left") )
			align |= Qt::AlignLeft;
		else if ( action->text() == i18n("Right") )
			align |= Qt::AlignRight;

		box->setAlignment( align );
	}
}

/*
**  Rescales the dimensions according to the page size
*/

void SetupDisplay::toAbsolute(QRect *r)
{
QSize scaledSize;
QPoint scaledPoint;

scaledSize.setWidth(r->width()/100.0*width());
scaledSize.setHeight(r->height()/100.0*height());
scaledPoint.setX(r->left()/100.0*width());
scaledPoint.setY(r->top()/100.0*height());
r->setTopLeft(scaledPoint);
r->setSize(scaledSize);

}

void SetupDisplay::toPercentage(QRect *r)
{
QSize scaledSize;
QPoint scaledPoint;

scaledSize.setWidth(r->width()*100.0/width());
scaledSize.setHeight(r->height()*100.0/height());
scaledPoint.setX(r->left()*100.0/width());
scaledPoint.setY(r->top()*100.0/height());
r->setTopLeft(scaledPoint);
r->setSize(scaledSize);

}

QSize SetupDisplay::minimumSize() const
{
return(QSize(300,400));
}

QSize SetupDisplay::sizeHint(void) const
{
return (minimumSize());
}