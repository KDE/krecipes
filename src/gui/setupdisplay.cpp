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
#include <kdebug.h>
#include <kfontdialog.h>
#include <kcolordialog.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <qaction.h>
#include <qlabel.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qobjectlist.h>
#include <qvaluelist.h>

#include "../image.h"
#include "../mixednumber.h"

#include <cmath>

SetupDisplay::SetupDisplay( const Recipe &sample, QWidget *parent ) : DragArea( parent, "background" ),
  box_properties(new PropertiesMap),
  has_changes(false)
{
	setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );

	box_properties->insert( this, BackgroundColor );

	connect( this, SIGNAL(widgetClicked(QMouseEvent*,QWidget*)), SLOT(widgetClicked(QMouseEvent*,QWidget*)) );
	connect( this, SIGNAL(widgetGeometryChanged()), SLOT(changeMade()) );

	createWidgets( sample );

	KConfig *config=kapp->config();
	config->setGroup("Page Setup");
	QSize default_size(300,400);
	m_size = config->readSizeEntry( "WindowSize", &default_size );
	resize(m_size);
	parentWidget()->resize(m_size);
}

SetupDisplay::~SetupDisplay()
{
	delete box_properties;
}

void SetupDisplay::loadLayout( const QString &filename )
{
	QFile input( filename );
	if ( input.open( IO_ReadOnly ) )
	{
		has_changes = false;
	
		QDomDocument doc;
		QString error; int line; int column;
		if (!doc.setContent(&input,&error,&line,&column))
		{
			kdDebug()<<QString( i18n("\"%1\" at line %2, column %3.  This may not be a Krecipes layout file.") ).arg(error).arg(line).arg(column)<<endl;
			return;
		}

		QDomElement layout = doc.documentElement();

		if ( layout.tagName() != "krecipes-layout" )
		{
			kdDebug()<<"This file does not appear to be a valid Krecipes layout file."<<endl;
			return;
		}

		QMap<QString,QWidget*> widget_map;
		QValueList<QWidget*> widgets = box_properties->keys();
		for ( QValueList<QWidget*>::const_iterator it = widgets.begin(); it != widgets.end(); ++it )
			widget_map.insert((*it)->name(),*it);

		QDomNodeList l = layout.childNodes();
		for (unsigned i = 0 ; i < l.count(); i++)
		{
			QDomElement el = l.item(i).toElement();
			QString tagName = el.tagName();

			QMap<QString,QWidget*>::const_iterator map_it = widget_map.find(tagName);
			if ( map_it != widget_map.end() )
			{
				QDomNodeList l = el.childNodes();
				for (unsigned i = 0 ; i < l.count(); i++)
				{
					QDomElement el = l.item(i).toElement();
					QString subTagName = el.tagName();

					if (subTagName == "background-color")
						loadBackgroundColor( *map_it, el );
					else if (subTagName == "geometry")
						loadGeometry( *map_it, el );
					else if (subTagName == "font")
						loadFont( *map_it, el );
					else if (subTagName == "text-color")
						loadTextColor( *map_it, el );
					else if (subTagName == "visible")
						loadVisibility( *map_it, el );
					else if (subTagName == "alignment")
						loadAlignment( *map_it, el );
					else
						kdDebug()<<"Warning: Unknown tag within <"<<tagName<<">: "<<subTagName<<endl;
				}
			}
			else
				kdDebug()<<"Warning: Unknown tag within <krecipes-layout>: "<<tagName<<endl;
		}
		
		setWidget( 0 );
	}
	else
		kdDebug()<<"Unable to open file: "<<filename<<endl;
}

void SetupDisplay::loadBackgroundColor( QWidget *widget, const QDomElement &tag )
{
	QColor c; c.setNamedColor(tag.text());
	widget->setPaletteBackgroundColor(c);
}

void SetupDisplay::loadFont( QWidget *widget, const QDomElement &tag )
{
	QFont f;
	if ( f.fromString( tag.text() ) )
		widget->setFont( f );
}

void SetupDisplay::loadGeometry( QWidget *widget, const QDomElement &tag )
{
	PreciseRect r( tag.attribute("left").toDouble(), tag.attribute("top").toDouble(), tag.attribute("width").toDouble(), tag.attribute("height").toDouble() );
	toAbsolute( &r );
	widget->setGeometry( r.toQRect() );
}

void SetupDisplay::loadTextColor( QWidget *widget, const QDomElement &tag )
{
	QColor c; c.setNamedColor(tag.text());
	widget->setPaletteForegroundColor(c);
}

void SetupDisplay::loadVisibility( QWidget *widget, const QDomElement &tag )
{
	bool visible = (tag.text()=="false") ? false : true;
	widget->setShown( visible );
	emit itemVisibilityChanged( widget, visible );
}

void SetupDisplay::loadAlignment( QWidget *widget, const QDomElement &tag )
{
	if ( widget->inherits("QLabel") )
		static_cast<QLabel*>(widget)->setAlignment( tag.text().toInt() );
}

