/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pagesetupdialog.h"

#include "../mixednumber.h"

#include "../widgets/dragarea.h"
#include "../widgets/sizehandle.h"

#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfontdialog.h>
#include <kcolordialog.h>

#include <qaction.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qobjectlist.h>
#include <qpopupmenu.h>

#include <cmath>

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : QDialog(parent,0,true),
  box_properties(new QMap< QWidget*, unsigned int >)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	dragArea = new DragArea(this);
	dragArea->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

	box_properties->insert( dragArea, BackgroundColor );

	layout->addWidget(dragArea);

	QHBox *buttonsBox = new QHBox(this);
	KIconLoader *il = new KIconLoader;
	QPushButton *okButton = new QPushButton(il->loadIconSet("ok",KIcon::Small),i18n("&OK"),buttonsBox);
	QPushButton *cancelButton = new QPushButton(il->loadIconSet("cancel",KIcon::Small),i18n("&Cancel"),buttonsBox);
	layout->addWidget(buttonsBox);

	connect( okButton, SIGNAL(clicked()), SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
	connect( dragArea, SIGNAL(widgetClicked(QMouseEvent*,QWidget*)), SLOT(widgetClicked(QMouseEvent*,QWidget*)) );

	createWidgets( sample );

	adjustSize(); //this seems to need to be called to set a fixed size...
	setFixedSize(500,600);

	loadSetup();
}

PageSetupDialog::~PageSetupDialog()
{
	delete box_properties;
}

