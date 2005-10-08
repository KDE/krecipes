/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include <stdlib.h>

#include "qsqlrecipedb.h"
#include "datablocks/categorytree.h"
#include "datablocks/rating.h"

#include <qbuffer.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmdcodec.h>

int QSqlRecipeDB::m_refCount = 0;

QSqlRecipeDB::QSqlRecipeDB( const QString &host, const QString &user, const QString &pass, const QString &name ) : RecipeDB(),
	connectionName("connection" + QString::number( m_refCount+1 ))
{
	DBuser = user;
	DBpass = pass;
	DBhost = host;
	DBname = name;

	dbOK = false; //it isn't ok until we've connect()'ed
	++m_refCount;
}

QSqlRecipeDB::~QSqlRecipeDB()
{
	if ( dbOK ) {
		database->close();
	}

	QSqlDatabase::removeDatabase( connectionName );
	--m_refCount;
}

void QSqlRecipeDB::connect( bool create_db, bool create_tables )
{
	kdDebug() << i18n( "QSqlRecipeDB: Opening Database..." ) << endl;
	kdDebug() << "Parameters: \n\thost: " << DBhost << "\n\tuser: " << DBuser << "\n\ttable: " << DBname << endl;

	bool driver_found = false;

	if ( qsqlDriver() ) //we're using a built-in driver
		driver_found = true;
	else {
		QStringList drivers = QSqlDatabase::drivers();
		for ( QStringList::const_iterator it = drivers.begin(); it != drivers.end(); ++it ) {
			if ( ( *it ) == qsqlDriverPlugin() ) {
				driver_found = true;
				break;
			}
		}
	}

	if ( !driver_found ) {
		dbErr = QString( i18n( "The Qt database plug-in (%1) is not installed.  This plug-in is required for using this database backend." ) ).arg( qsqlDriverPlugin() );
		return ;
	}

	//we need to have a unique connection name for each QSqlRecipeDB class as multiple db's may be open at once (db to db transfer)
	if ( qsqlDriver() )
		database = QSqlDatabase::addDatabase( qsqlDriver(), connectionName );
	else if ( !qsqlDriverPlugin().isEmpty() )
		database = QSqlDatabase::addDatabase( qsqlDriverPlugin(), connectionName );
	else
		kdDebug()<<"Fatal internal error!  Backend incorrectly written!"<<endl;

	database->setDatabaseName( DBname );
	if ( !( DBuser.isNull() ) )
		database->setUserName( DBuser );
	if ( !( DBpass.isNull() ) )
		database->setPassword( DBpass );
	database->setHostName( DBhost );

	kdDebug() << i18n( "Parameters set. Calling db->open()" ) << endl;

	if ( !database->open() ) {
		//Try to create the database
		if ( create_db ) {
			kdDebug() << i18n( "Failing to open database. Trying to create it" ) << endl;
			createDB();
		}
		else {
			// Handle the error (passively)
			dbErr = QString( i18n( "Krecipes could not open the database using the driver '%2' (with username: \"%1\"). You may not have the necessary permissions, or the server may be down." ) ).arg( DBuser ).arg( qsqlDriverPlugin() );
		}

		//Now Reopen the Database and signal & exit if it fails
		if ( !database->open() ) {
			QString error = i18n( "Database message: %1" ).arg( database->lastError().databaseText() );
			kdDebug() << i18n( "Failing to open database. Exiting\n" ).latin1();

			// Handle the error (passively)
			dbErr = QString( i18n( "Krecipes could not open the database using the driver '%2' (with username: \"%1\"). You may not have the necessary permissions, or the server may be down." ) ).arg( DBuser ).arg( qsqlDriverPlugin() );
			return ;
		}
	}

	if ( int( qRound( databaseVersion() * 1e5 ) ) > int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		dbErr = i18n( "This database was created with a newer version of Krecipes and cannot be opened." );
		return ;
	}

	// Check integrity of the database (tables). If not possible, exit
	// Because checkIntegrity() will create tables if they don't exist,
	// we don't want to run this when creating the database.  We would be
	// logged in as another user (usually the superuser and not have ownership of the tables
	if ( create_tables && !checkIntegrity() ) {
		kdError() << i18n( "Failed to fix database structure. Exiting.\n" ).latin1();
		kapp->exit( 1 );
	}

	// Database was opened correctly
	m_query = QSqlQuery( QString::null, database );
	m_query.setForwardOnly(true);
	dbOK = true;
}

void QSqlRecipeDB::execSQL( const QString &command )
{
	database->exec( command );
}