void SetupDisplay::saveLayout( const QString &filename )
{
	QDomImplementation dom_imp;
	QDomDocument doc = dom_imp.createDocument( QString::null, "krecipes-layout", dom_imp.createDocumentType( "krecipes-layout", QString::null, QString::null ) );

	QDomElement layout_tag = doc.documentElement();
	layout_tag.setAttribute( "version", 0.2 );
	//layout_tag.setAttribute( "generator", QString("Krecipes v%1").arg(krecipes_version()) );
	doc.appendChild( layout_tag );

	for ( PropertiesMap::const_iterator it = box_properties->begin(); it != box_properties->end(); ++it )
	{
		QDomElement base_tag = doc.createElement(it.key()->name());
		layout_tag.appendChild(base_tag);

		if ( it.data() & BackgroundColor )
		{
			QDomElement backgroundcolor_tag = doc.createElement("background-color");
			backgroundcolor_tag.appendChild( doc.createTextNode(it.key()->backgroundColor().name()) );
			base_tag.appendChild(backgroundcolor_tag);
		}

		if ( it.data() & TextColor )
		{
			QDomElement textcolor_tag = doc.createElement("text-color");
			textcolor_tag.appendChild( doc.createTextNode(it.key()->foregroundColor().name()) );
			base_tag.appendChild(textcolor_tag);
		}

		if ( it.data() & Font )
		{
			QDomElement font_tag = doc.createElement("font");
			font_tag.appendChild( doc.createTextNode(static_cast<QLabel*>(it.key())->font().toString()) );
			base_tag.appendChild(font_tag);
		}

		if ( it.data() & Visibility )
		{
			QDomElement visibility_tag = doc.createElement("visible");
			visibility_tag.appendChild( doc.createTextNode( (it.key()->isShown()) ? "true" : "false" ) );
			base_tag.appendChild(visibility_tag);
		}

		if ( it.data() & Geometry )
		{
			PreciseRect r( it.key()->geometry() );
			toPercentage( &r );
		
			QDomElement geometry_tag = doc.createElement("geometry");
			geometry_tag.setAttribute("top",r.top());
			geometry_tag.setAttribute("left",r.left());
			geometry_tag.setAttribute("width",r.width());
			geometry_tag.setAttribute("height",r.height());
			base_tag.appendChild(geometry_tag);
		}

		if ( it.data() & Alignment )
		{
			QDomElement alignment_tag = doc.createElement("alignment");
			alignment_tag.appendChild( doc.createTextNode(QString::number(static_cast<QLabel*>(it.key())->alignment())) );
			base_tag.appendChild(alignment_tag);
		}
	}
	
	KConfig *config = kapp->config();
	config->setGroup( "Page Setup" );
	config->writeEntry( "Aspect", static_cast<double>(width())/static_cast<double>(height()));
	config->writeEntry( "WindowSize", parentWidget()->size() );

	QFile out_file( filename );
	if ( out_file.open(IO_WriteOnly) )
	{
		has_changes = false;
	
		QTextStream stream( &out_file );
		stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" << doc.toString();
	}
	else
		kdDebug()<<"Error: Unable to write to file "<<filename<<endl;
}