//do not call until createWidgets() has been called!
void PageSetupDialog::loadSetup()
{
	QColor color;
	QFont default_font = title_box->font();

	//TODO: we have just one setup available for now. Later setup may be a separate file
	KConfig *config=kapp->config();

	//=========================DRAGAREA=======================//
	config->setGroup("BackgroundSetup");

	color.setRgb(255,255,255);
 	dragArea->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	//=========================TITLE=======================//
	config->setGroup("TitleSetup");
	QValueList<int> title_dim_info = config->readIntListEntry( "Geometry" );
	if ( title_dim_info.count() < 3 )
	{
		title_dim_info.append( 185 );   //left
		title_dim_info.append( 35 );   //top
		title_dim_info.append( 306 ); //width
	}
	title_box->setGeometry( title_dim_info[0], title_dim_info[1], title_dim_info[2], title_box->height() );

	color.setRgb(0,0,0);
	title_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	title_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));
	title_box->setFont(config->readFontEntry( "Font", &default_font ) );
	title_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	title_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignHCenter | Qt::WordBreak ));

	//======================INSTRUCTIONS===================//
	config->setGroup("InstructionsSetup");
	QValueList<int> instr_dim_info = config->readIntListEntry( "Geometry" );
	if ( instr_dim_info.count() < 3 )
	{
		instr_dim_info.append( 179 );   //left
		instr_dim_info.append( 134 ); //top
		instr_dim_info.append( 318 ); //width
	}
	instr_box->setGeometry( instr_dim_info[0], instr_dim_info[1], instr_dim_info[2], 100 );

	color.setRgb(0,0,0);
	instr_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	instr_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	instr_box->setFont(config->readFontEntry( "Font", &default_font ) );
	instr_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	instr_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================SERVINGS======================//
	config->setGroup("ServingsSetup");
	QValueList<int> servings_dim_info = config->readIntListEntry( "Geometry" );
	if ( servings_dim_info.count() < 3 )
	{
		servings_dim_info.append( 393 );   //left
		servings_dim_info.append( 124 ); //top
		servings_dim_info.append( 85 ); //width
	}
	servings_box->setGeometry( servings_dim_info[0], servings_dim_info[1], servings_dim_info[2], servings_box->height() );

	color.setRgb(0,0,0);
	servings_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	servings_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	servings_box->setFont(config->readFontEntry( "Font", &default_font ) );
	servings_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	servings_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::AlignVCenter | Qt::WordBreak ));

	//========================PHOTO========================//
	config->setGroup("PhotoSetup");
	QValueList<int> photo_dim_info = config->readIntListEntry( "Geometry" );
	if ( photo_dim_info.count() < 4 )
	{
		photo_dim_info.append( 4 );  //left
		photo_dim_info.append( 38 );  //top
		photo_dim_info.append( 172 );  //width
		photo_dim_info.append( 131 );  //height
	}
	photo_box->setGeometry( photo_dim_info[0], photo_dim_info[1], photo_dim_info[2], photo_dim_info[3] );
	photo_box->setEnabled(config->readBoolEntry( "Visibility", true ) );

	//=======================AUTHORS======================//
	config->setGroup("AuthorsSetup");
	QValueList<int> authors_dim_info = config->readIntListEntry( "Geometry" );
	if ( authors_dim_info.count() < 3 )
	{
		authors_dim_info.append( 186 );   //left
		authors_dim_info.append( 68 ); //top
		authors_dim_info.append( 123 ); //width
	}
	authors_box->setGeometry( authors_dim_info[0], authors_dim_info[1], authors_dim_info[2], authors_box->height() );

	color.setRgb(0,0,0);
	authors_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	authors_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	authors_box->setFont(config->readFontEntry( "Font", &default_font ) );
	authors_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	authors_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================CATEGORIES======================//
	config->setGroup("CategoriesSetup");
	QValueList<int> categories_dim_info = config->readIntListEntry( "Geometry" );
	if ( categories_dim_info.count() < 3 )
	{
		categories_dim_info.append( 312 );   //left
		categories_dim_info.append( 68 ); //top
		categories_dim_info.append( 184 ); //width
	}
	categories_box->setGeometry( categories_dim_info[0], categories_dim_info[1], categories_dim_info[2], categories_box->height() );

	color.setRgb(0,0,0);
	categories_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	categories_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	categories_box->setFont(config->readFontEntry( "Font", &default_font ) );
	categories_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	categories_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================ID======================//
	config->setGroup("RecipeIDSetup");
	QValueList<int> recipe_id_dim_info = config->readIntListEntry( "Geometry" );
	if ( recipe_id_dim_info.count() < 3 )
	{
		recipe_id_dim_info.append( 6 );   //left
		recipe_id_dim_info.append( 3 ); //top
		recipe_id_dim_info.append( 486 ); //width
	}
	id_box->setGeometry( recipe_id_dim_info[0], recipe_id_dim_info[1], recipe_id_dim_info[2], id_box->height() );

	color.setRgb(0,0,0);
	id_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	id_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	id_box->setFont(config->readFontEntry( "Font", &default_font ) );
	id_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	id_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));

	//=======================INGREDIENTS======================//
	config->setGroup("IngredientsSetup");
	QValueList<int> ing_dim_info = config->readIntListEntry( "Geometry" );
	if ( ing_dim_info.count() < 3 )
	{
		ing_dim_info.append( -27 );   //left
		ing_dim_info.append( 177 ); //top
		ing_dim_info.append( 205 ); //width
	}
	ingredients_box->setGeometry( ing_dim_info[0], ing_dim_info[1], ing_dim_info[2], ingredients_box->height() );

	color.setRgb(0,0,0);
	ingredients_box->setPaletteForegroundColor(config->readColorEntry( "TextColor", &color ));

	color.setRgb(255,255,255);
	ingredients_box->setPaletteBackgroundColor(config->readColorEntry( "BackgroundColor", &color ));

	ingredients_box->setFont(config->readFontEntry( "Font", &default_font ) );
	ingredients_box->setEnabled(config->readBoolEntry( "Visibility", true ) );
	ingredients_box->setAlignment(config->readNumEntry( "Alignment", Qt::AlignLeft | Qt::WordBreak ));
}