void QSqlRecipeDB::loadRecipes( RecipeList *rlist, int items, QValueList<int> ids )
{
	// Empty the recipe first
	rlist->empty();

	QMap <int, RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

	QString command;

	QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	QStringList ids_str;
	for ( QValueList<int>::const_iterator it = ids.begin(); it != ids.end(); ++it ) {
		QString number_str = QString::number(*it);
		ids_str << number_str;

		if ( !(items & RecipeDB::Noatime) )
 			database->exec( "UPDATE recipes SET atime='"+current_timestamp+"' WHERE id="+number_str );
	}

	// Read title, author, yield, and instructions as specified
	command = "SELECT id";
	if ( items & RecipeDB::Title ) command += ",title";
	if ( items & RecipeDB::Instructions ) command += ",instructions";
	if ( items & RecipeDB::PrepTime ) command += ",prep_time";
	if ( items & RecipeDB::Yield ) command += ",yield_amount,yield_amount_offset,yield_type_id";
	command += " FROM recipes"+(ids_str.count()!=0?" WHERE id IN ("+ids_str.join(",")+")":"");

	QSqlQuery recipeQuery(command,database);
	if ( recipeQuery.isActive() ) {
		while ( recipeQuery.next() ) {
			int row_at = 0;

			Recipe recipe;
			recipe.recipeID = recipeQuery.value( row_at ).toInt(); ++row_at;

			if ( items & RecipeDB::Title ) {
				 recipe.title = unescapeAndDecode( recipeQuery.value( row_at ).toString() ); ++row_at;
			}

			if ( items & RecipeDB::Instructions ) {
				recipe.instructions = unescapeAndDecode( recipeQuery.value( row_at ).toString() ); ++row_at;
			}

			if ( items & RecipeDB::PrepTime ) {
				 recipe.prepTime = recipeQuery.value( row_at ).toTime(); ++row_at;
			}

			if ( items & RecipeDB::Yield ) {
				recipe.yield.amount = recipeQuery.value( row_at ).toDouble(); ++row_at;
				recipe.yield.amount_offset = recipeQuery.value( row_at ).toDouble(); ++row_at;
				recipe.yield.type_id = recipeQuery.value( row_at ).toInt(); ++row_at;
				if ( recipe.yield.type_id != -1 ) {
					QString y_command = QString("SELECT name FROM yield_types WHERE id=%1;").arg(recipe.yield.type_id);
					QSqlQuery yield_query(y_command,database);
					if ( yield_query.isActive() && yield_query.first() )
						recipe.yield.type = unescapeAndDecode(yield_query.value( 0 ).toString());
					else
						kdDebug()<<yield_query.lastError().text()<<endl;
				}
			}

			if ( items & RecipeDB::Meta )
				loadRecipeMetadata(&recipe);

			recipeIterators[ recipe.recipeID ] = rlist->append( recipe );
		}
	}

	// Read the ingredients
	if ( items & RecipeDB::Ingredients ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			if ( !(items & RecipeDB::NamesOnly) )
				command = QString( "SELECT il.ingredient_id,i.name,il.amount,il.amount_offset,u.id,u.name,u.plural,il.group_id,il.id FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index;" ).arg( (*recipe_it).recipeID );
			else
				command = QString( "SELECT il.ingredient_id,i.name FROM ingredients i, ingredient_list il WHERE il.recipe_id=%1 AND i.id=il.ingredient_id;" ).arg( (*recipe_it).recipeID );

			QSqlQuery ingredientQuery( command, database );
			if ( ingredientQuery.isActive() ) {
				RecipeList::Iterator it = recipeIterators[ (*recipe_it).recipeID ];
				while ( ingredientQuery.next() ) {
					Ingredient ing;
					ing.ingredientID = ingredientQuery.value( 0 ).toInt();
					ing.name = unescapeAndDecode( ingredientQuery.value( 1 ).toString() );

					if ( !(items & RecipeDB::NamesOnly) ) {
						ing.amount = ingredientQuery.value( 2 ).toDouble();
						ing.amount_offset = ingredientQuery.value( 3 ).toDouble();
						ing.unitID = ingredientQuery.value( 4 ).toInt();
						ing.units.name = unescapeAndDecode( ingredientQuery.value( 5 ).toString() );
						ing.units.plural = unescapeAndDecode( ingredientQuery.value( 6 ).toString() );
	
						//if we don't have both name and plural, use what we have as both
						if ( ing.units.name.isEmpty() )
							ing.units.name = ing.units.plural;
						else if ( ing.units.plural.isEmpty() )
							ing.units.plural = ing.units.name;
			
						ing.groupID = ingredientQuery.value( 7 ).toInt();
						if ( ing.groupID != -1 ) {
							QSqlQuery toLoad( QString( "SELECT name FROM ingredient_groups WHERE id=%1" ).arg( ing.groupID ), database );
							if ( toLoad.isActive() && toLoad.first() )
								ing.group = unescapeAndDecode( toLoad.value( 0 ).toString() );
						}

						command = QString("SELECT pl.prep_method_id,p.name FROM prep_methods p, prep_method_list pl WHERE pl.ingredient_list_id=%1 AND p.id=pl.prep_method_id ORDER BY pl.order_index;").arg(ingredientQuery.value( 8 ).toInt());
						QSqlQuery ingPrepMethodsQuery( command, database );
						if ( ingPrepMethodsQuery.isActive() ) {
							while ( ingPrepMethodsQuery.next() ) {
								ing.prepMethodList.append( Element(ingPrepMethodsQuery.value(1).toString(),ingPrepMethodsQuery.value(0).toInt()) );
							}
						}
					}
					(*it).ingList.append( ing );
				}
			}
		}
	}

	//Load the Image
	if ( items & RecipeDB::Photo ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			loadPhoto( (*it).recipeID, (*it).photo );
		}
	}

	//Load the category list
	if ( items & RecipeDB::Categories ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			
			command = QString( "SELECT cl.category_id,c.name FROM category_list cl, categories c WHERE recipe_id=%1 AND cl.category_id=c.id;" ).arg( (*it).recipeID );
		
			m_query.exec( command );
			if ( m_query.isActive() ) {
				while ( m_query.next() ) {
					Element el;
					el.id = m_query.value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query.value( 1 ).toString() );
					(*it).categoryList.append( el );
				}
			}
		}
	}

	//Load the author list
	if ( items & RecipeDB::Authors ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];

			command = QString( "SELECT al.author_id,a.name FROM author_list al, authors a WHERE recipe_id=%1 AND al.author_id=a.id;" ).arg( (*it).recipeID );
		
			m_query.exec( command );
			if ( m_query.isActive() ) {
				while ( m_query.next() ) {
					Element el;
					el.id = m_query.value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query.value( 1 ).toString() );
					(*it).authorList.append( el );
				}
			}
		}
	}

	//Load the ratings
	if ( items & RecipeDB::Ratings ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			
			command = QString( "SELECT id,comment,rater FROM rating WHERE recipe_id=%1 ORDER BY created DESC" ).arg( (*it).recipeID );
			QSqlQuery query( command, database );
			if ( query.isActive() ) {
				while ( query.next() ) {
					Rating r;
					r.id = query.value( 0 ).toInt();
					r.comment = unescapeAndDecode( query.value( 1 ).toString() );
					r.rater = unescapeAndDecode( query.value( 2 ).toString() );

					command = QString( "SELECT rc.id,rc.name,rl.stars FROM rating_criterion rc, rating_criterion_list rl WHERE rating_id=%1 AND rl.rating_criteria_id=rc.id" ).arg(r.id);
					QSqlQuery criterionQuery( command, database );
					if ( criterionQuery.isActive() ) {
						while ( criterionQuery.next() ) {
							RatingCriteria rc;
							rc.id = criterionQuery.value( 0 ).toInt();
							rc.name = unescapeAndDecode( criterionQuery.value( 1 ).toString() );
							rc.stars = criterionQuery.value( 2 ).toDouble();
							r.append( rc );
						}
					}

					(*it).ratingList.append( r );
				}
			}
		}
	}
}

void QSqlRecipeDB::loadIngredientGroups( ElementList *list )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredient_groups ORDER BY name;";
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element group;
			group.id = m_query.value( 0 ).toInt();
			group.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( group );
		}
	}
}

void QSqlRecipeDB::loadIngredients( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredients ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element ing;
			ing.id = m_query.value( 0 ).toInt();
			ing.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( ing );
		}
	}
}

void QSqlRecipeDB::loadPrepMethods( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM prep_methods ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element prep_method;
			prep_method.id = m_query.value( 0 ).toInt();
			prep_method.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( prep_method );
		}
	}
}

