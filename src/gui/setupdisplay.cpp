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
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

	box_properties->insert( this, BackgroundColor );

	connect( this, SIGNAL(widgetClicked(QMouseEvent*,QWidget*)), SLOT(widgetClicked(QMouseEvent*,QWidget*)) );

	createWidgets( sample );
	loadSetup();

	adjustSize(); //this seems to need to be called to set a fixed size...
	setFixedSize(500,600);
}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
}

void SetupDisplay::setReadOnly( bool read_only )
{
	DragArea::setReadOnly( read_only );
}

//do not call until createWidgets() has been called!
void SetupDisplay::loadSetup()
{
	QColor color;
	QFont default_font = title_box->font();

	//TODO: we have just one setup available for now. Later setup may be a separate file
	KConfig *config=kapp->config();

	//=========================this=======================//
	config->setGroup("BackgroundSetup");

	color.setRgb(255,255,255);
 	setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	//=========================TITLE=======================//
	config->setGroup("TitleSetup");
	QRect default_title_geom( 185, 35, 306, title_box->sizeHint().height() );
	title_box->setGeometry( config->readRectEntry( "Geometry", &default_title_geom ));

	color.setRgb(0,0,0);
	title_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	title_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));
	title_box->setFont(config->readFontEntry( "Font", &default_font ) );
	title_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	title_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignHCenter | Qt::WordBreak ));

	//======================INSTRUCTIONS===================//
	config->setGroup("InstructionsSetup");
	QRect default_instr_geom( 179, 134, 318, instr_box->sizeHint().height() );
	instr_box->setGeometry( config->readRectEntry( "Geometry", &default_instr_geom ));

	color.setRgb(0,0,0);
	instr_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	instr_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	instr_box->setFont(config->readFontEntry( "Font", &default_font ) );
	instr_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	instr_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================SERVINGS======================//
	config->setGroup("ServingsSetup");
	QRect default_servings_geom( 393, 124, 85, servings_box->sizeHint().height() );
	servings_box->setGeometry( config->readRectEntry( "Geometry", &default_servings_geom ));

	color.setRgb(0,0,0);
	servings_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	servings_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	servings_box->setFont(config->readFontEntry( "Font", &default_font ) );
	servings_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	servings_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::AlignVCenter | Qt::WordBreak ));

	//========================PHOTO========================//
	config->setGroup("PhotoSetup");
	QRect default_photo_geom( 4, 38, 220, 165 );
	photo_box->setGeometry( config->readRectEntry( "Geometry", &default_photo_geom ));

	photo_box->setEnabled(config->readBoolEntry( "Visibility", true ) );

	//=======================AUTHORS======================//
	config->setGroup("AuthorsSetup");
	QRect default_authors_geom( 186, 68, 123, authors_box->sizeHint().height() );
	authors_box->setGeometry( config->readRectEntry( "Geometry", &default_authors_geom ));

	color.setRgb(0,0,0);
	authors_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	authors_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	authors_box->setFont(config->readFontEntry( "Font", &default_font ) );
	authors_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	authors_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================CATEGORIES======================//
	config->setGroup("CategoriesSetup");
	QRect default_categories_geom( 312, 68, 184, categories_box->sizeHint().height() );
	categories_box->setGeometry( config->readRectEntry( "Geometry", &default_categories_geom ));

	color.setRgb(0,0,0);
	categories_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	categories_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	categories_box->setFont(config->readFontEntry( "Font", &default_font ) );
	categories_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	categories_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================ID======================//
	config->setGroup("RecipeIDSetup");
	QRect default_header_geom( 6, 3, 486, id_box->sizeHint().height() );
	id_box->setGeometry( config->readRectEntry( "Geometry", &default_header_geom ));

	color.setRgb(0,0,0);
	id_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	id_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	id_box->setFont(config->readFontEntry( "Font", &default_font ) );
	id_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	id_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================INGREDIENTS======================//
	config->setGroup("IngredientsSetup");
	QRect default_ings_geom( 3, 177, 205, ingredients_box->sizeHint().height() );
	ingredients_box->setGeometry( config->readRectEntry( "Geometry", &default_ings_geom ));

	color.setRgb(0,0,0);
	ingredients_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	ingredients_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	ingredients_box->setFont(config->readFontEntry( "Font", &default_font ) );
	ingredients_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	ingredients_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));
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
	//title_box->setTextFormat( Qt::RichText ); //allows for wrapping
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
	id_box = new QLabel(QString(i18n("Recipe: #%1")).arg(sample.recipeID),this,"RecipeIDSetup");
	id_box->setFrameShape( QFrame::Box );
	QToolTip::add(id_box,i18n("Recipe ID"));

	box_properties->insert( id_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================INGREDIENTS========================//
	QString ingredients;

	KConfig *config=kapp->config();
	config->setGroup("Units");

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
}

void SetupDisplay::save()
{
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
				config->writeEntry( "Geometry", w->geometry() );

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
}

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