void PageSetupDialog::createWidgets( const Recipe &sample )
{
//=========================TITLE=======================//
	QString title;
	if ( sample.title.isNull() )
		title = i18n("Recipe Title");
	else
		title = sample.title;

	title_box = new QLabel(title,dragArea,"TitleSetup");
	title_box->setFrameShape( QFrame::Box );
	title_box->setMinimumSize(5,5);
	title_box->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	title_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(title_box,i18n("Title"));

	box_properties->insert( title_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//======================INSTRUCTIONS===================//
	QString instr;
	if ( sample.instructions.isNull() )
		instr = i18n("Instructions");
	else
		instr = sample.instructions;

	instr_box = new QLabel(instr,dragArea,"InstructionsSetup");
	instr_box->setMinimumSize(5,5);
	instr_box->setFrameShape( QFrame::Box );
	instr_box->setAlignment( Qt::AlignTop );
	instr_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(instr_box,i18n("instructions"));

	box_properties->insert( instr_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================SERVINGS======================//
	servings_box = new QLabel(QString(i18n("Servings: %1")).arg(sample.persons),dragArea,"ServingsSetup");
	servings_box->setFrameShape( QFrame::Box );
	QToolTip::add(servings_box,i18n("Servings"));

	box_properties->insert( servings_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================PHOTO========================//
	photo_box = new QLabel(dragArea,"PhotoSetup");
	photo_box->setFrameShape( QFrame::Box );
	photo_box->setMinimumSize(5,5);
	photo_box->setScaledContents( true );
	photo_box->setPixmap(sample.photo);
	QToolTip::add(photo_box,i18n("Photo"));

	box_properties->insert( photo_box, Visibility | Geometry );

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

	authors_box = new QLabel(authors,dragArea,"AuthorsSetup");
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

	categories_box = new QLabel(categories,dragArea,"CategoriesSetup");
	categories_box->setFrameShape( QFrame::Box );
	QToolTip::add(categories_box,i18n("Categories"));

	box_properties->insert( categories_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================ID======================//
	id_box = new QLabel(QString(i18n("Recipe: #%1")).arg(sample.recipeID),dragArea,"RecipeIDSetup");
	id_box->setFrameShape( QFrame::Box );
	QToolTip::add(id_box,i18n("Recipe ID"));

	box_properties->insert( id_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================INGREDIENTS========================//
	QString ingredients;

	KConfig *config=kapp->config();
	config->setGroup("Units");
	bool fraction = config->readBoolEntry("Fraction", false);

	QPtrListIterator<Ingredient> ing_it( sample.ingList );
	Ingredient *ing;
	while ( (ing = ing_it.current()) != 0 )
	{
        	++ing_it;
		if (ingredients.isNull()) ingredients += "<ul>";

		QString amount_str = MixedNumber(ing->amount).toString( (fraction) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat);
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

	ingredients_box = new QLabel(ingredients,dragArea,"IngredientsSetup");
	ingredients_box->setFrameShape( QFrame::Box );
	QToolTip::add(ingredients_box,i18n("Ingredients"));

	box_properties->insert( ingredients_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );
}

void PageSetupDialog::accept()
{
	save();

	QDialog::accept();
}

void PageSetupDialog::save()
{
	KConfig *config=kapp->config();

	QValueList<int> dimensions;

	const QObjectList *list = dragArea->children();
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
				dimensions.append( w->geometry().left() );
				dimensions.append( w->geometry().top() );
				dimensions.append( w->geometry().width() );
				dimensions.append( w->geometry().height() );
				config->writeEntry( "Geometry", dimensions );
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

			dimensions.clear();
		}
	}
}

void PageSetupDialog::widgetClicked( QMouseEvent *e, QWidget *w )
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

void PageSetupDialog::setBackgroundColor()
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

void PageSetupDialog::setTextColor()
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

void PageSetupDialog::setFont()
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

void PageSetupDialog::setShown( int id )
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QWidget") )
	{
		QWidget *box = static_cast<QWidget*>(obj);
		box->setEnabled( !popup->isItemChecked(id) );

		if ( popup->isItemChecked(id) )
			dragArea->setWidget( 0 ); //clears the selection
	}
}

void PageSetupDialog::setAlignment( QAction *action )
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QLabel") )
	{
		QLabel *box = static_cast<QLabel*>(obj);

		int align = box->alignment() ^ Qt::AlignRight ^ Qt::AlignHCenter ^ Qt::AlignLeft; //preserve non-horizontal alignment flags

		if ( action->text() == i18n("Center") )
			align |= Qt::AlignHCenter;
		else if ( action->text() == i18n("Left") )
			align |= Qt::AlignLeft;
		else if ( action->text() == i18n("Right") )
			align |= Qt::AlignRight;

		box->setAlignment( align );
		box->setTextFormat( Qt::RichText );
	}
}