void SetupDisplay::createWidgets( const Recipe &sample )
{
	//=========================TITLE=======================//
	QString title;
	if ( sample.title.isNull() )
		title = i18n("Recipe Title");
	else
		title = sample.title;

	title_box = new QLabel(title,this,"title");
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
	instr_box = new QLabel(instr,this,"instructions");
	instr_box->setMinimumSize(5,5);
	instr_box->setFrameShape( QFrame::Box );
	instr_box->setAlignment( Qt::AlignTop );
	instr_box->setTextFormat( Qt::RichText ); //allows for wrapping
	QToolTip::add(instr_box,i18n("Instructions"));

	box_properties->insert( instr_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================SERVINGS======================//
	servings_box = new QLabel(QString("<b>%1:</b> %2").arg(i18n("Servings")).arg(sample.persons),this,"servings");
	servings_box->setFrameShape( QFrame::Box );
	QToolTip::add(servings_box,i18n("Servings"));

	box_properties->insert( servings_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================PREP TIME======================//
	preptime_box = new QLabel(QString("<b>%1:</b> %2").arg(i18n("Preparation Time")).arg(sample.prepTime.toString("h:mm")),this,"prep_time");
	preptime_box->setFrameShape( QFrame::Box );
	QToolTip::add(preptime_box,i18n("Preparation Time"));

	box_properties->insert( preptime_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================PHOTO========================//
	photo_box = new QLabel(this,"photo");
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
	for ( ElementList::const_iterator author_it = sample.authorList.begin(); author_it != sample.authorList.end(); ++author_it )
	{
		if (!authors.isEmpty()) authors += ",";
		authors += (*author_it).name;
	}

	if ( authors.isNull() )
		authors = i18n("Author 1, Author 2, ...");
	authors.prepend( QString("<b>%1: </b>").arg(i18n("Authors")) );

	authors_box = new QLabel(authors,this,"authors");
	authors_box->setFrameShape( QFrame::Box );
	QToolTip::add(authors_box,i18n("Authors"));

	box_properties->insert( authors_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================CATEGORIES========================//
	QString categories;
	for ( ElementList::const_iterator cat_it = sample.categoryList.begin(); cat_it != sample.categoryList.end(); ++cat_it )
	{
		if (!categories.isEmpty()) categories += ",";
		categories += (*cat_it).name;
	}

	if ( categories.isNull() )
		categories = i18n("Category 1, Category 2, ...");
	categories.prepend( QString("<b>%1: </b>").arg(i18n("Categories")) );

	categories_box = new QLabel(categories,this,"categories");
	categories_box->setFrameShape( QFrame::Box );
	QToolTip::add(categories_box,i18n("Categories"));

	box_properties->insert( categories_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//=======================ID======================//
	id_box = new QLabel(QString(i18n("Recipe: #%1")).arg(sample.recipeID),this,"header");
	id_box->setFrameShape( QFrame::Box );
	QToolTip::add(id_box,i18n("Header"));

	box_properties->insert( id_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================INGREDIENTS========================//
	QString ingredients;

	KConfig *config=kapp->config();
	config->setGroup("Formatting");

	MixedNumber::Format number_format = (config->readBoolEntry("Fraction")) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;
	QString ingredient_format = config->readEntry("Ingredient","%n%p: %a %u");

	for ( IngredientList::const_iterator ing_it = sample.ingList.begin(); ing_it != sample.ingList.end(); ++ing_it )
	{
		if (ingredients.isNull()) ingredients += "<ul>";

		QString amount_str = MixedNumber((*ing_it).amount).toString( number_format );
		if ((*ing_it).amount <= 1e-10)
			amount_str = "";

		QString tmp_format(ingredient_format);
		tmp_format.replace(QRegExp(QString::fromLatin1("%n")),(*ing_it).name);
		tmp_format.replace(QRegExp(QString::fromLatin1("%a")),amount_str);
		tmp_format.replace(QRegExp(QString::fromLatin1("%u")),(*ing_it).units);
		tmp_format.replace(QRegExp(QString::fromLatin1("%p")),((*ing_it).prepMethod.isEmpty()) ?
				   QString::fromLatin1("") : QString::fromLatin1("; ")+(*ing_it).prepMethod);

		ingredients += QString("<li>%1</li>").arg(tmp_format);
	}

	if ( !ingredients.isNull() )
		ingredients += "</ul>";
	else
		ingredients = i18n("<ul><li>Ingredient 1</li><li>Ingredient 2</li><li>...</li></ul>");

	ingredients_box = new QLabel(ingredients,this,"ingredients");
	ingredients_box->setFrameShape( QFrame::Box );
	QToolTip::add(ingredients_box,i18n("Ingredients"));

	box_properties->insert( ingredients_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );

	//========================PROPERTIES========================//
	properties_box = new QLabel(i18n("<ul><li>Property 1</li><li>Property 2</li><li>...</li></ul>"),this,"properties");
	properties_box->setFrameShape( QFrame::Box );
	QToolTip::add(properties_box,i18n("Properties"));

	box_properties->insert( properties_box, Font | BackgroundColor | TextColor | Visibility | Geometry | Alignment );
}

void SetupDisplay::widgetClicked( QMouseEvent *e, QWidget *w )
{
	if ( e->button() == QMouseEvent::RightButton )
	{
		popup = new KPopupMenu( w ); //parent _must_ be widget acting on
		popup->insertTitle( ( w == this ) ? i18n("Background") : QToolTip::textFor(w) );

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
			popup->setItemChecked( id, w->isShown() );
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
		has_changes = true;

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
		has_changes = true;

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
		has_changes = true;

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
		has_changes = true;
	
		QWidget *box = static_cast<QWidget*>(obj);
		box->setShown( !popup->isItemChecked(id) );
		emit itemVisibilityChanged( box, !popup->isItemChecked(id) );
		
		setWidget( 0 );
	}
}

void SetupDisplay::setAlignment( QAction *action )
{
	QObject *obj = popup->parent();
	if ( obj->inherits("QLabel") )
	{
		has_changes = true;
	
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

void SetupDisplay::setItemShown( QWidget *item, bool visible )
{
	item->raise(); //raise it to make sure the user can see it (and doesn't think it didn't work)
	item->setShown( visible );
	
	setWidget( 0 );
	
	has_changes = true;
}

/*
**  Rescales the dimensions according to the page size
*/

void SetupDisplay::toAbsolute(PreciseRect *r)
{
	r->setWidth(r->width()/100.0*width());
	r->setHeight(r->height()/100.0*width());

	r->setLeft(r->left()/100.0*width());
	r->setTop(r->top()/100.0*width());
}

/*
** Set in percentages respect to the width
*/

void SetupDisplay::toPercentage(PreciseRect *r)
{
r->setWidth(r->width()*100.0/width());
r->setHeight(r->height()*100.0/width());

r->setLeft(r->left()*100.0/width());
r->setTop(r->top()*100.0/width());
}

QSize SetupDisplay::minimumSize() const
{
	return(QSize(300,400));
}

QSize SetupDisplay::sizeHint(void) const
{
	return (minimumSize());
}

void SetupDisplay::changeMade(void)
{
	has_changes = true;
}

#include "setupdisplay.moc"
