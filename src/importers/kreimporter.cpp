/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreimporter.h"

#include <klocale.h>
#include <kdebug.h>

#include <qfile.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3CString>
#include <kstandarddirs.h>

#include "datablocks/recipe.h"
#include "datablocks/categorytree.h"

KreImporter::KreImporter() : BaseImporter()
{}

void KreImporter::parseFile( const QString &filename )
{
	QFile * file = 0;
	bool unlink = false;
	kdDebug() << "loading file: %s" << filename << endl;

	if ( filename.right( 6 ) == ".kreml" ) {
		file = new QFile( filename );
	}
	else if ( filename.right( 4 ) == ".kre" ) {
		kdDebug() << "file is an archive" << endl;
		KTar* kre = new KTar( filename, "application/x-gzip" );
		kre->open( QIODevice::ReadOnly );
		const KArchiveDirectory* dir = kre->directory();
		QString name;
		QStringList fileList = dir->entries();
		for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it ) {
			if ( ( *it ).right( 6 ) == ".kreml" ) {
				name = *it;
			}
		}
		if ( name.isEmpty() ) {
			kdDebug() << "error: Archive doesn't contain a valid Krecipes file" << endl;
			setErrorMsg( i18n( "Archive does not contain a valid Krecipes file" ) );
			return ;
		}
		QString tmp_dir = locateLocal( "tmp", "" );
		dir->copyTo( tmp_dir );
		file = new QFile( tmp_dir + name );
		kre->close();
		unlink = true; //remove file after import
	}
	else {
		setErrorMsg( i18n( "File extension does not match that of a valid Krecipes file." ) );
		return ;
	}

	if ( file->open( QIODevice::ReadOnly ) ) {
		kdDebug() << "file opened" << endl;
		QDomDocument doc;
		QString error;
		int line;
		int column;
		if ( !doc.setContent( file, &error, &line, &column ) ) {
			kdDebug() << QString( "error: \"%1\" at line %2, column %3" ).arg( error ).arg( line ).arg( column ) << endl;
			setErrorMsg( QString( i18n( "\"%1\" at line %2, column %3" ) ).arg( error ).arg( line ).arg( column ) );
			return ;
		}

		QDomElement kreml = doc.documentElement();

		if ( kreml.tagName() != "krecipes" ) {
			setErrorMsg( i18n( "This file does not appear to be a *.kreml file" ) );
			return ;
		}

		// TODO Check if there are changes between versions
		QString kreVersion = kreml.attribute( "version" );
		kdDebug() << QString( i18n( "KreML version %1" ) ).arg( kreVersion ) << endl;

		QDomNodeList r = kreml.childNodes();
		QDomElement krecipe;

		for ( unsigned z = 0; z < r.count(); z++ ) {
			krecipe = r.item( z ).toElement();
			QDomNodeList l = krecipe.childNodes();
			if ( krecipe.tagName() == "krecipes-recipe" ) {
				Recipe recipe;
				for ( unsigned i = 0; i < l.count(); i++ ) {
					QDomElement el = l.item( i ).toElement();
					if ( el.tagName() == "krecipes-description" ) {
						readDescription( el.childNodes(), &recipe );
					}
					if ( el.tagName() == "krecipes-ingredients" ) {
						readIngredients( el.childNodes(), &recipe );
					}
					if ( el.tagName() == "krecipes-instructions" ) {
						recipe.instructions = el.text().stripWhiteSpace();
					}
				}
				add
					( recipe );
			}
			else if ( krecipe.tagName() == "krecipes-category-structure" ) {
				CategoryTree * tree = new CategoryTree;
				readCategoryStructure( l, tree );
				setCategoryStructure( tree );
			}
		}
	}
	if ( unlink ) {
		file->remove
		();
	}
}

KreImporter::~KreImporter()
{
}

void KreImporter::readCategoryStructure( const QDomNodeList& l, CategoryTree *tree )
{
	for ( unsigned i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();

		QString category = el.attribute( "name" );
		CategoryTree *child_node = tree->add
		                           ( Element( category ) );
		readCategoryStructure( el.childNodes(), child_node );
	}
}

void KreImporter::readDescription( const QDomNodeList& l, Recipe *recipe )
{
	for ( unsigned i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		if ( el.tagName() == "title" ) {
			recipe->title = el.text();
			kdDebug() << "Found title: " << recipe->title << endl;
		}
		else if ( el.tagName() == "author" ) {
			kdDebug() << "Found author: " << el.text() << endl;
			recipe->authorList.append( Element( el.text() ) );
		}
		else if ( el.tagName() == "serving" ) {
			recipe->persons = el.text().toInt();
		}
		else if ( el.tagName() == "preparation-time" ) {
			recipe->prepTime = QTime::fromString( el.text() );
		}
		else if ( el.tagName() == "category" ) {
			QDomNodeList categories = el.childNodes();
			for ( unsigned j = 0; j < categories.count(); j++ ) {
				QDomElement c = categories.item( j ).toElement();
				if ( c.tagName() == "cat" ) {
					kdDebug() << "Found category: " << QString( c.text() ).stripWhiteSpace() << endl;
					recipe->categoryList.append( Element( QString( c.text() ).stripWhiteSpace() ) );
				}
			}
		}
		else if ( el.tagName() == "pictures" ) {
			if ( el.hasChildNodes() ) {
				QDomNodeList pictures = el.childNodes();
				for ( unsigned j = 0; j < pictures.count(); j++ ) {
					QDomElement pic = pictures.item( j ).toElement();
					if ( pic.tagName() == "pic" )
						kdDebug() << "Found photo" << endl;
					QPixmap pix;
					QByteArray picData = KCodecs::base64Decode( pic.text().utf8() );
					bool ok = pix.loadFromData( picData, "JPEG" );
					if ( ok ) {
						recipe->photo = pix;
					}
				}
			}
		}
	}
}

void KreImporter::readIngredients( const QDomNodeList& l, Recipe *recipe, const QString &header )
{
	for ( unsigned i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		if ( el.tagName() == "ingredient" ) {
			QDomNodeList ingredient = el.childNodes();
			Ingredient new_ing;
			for ( unsigned j = 0; j < ingredient.count(); j++ ) {
				QDomElement ing = ingredient.item( j ).toElement();
				if ( ing.tagName() == "name" ) {
					new_ing.name = QString( ing.text() ).stripWhiteSpace();
					kdDebug() << "Found ingredient: " << new_ing.name << endl;
				}
				else if ( ing.tagName() == "amount" ) {
					new_ing.amount = ( QString( ing.text() ).stripWhiteSpace() ).toDouble();
				}
				else if ( ing.tagName() == "unit" ) {
					new_ing.units = Unit( ing.text().stripWhiteSpace(), new_ing.amount );
				}
				else if ( ing.tagName() == "prep" ) {
					new_ing.prepMethod = QString( ing.text() ).stripWhiteSpace();
				}
			}
			new_ing.group = header;
			recipe->ingList.append( new_ing );
		}
		else if ( el.tagName() == "ingredient-group" ) {
			readIngredients( el.childNodes(), recipe, el.attribute( "name" ) );
		}
	}
}