void QSqlRecipeDB::loadYieldTypes( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM yield_types ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query.exec( command );

	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element el;
			el.id = m_query.value( 0 ).toInt();
			el.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::createNewPrepMethod( const QString &prepMethodName )
{
	QString command;
	QString real_name = prepMethodName.left( maxPrepMethodNameLength() );

	command = QString( "INSERT INTO prep_methods VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "prep_methods", "id" ) );
	QSqlQuery prepMethodToCreate( command, database );

	emit prepMethodCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modPrepMethod( int prepMethodID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE prep_methods SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( prepMethodID );
	QSqlQuery prepMethodToCreate( command, database );

	emit prepMethodRemoved( prepMethodID );
	emit prepMethodCreated( Element( newLabel, prepMethodID ) );
}

void QSqlRecipeDB::modProperty( int propertyID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredient_properties SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( propertyID );
	QSqlQuery createQuery( command, database );

	emit propertyRemoved( propertyID );
	emit propertyCreated( propertyName( propertyID ) );
}

void QSqlRecipeDB::loadPossibleUnits( int ingredientID, UnitList *list )
{
	list->clear();

	QString command;

	command = QString( "SELECT u.id,u.name,u.plural FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;" ).arg( ingredientID );

	QSqlQuery unitToLoad( command, database );

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.id = unitToLoad.value( 0 ).toInt();
			unit.name = unescapeAndDecode( unitToLoad.value( 1 ).toString() );
			unit.plural = unescapeAndDecode( unitToLoad.value( 2 ).toString() );
			list->append( unit );
		}
	}


}

void QSqlRecipeDB::storePhoto( int recipeID, const QByteArray &data )
{
	QSqlQuery query( QString::null, database );

	query.prepare( "UPDATE recipes SET photo=? WHERE id=" + QString::number( recipeID ) );
	query.addBindValue( KCodecs::base64Encode( data ) );
	query.exec();
}

void QSqlRecipeDB::loadPhoto( int recipeID, QPixmap &photo )
{
	QString command = QString( "SELECT photo FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSqlQuery query( command, database );

	if ( query.isActive() && query.first() ) {
		QCString decodedPic;
		QPixmap pix;
		KCodecs::base64Decode( QCString( query.value( 0 ).toString().latin1() ), decodedPic );
		int len = decodedPic.size();

		if ( len > 0 ) {
			QByteArray picData( len );
			memcpy( picData.data(), decodedPic.data(), len );
	
			bool ok = pix.loadFromData( picData, "JPEG" );
			if ( ok )
				photo = pix;
		}
	}
}

void QSqlRecipeDB::loadRecipeMetadata( Recipe *recipe )
{
	QString command = "SELECT ctime,mtime,atime FROM recipes WHERE id="+QString::number(recipe->recipeID);

	QSqlQuery query( command, database );
	if ( query.isActive() && query.first() ) {
		recipe->ctime = query.value(0).toDateTime();
		recipe->mtime = query.value(1).toDateTime();
		recipe->atime = query.value(2).toDateTime();
	}
}

void QSqlRecipeDB::saveRecipe( Recipe *recipe )
{
	// Check if it's a new recipe or it exists (supossedly) already.

	bool newRecipe;
	newRecipe = ( recipe->recipeID == -1 );
	// First check if the recipe ID is set, if so, update (not create)
	// Be carefull, first check if the recipe hasn't been deleted while changing.

	QSqlQuery recipeToSave( QString::null, database );

	QString command;

	QDateTime current_datetime = QDateTime::currentDateTime();
	QString current_timestamp = current_datetime.toString(Qt::ISODate);
	if ( newRecipe ) {
		command = QString( "INSERT INTO recipes VALUES ("+getNextInsertIDStr("recipes","id")+",'%1',%2,'%3','%4','%5',NULL,'%6','%7','%8','%9');" )  // Id is autoincremented
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount )
		          .arg( recipe->yield.amount_offset )
		          .arg( recipe->yield.type_id )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( current_timestamp )
		          .arg( current_timestamp )
		          .arg( current_timestamp );
		recipe->mtime = recipe->ctime = recipe->atime = current_datetime;
	}
	else {
		command = QString( "UPDATE recipes SET title='%1',yield_amount='%2',yield_amount_offset='%3',yield_type_id='%4',instructions='%5',prep_time='%6',mtime='%8' WHERE id=%7;" )
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount )
		          .arg( recipe->yield.amount_offset )
		          .arg( recipe->yield.type_id )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( recipe->recipeID )
		          .arg( current_timestamp );
		recipe->mtime = current_datetime;
	}
	recipeToSave.exec( command );

	// If it's a new recipe, identify the ID that was given to the recipe and store in the Recipe itself
	int recipeID;
	if ( newRecipe ) {
		recipeID = lastInsertID();
		recipe->recipeID = recipeID;
	}
	recipeID = recipe->recipeID;
	loadRecipeMetadata(recipe);

	// Let's begin storing the Image!
	if ( !recipe->photo.isNull() ) {
		QByteArray ba;
		QBuffer buffer( ba );
		buffer.open( IO_WriteOnly );
		QImageIO iio( &buffer, "JPEG" );
		iio.setImage( recipe->photo.convertToImage() );
		iio.write();
		//recipe->photo.save( &buffer, "JPEG" ); don't need QImageIO in QT 3.2

		storePhoto( recipeID, ba );
	}
	else {
		recipeToSave.exec( "UPDATE recipes SET photo=NULL, mtime=mtime, ctime=ctime, atime=atime WHERE id=" + QString::number( recipeID ) );
	}

	// Save the ingredient list (first delete if we are updating)
	command = QString( "SELECT id FROM ingredient_list WHERE recipe_id=%1" ).arg(recipeID);
	recipeToSave.exec( command );
	if ( recipeToSave.isActive() ) {
		while ( recipeToSave.next() ) {
			command = QString("DELETE FROM prep_method_list WHERE ingredient_list_id=%1")
			  .arg(recipeToSave.value(0).toInt());
			database->exec(command);
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	int order_index = 0;
	for ( IngredientList::const_iterator ing_it = recipe->ingList.begin(); ing_it != recipe->ingList.end(); ++ing_it ) {
		order_index++;
		QString ing_list_id_str = getNextInsertIDStr("ingredient_list","id");
		command = QString( "INSERT INTO ingredient_list VALUES (%1,%2,%3,%4,%5,%6,%7,%8);" )
		          .arg( ing_list_id_str )
		          .arg( recipeID )
		          .arg( ( *ing_it ).ingredientID )
		          .arg( ( *ing_it ).amount )
		          .arg( ( *ing_it ).amount_offset )
		          .arg( ( *ing_it ).unitID )
		          .arg( order_index )
		          .arg( ( *ing_it ).groupID );
		recipeToSave.exec( command );

		int ing_list_id = lastInsertID();
		int prep_order_index = 0;
		for ( ElementList::const_iterator prep_it = (*ing_it).prepMethodList.begin(); prep_it != (*ing_it).prepMethodList.end(); ++prep_it ) {
			prep_order_index++;
			command = QString( "INSERT INTO prep_method_list VALUES (%1,%2,%3);" )
				.arg( ing_list_id )
				.arg( ( *prep_it ).id )
				.arg( prep_order_index );
			recipeToSave.exec( command );
		}
	}

	// Save the category list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	ElementList::const_iterator cat_it = recipe->categoryList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--cat_it;
	for ( unsigned int i = 0; i < recipe->categoryList.count(); i++ ) {
		command = QString( "INSERT INTO category_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *cat_it ).id );
		recipeToSave.exec( command );

		--cat_it;
	}

	//Add the default category -1 to ease and speed up searches

	command = QString( "INSERT INTO category_list VALUES (%1,-1);" )
	          .arg( recipeID );
	recipeToSave.exec( command );


	// Save the author list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM author_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	ElementList::const_iterator author_it = recipe->authorList.end(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--author_it;
	for ( unsigned int i = 0; i < recipe->authorList.count(); i++ ) {
		command = QString( "INSERT INTO author_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *author_it ).id );
		recipeToSave.exec( command );

		--author_it;
	}

	// Save the ratings (first delete if we are updating)
	command = QString( "SELECT id FROM rating WHERE recipe_id=%1" ).arg(recipeID);
	recipeToSave.exec( command );
	if ( recipeToSave.isActive() ) {
		while ( recipeToSave.next() ) {
			command = QString("DELETE FROM rating_criterion_list WHERE rating_id=%1")
			  .arg(recipeToSave.value(0).toInt());
			database->exec(command);
		}
	}
	command = QString( "DELETE FROM rating WHERE recipe_id=%1" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	for ( RatingList::const_iterator rating_it = recipe->ratingList.begin(); rating_it != recipe->ratingList.end(); ++rating_it ) {
		command = QString( "INSERT INTO rating VALUES("+QString(getNextInsertIDStr("rating","id"))+","+QString::number(recipeID)+",'"+(*rating_it).comment+"','"+(*rating_it).rater+"','"+current_timestamp+"')" );
kdDebug()<<"calling: "<<command<<endl;
		recipeToSave.exec( command );
		int rating_id = lastInsertID();
		
		for ( QValueList<RatingCriteria>::const_iterator rc_it = (*rating_it).ratingCriteriaList.begin(); rc_it != (*rating_it).ratingCriteriaList.end(); ++rc_it ) {
			command = QString( "INSERT INTO rating_criterion_list VALUES("+QString::number(rating_id)+","+QString::number((*rc_it).id)+","+QString::number((*rc_it).stars)+")" );
kdDebug()<<"calling: "<<command<<endl;
			recipeToSave.exec( command );
		}
	}
	

	if ( newRecipe )
		emit recipeCreated( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
	else
		emit recipeModified( Element( recipe->title.left( maxRecipeTitleLength() ), recipeID ), recipe->categoryList );
}

void QSqlRecipeDB::loadRecipeList( ElementList *list, int categoryID, bool recursive )
{
	QString command;

	if ( categoryID == -1 )  // load just the list
		command = "SELECT id,title FROM recipes;";
	else  // load the list of those in the specified category
		command = QString( "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1 ORDER BY r.title" ).arg( categoryID );

	if ( recursive ) {
		QSqlQuery subcategories( QString("SELECT id FROM categories WHERE parent_id='%1'").arg(categoryID), database );
		if ( subcategories.isActive() ) {
			while ( subcategories.next() ) {
				loadRecipeList(list,subcategories.value( 0 ).toInt(),true);
			}
		}
	}

	QSqlQuery recipeToLoad( command, database );

	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			Element recipe;
			recipe.id = recipeToLoad.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeToLoad.value( 1 ).toString() );
			list->append( recipe );
		}
	}
}


void QSqlRecipeDB::loadUncategorizedRecipes( ElementList *list )
{
	list->clear();

	QString command = "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id GROUP BY id HAVING COUNT(*)=1 ORDER BY r.title DESC";
	m_query.exec( command );
	if ( m_query.isActive() ) {
		while ( m_query.next() ) {
			Element recipe;
			recipe.id = m_query.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( m_query.value( 1 ).toString() );
			list->append( recipe );
		}
	}
}



void QSqlRecipeDB::removeRecipe( int id )
{
	emit recipeRemoved( id );

	QString command;

	command = QString( "DELETE FROM recipes WHERE id=%1;" ).arg( id );
	QSqlQuery recipeToRemove( command, database );
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id=%1;" ).arg( id );
	recipeToRemove.exec( command );
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" ).arg( id );
	recipeToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	QStringList ids;
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	recipeToRemove.exec( command );
	if ( recipeToRemove.isActive() ) {
		while ( recipeToRemove.next() ) {
			if ( recipeToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( recipeToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	recipeToRemove.exec( command );
}

void QSqlRecipeDB::removeRecipeFromCategory( int recipeID, int categoryID )
{
	QString command;
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;" ).arg( recipeID ).arg( categoryID );
	QSqlQuery recipeToRemove( command, database );

	emit recipeRemoved( recipeID, categoryID );
}

void QSqlRecipeDB::createNewIngGroup( const QString &name )
{
	QString command;
	QString real_name = name.left( maxIngGroupNameLength() );

	command = QString( "INSERT INTO ingredient_groups VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredient_groups", "id" ) );
	QSqlQuery query( command, database );

	emit ingGroupCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::createNewIngredient( const QString &ingredientName )
{
	QString command;
	QString real_name = ingredientName.left( maxIngredientNameLength() );

	command = QString( "INSERT INTO ingredients VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredients", "id" ) );
	QSqlQuery ingredientToCreate( command, database );

	emit ingredientCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::createNewRating( const QString &rating )
{
	QString command;
	QString real_name = rating/*.left( maxIngredientNameLength() )*/;

	command = QString( "INSERT INTO rating_criterion VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "rating_criterion", "id" ) );
	QSqlQuery toCreate( command, database );

	//emit ratingCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::createNewYieldType( const QString &name )
{
	QString command;
	QString real_name = name.left( maxYieldTypeLength() );

	command = QString( "INSERT INTO yield_types VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "yield_types", "id" ) );
	database->exec(command);

	//emit yieldTypeCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modIngredientGroup( int groupID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredient_groups SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( groupID );
	QSqlQuery ingredientToCreate( command, database );

	emit ingGroupRemoved( groupID );
	emit ingGroupCreated( Element( newLabel, groupID ) );
}

void QSqlRecipeDB::modIngredient( int ingredientID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredients SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( ingredientID );
	QSqlQuery ingredientToCreate( command, database );

	emit ingredientRemoved( ingredientID );
	emit ingredientCreated( Element( newLabel, ingredientID ) );
}

void QSqlRecipeDB::addUnitToIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "INSERT INTO unit_list VALUES(%1,%2);" ).arg( ingredientID ).arg( unitID );
	QSqlQuery ingredientToCreate( command, database );
}

void QSqlRecipeDB::loadUnits( UnitList *list, int limit, int offset )
{
	list->clear();

	QString command;

	command = "SELECT id,name,plural FROM units ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));

	QSqlQuery unitToLoad( command, database );

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.id = unitToLoad.value( 0 ).toInt();
			unit.name = unescapeAndDecode( unitToLoad.value( 1 ).toString() );
			unit.plural = unescapeAndDecode( unitToLoad.value( 2 ).toString() );
			list->append( unit );
		}
	}
}

void QSqlRecipeDB::removeUnitFromIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, database );

	// Remove any recipe using that combination of ingredients also (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			emit recipeRemoved( unitToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( unitToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit and ingredient
	command = QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );

	// Remove any ingredient properties from ingredient_info where the this ingredient+unit is being used (user must have been warned before calling this function!)
	command = QString( "DELETE FROM ingredient_info ii WHERE ii.ingredient_id=%1 AND ii.per_units=%2;" ).arg( ingredientID ).arg( unitID );
	unitToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			if ( unitToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );
}


void QSqlRecipeDB::findUseOf_Ing_Unit_InRecipes( ElementList *results, int ingredientID, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2" ).arg( ingredientID ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOfIngInRecipes( ElementList *results, int ingredientID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::removeIngredientGroup( int groupID )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredient_groups WHERE id=%1" ).arg( groupID );
	QSqlQuery toDelete( command, database );

	// Remove all the unit entries for this ingredient

	command = QString( "UPDATE ingredient_list SET group_id='-1' WHERE group_id=%1" ).arg( groupID );
	toDelete.exec( command );

	emit ingGroupRemoved( groupID );
}

void QSqlRecipeDB::removeIngredient( int ingredientID )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredients WHERE id=%1;" ).arg( ingredientID );
	QSqlQuery ingredientToDelete( command, database );

	// Remove all the unit entries for this ingredient

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	// Remove any recipe using that ingredient

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			emit recipeRemoved( ingredientToDelete.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( ingredientToDelete.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this ingredient
	command = QString( "DELETE FROM ingredient_list WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			ids << QString::number( ingredientToDelete.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	ingredientToDelete.exec( command );
	if ( ingredientToDelete.isActive() ) {
		while ( ingredientToDelete.next() ) {
			if ( ingredientToDelete.value( 0 ).toInt() != -1 )
				ids << QString::number( ingredientToDelete.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	ingredientToDelete.exec( command );

	// Remove property list of this ingredient
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1;" ).arg( ingredientID );
	ingredientToDelete.exec( command );

	emit ingredientRemoved( ingredientID );
}

void QSqlRecipeDB::addProperty( const QString &name, const QString &units )
{
	QString command;
	QString real_name = name.left( maxPropertyNameLength() );

	command = QString( "INSERT INTO ingredient_properties VALUES(%3,'%1','%2');" )
	          .arg( escapeAndEncode( real_name ) )
	          .arg( escapeAndEncode( units ) )
	          .arg( getNextInsertIDStr( "ingredient_properties", "id" ) );
	QSqlQuery propertyToAdd( command, database );

	emit propertyCreated( IngredientProperty( real_name, units, lastInsertID() ) );
}

void QSqlRecipeDB::loadProperties( IngredientPropertyList *list, int ingredientID )
{
	list->clear();
	QString command;
	bool usePerUnit;
	if ( ingredientID >= 0 )  // Load properties of this ingredient
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id  FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID );
	}
	else if ( ingredientID == -1 )  // Load the properties of all the ingredients
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.property_id=ip.id AND ii.per_units=u.id;" );
	}
	else // Load the whole property list (just the list of possible properties, not the ingredient properties)
	{
		usePerUnit = false;
		command = QString( "SELECT  id,name,units FROM ingredient_properties;" );
	}

	QSqlQuery propertiesToLoad ( command, database );
	// Load the results into the list
	if ( propertiesToLoad.isActive() ) {
		while ( propertiesToLoad.next() ) {
			IngredientProperty prop;
			prop.id = propertiesToLoad.value( 0 ).toInt();
			prop.name = unescapeAndDecode( propertiesToLoad.value( 1 ).toString() );
			prop.units = unescapeAndDecode( propertiesToLoad.value( 2 ).toString() );
			if ( usePerUnit ) {
				prop.perUnit.id = propertiesToLoad.value( 3 ).toInt();
				prop.perUnit.name = unescapeAndDecode( propertiesToLoad.value( 4 ).toString() );
			}
			else {
				prop.perUnit.id = -1;
				prop.perUnit.name = QString::null;
			}

			if ( ingredientID >= -1 )
				prop.amount = propertiesToLoad.value( 5 ).toDouble();
			else
				prop.amount = -1; // Property is generic, not attached to an ingredient

			if ( ingredientID >= -1 )
				prop.ingredientID = propertiesToLoad.value( 6 ).toInt();

			list->add( prop );
		}
	}
}

void QSqlRecipeDB::changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units )
{
	QString command;
	command = QString( "UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;" ).arg( amount ).arg( ingredientID ).arg( propertyID ).arg( per_units );
	QSqlQuery infoToChange( command, database );
}

void QSqlRecipeDB::addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID )
{
	QString command;

	command = QString( "INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);" ).arg( ingredientID ).arg( propertyID ).arg( amount ).arg( perUnitsID );
	QSqlQuery propertyToAdd( command, database );
}


void QSqlRecipeDB::removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID )
{
	QString command;
	// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitID );
	QSqlQuery propertyToRemove( command, database );
}

void QSqlRecipeDB::removeProperty( int propertyID )
{
	QString command;

	// Remove property from the ingredient_properties
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1;" ).arg( propertyID );
	QSqlQuery propertyToRemove( command, database );

	// Remove any ingredient info that uses this property
	command = QString( "DELETE FROM ingredient_info WHERE property_id=%1;" ).arg( propertyID );
	propertyToRemove.exec( command );

	emit propertyRemoved( propertyID );
}

void QSqlRecipeDB::removeUnit( int unitID )
{
	QString command;
	// Remove the unit first
	command = QString( "DELETE FROM units WHERE id=%1;" ).arg( unitID );
	QSqlQuery unitToRemove( command, database );

	//Remove the unit from ingredients using it

	command = QString( "DELETE FROM unit_list WHERE unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );


	// Remove any recipe using that unit in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			emit recipeRemoved( unitToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( unitToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this unit
	command = QString( "DELETE FROM ingredient_list WHERE unit_id=%1;" ).arg( unitID );
	unitToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			if ( unitToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( unitToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	unitToRemove.exec( command );

	// Remove the ingredient properties using this unit (user must be warned before calling this function)
	command = QString( "DELETE FROM ingredient_info WHERE per_units=%1;" ).arg( unitID );
	unitToRemove.exec( command );

	// Remove the unit conversion ratios with this unit
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=%1 OR unit2_id=%2;" ).arg( unitID ).arg( unitID );
	unitToRemove.exec( command );

	emit unitRemoved( unitID );
}

void QSqlRecipeDB::removePrepMethod( int prepMethodID )
{
	QString command;
	// Remove the prep method first
	command = QString( "DELETE FROM prep_methods WHERE id=%1;" ).arg( prepMethodID );
	QSqlQuery prepMethodToRemove( command, database );

	// Remove any recipe using that prep method in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT r.id FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1;" ).arg( prepMethodID );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			emit recipeRemoved( prepMethodToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( prepMethodToRemove.value( 0 ).toInt() ) );
		}
	}

	// Remove any ingredient in ingredient_list which has references to this prep method
	command = QString( "DELETE FROM ingredient_list WHERE prep_method_id=%1;" ).arg( prepMethodID );
	prepMethodToRemove.exec( command );

	// Clean up ingredient_list which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_list WHERE recipe_id NOT IN ( SELECT id FROM recipes );)
	QStringList ids;
	command = QString( "SELECT id FROM recipes;" );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			ids << QString::number( prepMethodToRemove.value( 0 ).toInt() );
		}
	}

	command = QString( "DELETE FROM ingredient_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	// Clean up category_list which have no recipe that they belong to. Same method as above
	command = QString( "DELETE FROM category_list WHERE recipe_id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	// Clean up ingredient_groups which have no recipe that they belong to
	// MySQL doesn't support subqueries until 4.1, so we'll do this the long way
	// (Easy way: DELETE FROM ingredient_groups WHERE id NOT IN ( SELECT DISTINCT(group_id) FROM ingredient_list );)
	ids.clear();
	command = QString( "SELECT DISTINCT(group_id) FROM ingredient_list;" );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			if ( prepMethodToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( prepMethodToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM ingredient_groups WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	prepMethodToRemove.exec( command );

	emit prepMethodRemoved( prepMethodID );
}


void QSqlRecipeDB::createNewUnit( const QString &unitName, const QString &unitPlural )
{
	QString command;
	QString real_name = unitName.left( maxUnitNameLength() ).stripWhiteSpace();
	QString real_plural = unitPlural.left( maxUnitNameLength() ).stripWhiteSpace();

	if ( real_name.isEmpty() )
		real_name = real_plural;
	else if ( real_plural.isEmpty() )
		real_plural = real_name;

	command = "INSERT INTO units VALUES(" + getNextInsertIDStr( "units", "id" ) + ",'" + QString(escapeAndEncode( real_name )) + "','" + QString(escapeAndEncode( real_plural )) + "');";
	QSqlQuery unitToCreate( command, database );

	emit unitCreated( Unit( real_name, real_plural, lastInsertID() ) );
}


void QSqlRecipeDB::modUnit( int unitID, const QString &newName, const QString &newPlural )
{
	QSqlQuery unitQuery( QString::null, database );

	unitQuery.exec( "UPDATE units SET name='" + QString( escapeAndEncode( newName ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );
	unitQuery.exec( "UPDATE units SET plural='" + QString( escapeAndEncode( newPlural ) ) + "' WHERE id='" + QString::number( unitID ) + "';" );

	emit unitRemoved( unitID );
	emit unitCreated( Unit( newName, newPlural, unitID ) );
}

void QSqlRecipeDB::findUseOf_Unit_InRecipes( ElementList *results, int unitID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeFound.value( 1 ).toString() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOf_Unit_InProperties( ElementList *results, int unitID )
{
	QString command = QString( "SELECT ip.id,ip.name FROM ingredient_info ii, ingredient_properties ip WHERE ii.per_units=%1 AND ip.id=ii.property_id;" ).arg( unitID );
	QSqlQuery recipeFound( command, database ); // Find the entries

	// Populate data in the ElementList*
	if ( recipeFound.isActive() ) {
		while ( recipeFound.next() ) {
			Element recipe;
			recipe.id = recipeFound.value( 0 ).toInt();
			recipe.name = recipeFound.value( 1 ).toString();
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOfIngGroupInRecipes( ElementList *results, int groupID )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.group_id=%1" ).arg( groupID );
	QSqlQuery query( command, database );

	// Populate data
	if ( query.isActive() ) {
		while ( query.next() ) {
			Element recipe;
			recipe.id = query.value( 0 ).toInt();
			recipe.name = query.value( 1 ).toString();
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOfCategoryInRecipes( ElementList *results, int catID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1" ).arg( catID );
	QSqlQuery query( command, database );

	// Populate data
	if ( query.isActive() ) {
		while ( query.next() ) {
			Element recipe;
			recipe.id = query.value( 0 ).toInt();
			recipe.name = query.value( 1 ).toString();
			results->append( recipe );
		}
	}

	//recursively find dependenacies in subcategories
	command = QString( "SELECT id FROM categories WHERE parent_id=%1" ).arg( catID );
	QSqlQuery findDeps = database->exec( command );
	if ( findDeps.isActive() ) {
		while ( findDeps.next() ) {
			findUseOfCategoryInRecipes(results,findDeps.value( 0 ).toInt() );
		}
	}
}

void QSqlRecipeDB::loadUnitRatios( UnitRatioList *ratioList )
{
	ratioList->clear();

	QString command;
	command = "SELECT unit1_id,unit2_id,ratio FROM units_conversion;";
	QSqlQuery ratiosToLoad( command, database );

	if ( ratiosToLoad.isActive() ) {
		while ( ratiosToLoad.next() ) {
			UnitRatio ratio;
			ratio.uID1 = ratiosToLoad.value( 0 ).toInt();
			ratio.uID2 = ratiosToLoad.value( 1 ).toInt();
			ratio.ratio = ratiosToLoad.value( 2 ).toDouble();
			ratioList->add
			( ratio );
		}
	}
}

void QSqlRecipeDB::saveUnitRatio( const UnitRatio *ratio )
{
	QString command;

	// Check if it's a new ratio or it exists already.
	command = QString( "SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( ratio->uID1 ).arg( ratio->uID2 ); // Find ratio between units

	QSqlQuery ratioFound( command, database ); // Find the entries
	bool newRatio = ( ratioFound.size() == 0 );

	if ( newRatio )
		command = QString( "INSERT INTO units_conversion VALUES(%1,%2,%3);" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );
	else
		command = QString( "UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2" ).arg( ratio->uID1 ).arg( ratio->uID2 ).arg( ratio->ratio );

	ratioFound.exec( command ); // Enter the new ratio
}

double QSqlRecipeDB::unitRatio( int unitID1, int unitID2 )
{

	if ( unitID1 == unitID2 )
		return ( 1.0 );
	QString command;

	command = QString( "SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( unitID1 ).arg( unitID2 );
	QSqlQuery ratioToLoad( command, database );

	if ( ratioToLoad.isActive() ) {
		if ( ratioToLoad.next() )
			return ( ratioToLoad.value( 0 ).toDouble() );
		else
			return ( -1 ); // There is no ratio defined between the units
	}

	else {
		return ( -1 );
	}
}

//Finds data dependant on this Ingredient/Unit combination
void QSqlRecipeDB::findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo )
{

	// Recipes using that combination

	QString command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, database );
	loadElementList( recipes, &unitToRemove );
	// Ingredient info using that combination
	command = QString( "SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID ).arg( unitID );

	unitToRemove.exec( command );
	loadPropertyElementList( ingredientInfo, &unitToRemove );
}

void QSqlRecipeDB::findIngredientDependancies( int ingredientID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1" ).arg( ingredientID );

	QSqlQuery ingredientToRemove( command, database );
	loadElementList( recipes, &ingredientToRemove );
}



//Finds data dependant on the removal of this Unit
void QSqlRecipeDB::findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes )
{

	// Ingredient-Info (ingredient->property) using this Unit

	QString command = QString( "SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id;" ).arg( unitID );
	QSqlQuery unitToRemove( command, database );
	loadPropertyElementList( properties, &unitToRemove );

	// Recipes using this Unit
	command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1;" ).arg( unitID ); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
	unitToRemove.exec( command );
	loadElementList( recipes, &unitToRemove );

}

void QSqlRecipeDB::findPrepMethodDependancies( int prepMethodID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.prep_method_id=%1" ).arg( prepMethodID );

	QSqlQuery prepMethodToRemove( command, database );
	loadElementList( recipes, &prepMethodToRemove );
}


void QSqlRecipeDB::loadElementList( ElementList *elList, QSqlQuery *query )
{
	if ( query->isActive() ) {
		while ( query->next() ) {
			Element el;
			el.id = query->value( 0 ).toInt();
			el.name = unescapeAndDecode( query->value( 1 ).toString() );
			elList->append( el );
		}
	}
}
// See function "findUnitDependancies" for use
void QSqlRecipeDB::loadPropertyElementList( ElementList *elList, QSqlQuery *query )
{
	if ( query->isActive() ) {
		while ( query->next() ) {
			Element el;
			el.id = -1; // There's no ID for the ingredient-property combination
			QString ingName = query->value( 0 ).toString();
			QString propName = unescapeAndDecode( query->value( 1 ).toString() );
			QString propUnits = unescapeAndDecode( query->value( 2 ).toString() );
			QString propPerUnits = unescapeAndDecode( query->value( 3 ).toString() );

			el.name = QString( "In ingredient %1: property \"%2\" [%3/%4]" ).arg( ingName ).arg( propName ).arg( propUnits ).arg( propPerUnits );
			elList->append( el );
		}
	}
}

QCString QSqlRecipeDB::escapeAndEncode( const QString &s ) const
{
	QString s_escaped = s;

	s_escaped.replace ( "'", "\\'" );
	s_escaped.replace ( ";", "\";@" ); // Small trick for only for parsing later on

	return ( s_escaped.utf8() );
}

QString QSqlRecipeDB::unescapeAndDecode( const QString &s ) const
{
	QString s_escaped = QString::fromUtf8( s.latin1() );
	s_escaped.replace( "\";@", ";" );
	return ( s_escaped ); // Use unicode encoding
}

bool QSqlRecipeDB::ingredientContainsUnit( int ingredientID, int unitID )
{
	QString command = QString( "SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

bool QSqlRecipeDB::ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID )
{
	QString command = QString( "SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitsID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

QString QSqlRecipeDB::categoryName( int ID )
{
	QString command = QString( "SELECT name FROM categories WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode( toLoad.value( 0 ).toString() ) );

	return ( QString::null );
}

IngredientProperty QSqlRecipeDB::propertyName( int ID )
{
	QString command = QString( "SELECT name,units FROM ingredient_properties WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		return ( IngredientProperty( unescapeAndDecode( toLoad.value( 0 ).toString() ), unescapeAndDecode( toLoad.value( 1 ).toString() ), ID ) );
	}

	return ( IngredientProperty( QString::null, QString::null ) );
}

Unit QSqlRecipeDB::unitName( int ID )
{
	QString command = QString( "SELECT name,plural FROM units WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		Unit unit( unescapeAndDecode( toLoad.value( 0 ).toString() ), unescapeAndDecode( toLoad.value( 1 ).toString() ) );

		//if we don't have both name and plural, use what we have as both
		if ( unit.name.isEmpty() )
			unit.name = unit.plural;
		else if ( unit.plural.isEmpty() )
			unit.plural = unit.name;

		return unit;
	}

	return Unit();
}

int QSqlRecipeDB::getCount( const QString &table_name )
{
	QSqlQuery count( "SELECT COUNT(1) FROM "+table_name, database );
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

int QSqlRecipeDB::categoryTopLevelCount()
{
	QSqlQuery count( "SELECT COUNT(1) FROM categories WHERE parent_id='-1'", database );
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

bool QSqlRecipeDB::checkIntegrity( void )
{


	// Check existence of the necessary tables (the database may be created, but empty)
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "db_info" << "prep_methods" << "ingredient_groups" << "yield_types" << "prep_method_list" << "rating" << "rating_criterion" << "rating_criterion_list";

	QStringList existingTableList = database->tables();kdDebug()<<"found tables: "<<database->tables()<<endl;
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		bool found = false;

		for ( QStringList::Iterator ex_it = existingTableList.begin(); ( ( ex_it != existingTableList.end() ) && ( !found ) ); ++ex_it ) {
			found = ( *ex_it == *it );
		}

		if ( !found ) {
			kdDebug() << "Recreating missing table: " << *it << "\n";
			createTable( *it );
		}
	}


	// Check for older versions, and port

	kdDebug() << "Checking database version...\n";
	float version = databaseVersion();
	kdDebug() << "version found... " << version << " \n";
	kdDebug() << "latest version... " << latestDBVersion() << endl;
	if ( int( qRound( databaseVersion() * 1e5 ) ) < int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		switch ( KMessageBox::questionYesNo( 0, i18n( "<!doc>The database was created with a previous version of Krecipes.  Would you like Krecipes to update this database to work with this version of Krecipes?  Depending on the number of recipes and amount of data, this could take some time.<br><br><b>Warning: After updating, this database will no longer be compatible with previous versions of Krecipes.<br><br>Cancelling this operation may result in corrupting the database.</b>" ) ) ) {
		case KMessageBox::Yes:
			emit progressBegin(0,QString::null,i18n("Porting database structure..."),50);
			portOldDatabases( version );
			emit progressDone();
			break;
		case KMessageBox::No:
			return false;
		}
	}

	return true;
}

void QSqlRecipeDB::splitCommands( QString& s, QStringList& sl )
{
	sl = QStringList::split( QRegExp( ";{1}(?!@)" ), s );
}

void QSqlRecipeDB::portOldDatabases( float /* version */ )
{}

float QSqlRecipeDB::databaseVersion( void )
{

	QString command = "SELECT ver FROM db_info";
	QSqlQuery dbVersion( command, database );

	if ( dbVersion.isActive() && dbVersion.next() )
		return ( dbVersion.value( 0 ).toDouble() ); // There should be only one (or none for old DB) element, so go to first
	else
		return ( 0.2 ); // if table is empty, assume oldest (0.2), and port
}

void QSqlRecipeDB::loadRatingCriterion( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM rating_criteria ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery toLoad( command, database );
	if ( toLoad.isActive() ) {
		while ( toLoad.next() ) {
			Element el;
			el.id = toLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( toLoad.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::loadCategories( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM categories ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery categoryToLoad( command, database );
	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			Element el;
			el.id = categoryToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::loadCategories( CategoryTree *list, int limit, int offset, int parent_id, bool recurse )
{
	QString limit_str;
	if ( parent_id == -1 ) {
		emit progressBegin(0,QString::null,i18n("Loading category list"));
		list->clear();

		//only limit the number of top-level categories
		limit_str = (limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset);
	}

	QString command = "SELECT id,name,parent_id FROM categories WHERE parent_id='"+QString::number(parent_id)+"' ORDER BY name "+limit_str;

	QSqlQuery categoryToLoad( QString::null, database );
	//categoryToLoad.setForwardOnly(true); //FIXME? Subcategories aren't loaded if this is enabled, even though we only go forward
	categoryToLoad.exec(command);

	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			emit progress();

			int id = categoryToLoad.value( 0 ).toInt();
			Element el;
			el.id = id;
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toString() );
			CategoryTree *list_child = list->add( el );

			if ( recurse ) {
				//QTime dbg_timer; dbg_timer.start(); kdDebug()<<"   calling QSqlRecipeDB::loadCategories"<<endl;
				loadCategories( list_child, -1, -1, id ); //limit and offset won't be used
				// kdDebug()<<"   done in "<<dbg_timer.elapsed()<<" ms"<<endl;
			}
		}
	}

	if ( parent_id == -1 )
		emit progressDone();
}

void QSqlRecipeDB::createNewCategory( const QString &categoryName, int parent_id )
{
	QString command;
	QString real_name = categoryName.left( maxCategoryNameLength() );

	command = QString( "INSERT INTO categories VALUES(%3,'%1',%2);" )
	          .arg( escapeAndEncode( real_name ) )
	          .arg( parent_id )
	          .arg( getNextInsertIDStr( "categories", "id" ) );
	QSqlQuery categoryToCreate( command, database );

	emit categoryCreated( Element( real_name, lastInsertID() ), parent_id );
}

void QSqlRecipeDB::modCategory( int categoryID, const QString &newLabel )
{
	QString command = QString( "UPDATE categories SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, database );

	emit categoryModified( Element( newLabel, categoryID ) );
}

void QSqlRecipeDB::modCategory( int categoryID, int new_parent_id )
{
	QString command = QString( "UPDATE categories SET parent_id=%1 WHERE id=%2;" ).arg( new_parent_id ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, database );

	emit categoryModified( categoryID, new_parent_id );
}

void QSqlRecipeDB::removeCategory( int categoryID )
{
	QString command;

	command = QString( "DELETE FROM categories WHERE id=%1;" ).arg( categoryID );
	QSqlQuery categoryToRemove( command, database );

	command = QString( "DELETE FROM category_list WHERE category_id=%1;" ).arg( categoryID );
	categoryToRemove.exec( command );

	//recursively delete subcategories
	command = QString( "SELECT id FROM categories WHERE parent_id=%1;" ).arg( categoryID );
	categoryToRemove.exec( command );
	if ( categoryToRemove.isActive() ) {
		while ( categoryToRemove.next() ) {
			removeCategory( categoryToRemove.value( 0 ).toInt() );
		}
	}

	emit categoryRemoved( categoryID );
}


void QSqlRecipeDB::loadAuthors( ElementList *list, int limit, int offset )
{
	list->clear();
	QString command = "SELECT id,name FROM authors ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery authorToLoad( command, database );
	if ( authorToLoad.isActive() ) {
		while ( authorToLoad.next() ) {
			Element el;
			el.id = authorToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( authorToLoad.value( 1 ).toString() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::createNewAuthor( const QString &authorName )
{
	QString command;
	QString real_name = authorName.left( maxAuthorNameLength() );

	command = QString( "INSERT INTO authors VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "authors", "id" ) );
	QSqlQuery authorToCreate( command, database );

	emit authorCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modAuthor( int authorID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE authors SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( authorID );
	QSqlQuery authorToCreate( command, database );

	emit authorRemoved( authorID );
	emit authorCreated( Element( newLabel, authorID ) );
}

void QSqlRecipeDB::removeAuthor( int authorID )
{
	QString command;

	command = QString( "DELETE FROM authors WHERE id=%1;" ).arg( authorID );
	QSqlQuery authorToRemove( command, database );

	emit authorRemoved( authorID );
}

int QSqlRecipeDB::findExistingUnitsByName( const QString& name, int ingredientID, ElementList *list )
{
	QString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds
	QString command;
	if ( ingredientID < 0 )  // We're looking for units with that name all through the table, no specific ingredient
	{
		command = "SELECT id,name FROM units WHERE name='" + search_str + "' OR plural='" + search_str + "';";
	}
	else // Look for units  with that name for the specified ingredient
	{
		command = "SELECT u.id,u.name FROM units u, unit_list ul WHERE u.id=ul.unit_id AND ul.ingredient_id=" + QString::number( ingredientID ) + " AND ( u.name='" + search_str + "' OR u.plural='" + search_str + "' );";
	}

	QSqlQuery unitsToLoad( command, database ); // Run the query

	if ( list )  //If the pointer exists, then load all the values found into it
	{
		if ( unitsToLoad.isActive() )
		{
			while ( unitsToLoad.next() ) {
				Element el;
				el.id = unitsToLoad.value( 0 ).toInt();
				el.name = unescapeAndDecode( unitsToLoad.value( 1 ).toString() );
				list->append( el );
			}
		}
	}

	return ( unitsToLoad.size() );
}

int QSqlRecipeDB::findExistingAuthorByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxAuthorNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM authors WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingCategoryByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxCategoryNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM categories WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingIngredientGroupByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxIngGroupNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_groups WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingIngredientByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxIngredientNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredients WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPrepByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPrepMethodNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM prep_methods WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPropertyByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxPropertyNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_properties WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingUnitByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds

	QString command = "SELECT id FROM units WHERE name='" + search_str + "' OR plural='" + search_str + "';";
	QSqlQuery elementToLoad( command, database ); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingRatingByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM rating_criterion WHERE name='%1'" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingRecipeByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxRecipeTitleLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM recipes WHERE title='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingYieldTypeByName( const QString& name )
{
	QCString search_str = escapeAndEncode( name.left( maxYieldTypeLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM yield_types WHERE name='%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, database ); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

void QSqlRecipeDB::mergeAuthors( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE author_list SET author_id=%1 WHERE author_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM author_list WHERE author_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM author_list WHERE author_id=%1 AND recipe_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//remove author with id 'id2'
	command = QString( "DELETE FROM authors WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit authorRemoved( id2 );
}

void QSqlRecipeDB::mergeCategories( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE category_list SET category_id=%1 WHERE category_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT recipe_id FROM category_list WHERE category_id=%1 ORDER BY recipe_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM category_list WHERE category_id=%1 AND recipe_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	command = QString( "UPDATE categories SET parent_id=%1 WHERE parent_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//we don't want to have a category be its own parent...
	command = QString( "UPDATE categories SET parent_id=-1 WHERE parent_id=id" );
	update.exec( command );

	//remove category with id 'id2'
	command = QString( "DELETE FROM categories WHERE id=%1" ).arg( id2 );
	update.exec( command );

	emit categoriesMerged( id1, id2 );
}

void QSqlRecipeDB::mergeIngredientGroups( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_list SET group_id=%1 WHERE group_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//remove ingredient with id 'id2'
	command = QString( "DELETE FROM ingredient_groups WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit ingGroupRemoved( id2 );
}

void QSqlRecipeDB::mergeIngredients( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//delete nutrient info associated with ingredient with id 'id2'
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1" )
	          .arg( id2 );
	update.exec( command );

	//update the unit_list
	command = QString( "UPDATE unit_list SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT unit_id FROM unit_list WHERE ingredient_id=%1 ORDER BY unit_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//update ingredient info
	command = QString( "UPDATE ingredient_info SET ingredient_id=%1 WHERE ingredient_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	//info associated with one ingredient will be lost... they should be the same ingredient and thus info anyways
	command = QString( "SELECT property_id FROM ingredient_info WHERE ingredient_id=%1 ORDER BY property_id" )
	          .arg( id1 );
	update.exec( command );
	last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//remove ingredient with id 'id2'
	command = QString( "DELETE FROM ingredients WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit ingredientRemoved( id2 );
}

void QSqlRecipeDB::mergePrepMethods( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE ingredient_list SET prep_method_id=%1 WHERE prep_method_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM prep_methods WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit prepMethodRemoved( id2 );
}

void QSqlRecipeDB::mergeProperties( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1'
	QString command = QString( "UPDATE ingredient_properties SET id=%1 WHERE id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	command = QString( "UPDATE ingredient_info SET property_id=%1 WHERE property_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//remove prep method with id 'id2'
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit propertyRemoved( id2 );
}

void QSqlRecipeDB::mergeUnits( int id1, int id2 )
{
	QSqlQuery update( QString::null, database );

	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE unit_list SET unit_id=%1 WHERE unit_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in ingredient list
	command = QString( "UPDATE ingredient_list SET unit_id=%1 WHERE unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure no duplicates were created in this process
	command = QString( "SELECT ingredient_id FROM unit_list WHERE unit_id=%1 ORDER BY ingredient_id" )
	          .arg( id1 );
	update.exec( command );
	int last_id = -1;
	if ( update.isActive() ) {
		while ( update.next() ) {
			int current_id = update.value( 0 ).toInt();
			if ( last_id == current_id ) {
				command = QString( "DELETE FROM unit_list WHERE unit_id=%1 AND ingredient_id=%2 LIMIT 1" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove
					( command, database );

			}
			last_id = current_id;
		}
	}

	//update ingredient info
	command = QString( "UPDATE ingredient_info SET per_units=%1 WHERE per_units=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in unit_conversion
	command = QString( "UPDATE units_conversion SET unit1_id=%1 WHERE unit1_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );
	command = QString( "UPDATE units_conversion SET unit2_id=%1 WHERE unit2_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//and ensure that the one to one ratio wasn't created
	command = QString( "DELETE FROM units_conversion WHERE unit1_id=unit2_id" );
	update.exec( command );

	//remove units with id 'id2'
	command = QString( "DELETE FROM units WHERE id=%1" ).arg( id2 );
	update.exec( command );
	emit unitRemoved( id2 );
}

QString QSqlRecipeDB::getUniqueRecipeTitle( const QString &recipe_title )
{
	//already is unique
	if ( findExistingRecipeByName( recipe_title ) == -1 )
		return recipe_title;

	QString return_title = recipe_title; //If any error is produced, just go for default value (always return something)

	QString command = QString( "SELECT COUNT(DISTINCT title) FROM recipes WHERE title LIKE '%1 (%)';" ).arg( escapeAndEncode( recipe_title ) );

	QSqlQuery alikeRecipes( command, database );
	if ( alikeRecipes.isActive() && alikeRecipes.first() )
		;
	{
		int count = alikeRecipes.value( 0 ).toInt();
		return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );

		//make sure this newly created title is unique (just in case)
		while ( findExistingRecipeByName( return_title ) != -1 ) {
			count--; //go down to find the skipped recipe(s)
			return_title = QString( "%1 (%2)" ).arg( recipe_title ).arg( count + 2 );
		}
	}

	return return_title;
}

QString QSqlRecipeDB::recipeTitle( int recipeID )
{
	QString command = QString( "SELECT title FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSqlQuery recipeToLoad( command, database );
	if ( recipeToLoad.isActive() && recipeToLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode(recipeToLoad.value( 0 ).toString()) );

	return ( QString::null );
}

void QSqlRecipeDB::emptyData( void )
{
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "ingredient_groups" << "yield_types";
	QSqlQuery tablesToEmpty( QString::null, database );
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		tablesToEmpty.exec( command );
	}
}

void QSqlRecipeDB::empty( void )
{
	QSqlQuery tablesToEmpty( QString::null, database );

	QStringList list = database->tables();
	QStringList::const_iterator it = list.begin();
	while( it != list.end() ) {
		QString command = QString( "DROP TABLE %1;" ).arg( *it );
		tablesToEmpty.exec( command );

		if ( !tablesToEmpty.isActive() )
			kdDebug()<<tablesToEmpty.lastError().text()<<endl;

		++it;
	}
}

QString QSqlRecipeDB::getNextInsertIDStr( const QString &table, const QString &column )
{
	int next_id = getNextInsertID( table, column );

	QString id_str;
	if ( next_id == -1 )
		id_str = "NULL";
	else
		id_str = QString::number( next_id );

	return id_str;
}

void QSqlRecipeDB::search( RecipeList *list, int items, const RecipeSearchParameters &parameters )
{
	QString query = buildSearchQuery(parameters);

	QValueList<int> ids;
	QSqlQuery recipeToLoad( query, database );
	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			ids << recipeToLoad.value( 0 ).toInt();
		}
	}

	if ( ids.count() > 0 )
		loadRecipes( list, items, ids );
}

#include "qsqlrecipedb.moc"
