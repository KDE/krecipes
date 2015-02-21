/***************************************************************************
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreimporter.h"

#include <klocale.h>
#include <kdebug.h>

#include <QFile>
#include <QStringList>
#include <QPixmap>
#include <kstandarddirs.h>

#include "datablocks/recipe.h"
#include "datablocks/categorytree.h"

KreImporter::KreImporter() : BaseImporter()
{}

void KreImporter::parseFile( const QString &filename )
{
	QFile * file = 0;
	bool unlink = false;
	kDebug() << "loading file:" << filename ;

	if ( filename.right( 4 ) == ".kre" ) {
		file = new QFile( filename );
		kDebug() << "file is an archive" ;
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
			kDebug() << "error: Archive doesn't contain a valid Krecipes file" ;
			setErrorMsg( i18n( "Archive does not contain a valid Krecipes file" ) );
			return ;
		}
		QString tmp_dir = KStandardDirs::locateLocal( "tmp", "" );
		dir->copyTo( tmp_dir );
		file = new QFile( tmp_dir + name );
		kre->close();
		unlink = true; //remove file after import
	}
	else {
		file = new QFile( filename );
	}

	if ( file->open( QIODevice::ReadOnly ) ) {
		kDebug() << "file opened" ;
		QDomDocument doc;
		QString error;
		int line;
		int column;
		if ( !doc.setContent( file, &error, &line, &column ) ) {
			kDebug() << QString( "error: \"%1\" at line %2, column %3" ).arg( error ).arg( line ).arg( column ) ;
			setErrorMsg( i18n( "\"%1\" at line %2, column %3" , error , line, column ) );
			return ;
		}

		QDomElement kreml = doc.documentElement();

		if ( kreml.tagName() != "krecipes" ) {
			setErrorMsg( i18n( "This file does not appear to be a *.kreml file" ) );
			return ;
		}

		// TODO Check if there are changes between versions
		QString kreVersion = kreml.attribute( "version" );
		kDebug() << "KreML version" << kreVersion ;

		QDomNodeList r = kreml.childNodes();
		QDomElement krecipe;

		for ( int z = 0; z < r.count(); z++ ) {
			krecipe = r.item( z ).toElement();
			QDomNodeList l = krecipe.childNodes();
			if ( krecipe.tagName() == "krecipes-recipe" ) {
				Recipe recipe;
				for ( int i = 0; i < l.count(); i++ ) {
					QDomElement el = l.item( i ).toElement();
					if ( el.tagName() == "krecipes-description" ) {
						readDescription( el.childNodes(), &recipe );
					}
					if ( el.tagName() == "krecipes-ingredients" ) {
						readIngredients( el.childNodes(), &recipe );
					}
					if ( el.tagName() == "krecipes-instructions" ) {
						recipe.instructions = el.text().trimmed();
					}
					if ( el.tagName() == "krecipes-ratings" ) {
						readRatings( el.childNodes(), &recipe );
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
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();

		QString category = el.attribute( "name" );
		CategoryTree *child_node = tree->add
		                           ( Element( category ) );
		readCategoryStructure( el.childNodes(), child_node );
	}
}

void KreImporter::readDescription( const QDomNodeList& l, Recipe *recipe )
{
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		if ( el.tagName() == "title" ) {
			recipe->title = el.text();
			kDebug() << "Found title: " << recipe->title ;
		}
		else if ( el.tagName() == "author" ) {
			kDebug() << "Found author: " << el.text() ;
			recipe->authorList.append( Element( el.text() ) );
		}
		else if ( el.tagName() == "serving" ) { //### Keep for < 0.9 compatibility
			recipe->yield.setAmount(el.text().toInt());
		}
		else if ( el.tagName() == "yield" ) {
			QDomNodeList yield_children = el.childNodes();
			for ( int j = 0; j < yield_children.count(); j++ ) {
				QDomElement y = yield_children.item( j ).toElement();
				if ( y.tagName() == "amount" ) {
					double amount = 0.0, amountOffset = 0.0;
					readAmount(y, amount, amountOffset);
					recipe->yield.setAmount(amount);
					recipe->yield.setAmountOffset(amountOffset);
				}
				else if ( y.tagName() == "type" )
					recipe->yield.setType(y.text());
			}
		}
		else if ( el.tagName() == "preparation-time" ) {
			kDebug() << "Found preparation time: " << el.text();
			recipe->prepTime = QTime::fromString( el.text(), "hh:mm" );
		}
		else if ( el.tagName() == "category" ) {
			QDomNodeList categories = el.childNodes();
			for ( int j = 0; j < categories.count(); j++ ) {
				QDomElement c = categories.item( j ).toElement();
				if ( c.tagName() == "cat" ) {
					kDebug() << "Found category: " << QString( c.text() ).trimmed() ;
					recipe->categoryList.append( Element( QString( c.text() ).trimmed() ) );
				}
			}
		}
		else if ( el.tagName() == "pictures" ) {
			if ( el.hasChildNodes() ) {
				QDomNodeList pictures = el.childNodes();
				for ( int j = 0; j < pictures.count(); j++ ) {
					QDomElement pic = pictures.item( j ).toElement();
					QByteArray decodedPic;
					if ( pic.tagName() == "pic" )
						kDebug() << "Found photo" ;
					QPixmap pix;
					KCodecs::base64Decode( QByteArray( pic.text().toLatin1() ), decodedPic );
					int len = decodedPic.size();
					QByteArray picData;
					picData.resize( len );
					memcpy( picData.data(), decodedPic.data(), len );
					bool ok = pix.loadFromData( picData, "JPEG" );
					if ( ok ) {
						recipe->photo = pix;
					}
				}
			}
		}
	}
}

void KreImporter::readIngredients( const QDomNodeList& l, Recipe *recipe, const QString &header, Ingredient *ing )
{
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement el = l.item( i ).toElement();
		if ( el.tagName() == "ingredient" ) {
			QDomNodeList ingredient = el.childNodes();
			Ingredient new_ing;
			for ( int j = 0; j < ingredient.count(); j++ ) {
				QDomElement ing = ingredient.item( j ).toElement();
				if ( ing.tagName() == "name" ) {
					new_ing.name = QString( ing.text() ).trimmed();
					kDebug() << "Found ingredient: " << new_ing.name ;
				}
				else if ( ing.tagName() == "amount" ) {
					readAmount(ing,new_ing.amount,new_ing.amount_offset);
				}
				else if ( ing.tagName() == "unit" ) {
					new_ing.units = Unit( ing.text().trimmed(), new_ing.amount+new_ing.amount_offset );
				}
				else if ( ing.tagName() == "prep" ) {
					new_ing.prepMethodList = ElementList::split(",",QString( ing.text() ).trimmed());
				}
				else if ( ing.tagName() == "substitutes" ) {
					readIngredients(ing.childNodes(), recipe, header, &new_ing);
				}
			}
			new_ing.group = header;

			if ( !ing )
				recipe->ingList.append( new_ing );
			else
				ing->substitutes.append( new_ing );
		}
		else if ( el.tagName() == "ingredient-group" ) {
			readIngredients( el.childNodes(), recipe, el.attribute( "name" ) );
		}
	}
}

void KreImporter::readAmount( const QDomElement& amountEl, double &amount, double &amount_offset )
{
	QDomNodeList children = amountEl.childNodes();

	double min = 0,max = 0;
	for ( int i = 0; i < children.count(); i++ ) {
		QDomElement child = children.item( i ).toElement();
		if ( child.tagName() == "min" ) {
			min = ( QString( child.text() ).trimmed() ).toDouble();
		}
		else if ( child.tagName() == "max" )
			max = ( QString( child.text() ).trimmed() ).toDouble();
		else if ( child.tagName().isEmpty() )
			min = ( QString( amountEl.text() ).trimmed() ).toDouble();
	}

	amount = min;
	if ( max > 0 )
		amount_offset = max-min;
}

void KreImporter::readRatings( const QDomNodeList& l, Recipe *recipe )
{
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement child = l.item( i ).toElement();
		if ( child.tagName() == "rating" ) {
			Rating r;

			QDomNodeList ratingChildren = child.childNodes();
			for ( int j = 0; j < ratingChildren.count(); j++ ) {
				QDomElement ratingChild = ratingChildren.item( j ).toElement();
				if ( ratingChild.tagName() == "comment" ) {
					r.setComment(ratingChild.text());
				}
				else if ( ratingChild.tagName() == "rater" ) {
					r.setRater(ratingChild.text());
				}
				else if ( ratingChild.tagName() == "criterion" ) {
					readCriterion(ratingChild.childNodes(),r);
				}
			}
			recipe->ratingList.append(r);
		}
	}
}

void KreImporter::readCriterion( const QDomNodeList& l, Rating & r )
{
	for ( int i = 0; i < l.count(); i++ ) {
		QDomElement child = l.item( i ).toElement();

		if ( child.tagName() == "criteria" ) {
			RatingCriteria rc;

			QDomNodeList criteriaChildren = child.childNodes();
			for ( int j = 0; j < criteriaChildren.count(); j++ ) {
				QDomElement criteriaChild = criteriaChildren.item( j ).toElement();
		
				if ( criteriaChild.tagName() == "name" ) {
					rc.setName( criteriaChild.text() );
				}
				else if ( criteriaChild.tagName() == "stars" ) {
					rc.setStars( criteriaChild.text().toDouble() );
				}
			}
			r.append(rc);
		}
	}
}
