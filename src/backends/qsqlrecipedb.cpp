/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "qsqlrecipedb.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QByteArray>
#include <QImageWriter>
#include <QPixmap>
#include "datablocks/categorytree.h"
#include "datablocks/rating.h"
#include "datablocks/weight.h"

#include "propertycalculator.h"

#include <QBuffer>
#include <QTextCodec>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcodecs.h>

int QSqlRecipeDB::m_refCount = 0;

QSqlRecipeDB::QSqlRecipeDB( const QString &host, const QString &user, const QString &pass, const QString &name, int port ) : RecipeDB(),
	connectionName("connection" + QString::number( m_refCount+1 ))
{
	kDebug();
	DBuser = user;
	DBpass = pass;
	DBhost = host;
	DBname = name;
	DBport = port;

	dbOK = false; //it isn't ok until we've connect()'ed
	++m_refCount;

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Latin1"));  //this is the default, but let's explicitly set this to be sure
}

QSqlRecipeDB::~QSqlRecipeDB()
{
	kDebug();
	if ( dbOK ) {
		kDebug()<<" close :"<<*database;
		database->close();
	}
	
	delete database;
	if ( dbOK )
		delete m_query;
	QSqlDatabase::removeDatabase( connectionName );
	--m_refCount;
}

RecipeDB::Error QSqlRecipeDB::connect( bool create_db, bool create_tables )
{
	kDebug() << i18n( "QSqlRecipeDB: Opening Database..." ) ;
	kDebug() << "Parameters: \n\thost: " << DBhost << "\n\tuser: " << DBuser << "\n\ttable: " << DBname ;

	bool driver_found = false;

	if ( qsqlDriver() ) //we're using a built-in driver
		driver_found = true;
	else {
		const QStringList drivers = QSqlDatabase::drivers();
		for ( QStringList::const_iterator it = drivers.constBegin(); it != drivers.constEnd(); ++it ) {
			if ( ( *it ) == qsqlDriverPlugin() ) {
				driver_found = true;
				break;
			}
		}
	}

	if ( !driver_found ) {
		dbErr = i18n( "The Qt database plug-in (%1) is not installed.  This plug-in is required for using this database backend." , qsqlDriverPlugin() );
		return NoDriverFound;
	}

	//we need to have a unique connection name for each QSqlRecipeDB class as multiple db's may be open at once (db to db transfer)
	if ( qsqlDriver() ) {
		kDebug()<<" qsqlDriver() :"<<qsqlDriver()<< " connectionName "<<connectionName;
		database = new QSqlDatabase;
		*database = QSqlDatabase::addDatabase( qsqlDriver(), connectionName );
		kDebug()<<" database :"<<*database;
	}
	else if ( !qsqlDriverPlugin().isEmpty() ) {
		kDebug()<<" qsqlDriverPlugin() :"<<qsqlDriverPlugin()<<" connectionName :"<<connectionName;
		database = new QSqlDatabase;
		*database = QSqlDatabase::addDatabase( qsqlDriverPlugin(), connectionName );
		kDebug()<<"database :"<<*database;
	}
	else
		kDebug()<<"Fatal internal error!  Backend incorrectly written!";

	kDebug()<<" DBname :"<<DBname;
	database->setDatabaseName( DBname );
	if ( !( DBuser.isEmpty() ) )
		database->setUserName( DBuser );
	if ( !( DBpass.isEmpty() ) )
		database->setPassword( DBpass );
	database->setHostName( DBhost );
	if ( DBport > 0 )
		database->setPort(DBport);

	kDebug() << i18n( "Parameters set. Calling db->open()" ) ;

	if ( !database->open() ) {
		kDebug()<<" database.open false : create_db ? :"<<create_db;
		//Try to create the database
		if ( create_db ) {
			kDebug() << i18n( "Failing to open database. Trying to create it" ) ;
			createDB();
		}
		else {
			// Handle the error (passively)
			if ( DBuser.isEmpty() ) {
				dbErr = i18n( "Krecipes could not open the \"%1\" database.", DBname );
			}
			else {
				dbErr = i18n( "Krecipes could not open the database using the "
				"driver '%2' (with username: \"%1\"). You may not have the necessary "
				"permissions, or the server may be down." , DBuser , qsqlDriverPlugin() );
			}
		}

		//Now Reopen the Database and signal & exit if it fails
		if ( !database->open() ) {
			QString error = i18n( "Database message: %1" , database->lastError().databaseText() );
			kDebug() << i18n( "Failing to open database. Exiting\n" ).toLatin1();
			// Handle the error (passively)
			if ( DBuser.isEmpty() ) {
				dbErr = i18n( "Krecipes could not open the \"%1\" database.", DBname );
			}
			else {
				dbErr = i18n( "Krecipes could not open the database using the "
				"driver '%2' (with username: \"%1\"). You may not have the necessary "
				"permissions, or the server may be down." , DBuser , qsqlDriverPlugin() );
			}
			return RefusedByServer;
		}
	}

	if ( int( qRound( databaseVersion() * 1e5 ) ) > int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		dbErr = i18n( "This database was created with a newer version of Krecipes and cannot be opened." );
		return NewerDbVersion;
	}

	// Check integrity of the database (tables). If not possible, exit
	// Because checkIntegrity() will create tables if they don't exist,
	// we don't want to run this when creating the database.  We would be
	// logged in as another user (usually the superuser and not have ownership of the tables
	if ( create_tables && !checkIntegrity() ) {
		dbErr = i18n( "Failed to fix database structure." );
		return FixDbFailed;
	}

	// Database was opened correctly
	m_query = new QSqlQuery( QString(), *database );
	m_query->setForwardOnly(true);
	dbOK = true;
	return NoError;
}

void QSqlRecipeDB::execSQL( const QString &command )
{
	kDebug();
	database->exec( command );
}

void QSqlRecipeDB::loadRecipes( RecipeList *rlist, int items, QList<int> ids )
{
	// Empty the recipe list first
	rlist->clear();

	QMap <int, RecipeList::Iterator> recipeIterators; // Stores the iterator of each recipe in the list;

	QString command;

	QString current_timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);

	QStringList ids_str;
	for ( QList<int>::const_iterator it = ids.constBegin(); it != ids.constEnd(); ++it ) {
		QString number_str = QString::number(*it);
		ids_str << number_str;

		if ( !(items & RecipeDB::Noatime) )
			database->exec( "UPDATE recipes SET ctime=ctime,mtime=mtime,atime='"+current_timestamp+"' WHERE id="+number_str );
	}

	// Read title, author, yield, and instructions as specified
	command = "SELECT id";
	if ( items & RecipeDB::Title ) command += ",title";
	if ( items & RecipeDB::Instructions ) command += ",instructions";
	if ( items & RecipeDB::PrepTime ) command += ",prep_time";
	if ( items & RecipeDB::Yield ) command += ",yield_amount,yield_amount_offset,yield_type_id";
	command += " FROM recipes"+(ids_str.count()!=0?" WHERE id IN ("+ids_str.join(",")+')':"");

	QSqlQuery recipeQuery(command,*database);
	if ( recipeQuery.isActive() ) {
		while ( recipeQuery.next() ) {
			int row_at = 0;

			Recipe recipe;
			recipe.recipeID = recipeQuery.value( row_at ).toInt(); ++row_at;

			if ( items & RecipeDB::Title ) {
				 recipe.title = unescapeAndDecode( recipeQuery.value( row_at ).toByteArray() ); ++row_at;
			}

			if ( items & RecipeDB::Instructions ) {
				recipe.instructions = unescapeAndDecode( recipeQuery.value( row_at ).toByteArray() ); ++row_at;
			}

			if ( items & RecipeDB::PrepTime ) {
				recipe.prepTime = QTime::fromString(
					recipeQuery.value( row_at ).toString(),
					"hh:mm:ss" );
				++row_at;
			}

			if ( items & RecipeDB::Yield ) {
				//### toString().toString().toDouble() gets around that fact that
				// QCString().toString().toDouble() does a locale-aware
				// conversion, which isn't what we want.  The real fix is in the SQLite plugin,
				// but rather than spending time on that, I'll wait and use Qt4's plugins.
				recipe.yield.setAmount(recipeQuery.value( row_at ).toString().toDouble()); ++row_at;
				recipe.yield.setAmountOffset(recipeQuery.value( row_at ).toString().toDouble()); ++row_at;
				recipe.yield.setTypeId(recipeQuery.value( row_at ).toInt()); ++row_at;
				if ( recipe.yield.typeId() != -1 ) {
					QString y_command = QString("SELECT name FROM yield_types WHERE id=%1;").arg(recipe.yield.typeId());
					QSqlQuery yield_query(y_command,*database);
					if ( yield_query.isActive() && yield_query.first() )
						recipe.yield.setType(unescapeAndDecode(yield_query.value( 0 ).toByteArray()));
					else
						kDebug()<<yield_query.lastError().databaseText();
				}
			}

			if ( items & RecipeDB::Meta )
				loadRecipeMetadata(&recipe);

			rlist->append( recipe );
			recipeIterators[ recipe.recipeID ] = --(rlist->end());
		}
	}

	// Read the ingredients
	if ( items & RecipeDB::Ingredients ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			if ( items & RecipeDB::NamesOnly ) {
				if ( items & IngredientAmounts )
					command = QString( "SELECT il.ingredient_id,i.name,il.substitute_for,il.amount,il.amount_offset,u.id,u.type FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index" ).arg( (*recipe_it).recipeID );
				else
					command = QString( "SELECT il.ingredient_id,i.name,il.substitute_for FROM ingredients i, ingredient_list il WHERE il.recipe_id=%1 AND i.id=il.ingredient_id" ).arg( (*recipe_it).recipeID );
			}
			else
				command = QString( "SELECT il.ingredient_id,i.name,il.substitute_for,il.amount,il.amount_offset,u.id,u.name,u.plural,u.name_abbrev,u.plural_abbrev,u.type,il.group_id,il.id FROM ingredients i, ingredient_list il, units u WHERE il.recipe_id=%1 AND i.id=il.ingredient_id AND u.id=il.unit_id ORDER BY il.order_index" ).arg( (*recipe_it).recipeID );

			QSqlQuery ingredientQuery( command, *database);
			if ( ingredientQuery.isActive() ) {
				RecipeList::Iterator it = recipeIterators[ (*recipe_it).recipeID ];
				while ( ingredientQuery.next() ) {
					Ingredient ing;
					ing.ingredientID = ingredientQuery.value( 0 ).toInt();
					ing.name = unescapeAndDecode( ingredientQuery.value( 1 ).toByteArray() );

					if ( items & RecipeDB::NamesOnly ) {
						if ( items & IngredientAmounts ) {
							ing.amount = ingredientQuery.value( 3 ).toString().toDouble();
							ing.amount_offset = ingredientQuery.value( 4 ).toString().toDouble();
							ing.units.setId(ingredientQuery.value( 5 ).toInt());;
							ing.units.setType((Unit::Type)ingredientQuery.value( 6 ).toInt());
						}
					}
					else  {
						ing.amount = ingredientQuery.value( 3 ).toString().toDouble();
						ing.amount_offset = ingredientQuery.value( 4 ).toString().toDouble();
						ing.units.setId(ingredientQuery.value( 5 ).toInt());
						ing.units.setName(unescapeAndDecode( ingredientQuery.value( 6 ).toByteArray() ));
						ing.units.setPlural(unescapeAndDecode( ingredientQuery.value( 7 ).toByteArray() ));
						ing.units.setNameAbbrev(unescapeAndDecode( ingredientQuery.value( 8 ).toByteArray() ));
						ing.units.setPluralAbbrev(unescapeAndDecode( ingredientQuery.value( 9 ).toByteArray() ));
						ing.units.setType((Unit::Type)ingredientQuery.value( 10 ).toInt());

						//if we don't have both name and plural, use what we have as both
						if ( ing.units.name().isEmpty() )
							ing.units.setName(ing.units.plural());
						else if ( ing.units.plural().isEmpty() )
							ing.units.setPlural(ing.units.name());

						ing.groupID = ingredientQuery.value( 11 ).toInt();
						if ( ing.groupID != -1 ) {
							QSqlQuery toLoad( QString( "SELECT name FROM ingredient_groups WHERE id=%1" ).arg( ing.groupID ), *database);
							if ( toLoad.isActive() && toLoad.first() )
								ing.group = unescapeAndDecode( toLoad.value( 0 ).toByteArray() );
						}

						command = QString("SELECT pl.prep_method_id,p.name FROM prep_methods p, prep_method_list pl WHERE pl.ingredient_list_id=%1 AND p.id=pl.prep_method_id ORDER BY pl.order_index;").arg(ingredientQuery.value( 12 ).toInt());
						QSqlQuery ingPrepMethodsQuery( command, *database);
						if ( ingPrepMethodsQuery.isActive() ) {
							while ( ingPrepMethodsQuery.next() ) {
								ing.prepMethodList.append( Element( unescapeAndDecode(ingPrepMethodsQuery.value(1).toByteArray()),ingPrepMethodsQuery.value(0).toInt()) );
							}
						}
					}

					if ( ingredientQuery.value( 2 ).toInt() > 0 ) {
						//given the ordering, we can assume substitute_for is the id of the last
						//ingredient in the list
						//int substitute_for = ingredientQuery.value( 2 ).toInt();
						(*it).ingList.last().substitutes.append( ing );
					}
					else
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

			m_query->exec( command );
			if ( m_query->isActive() ) {
				while ( m_query->next() ) {
					Element el;
					el.id = m_query->value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
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

			m_query->exec( command );
			if ( m_query->isActive() ) {
				while ( m_query->next() ) {
					Element el;
					el.id = m_query->value( 0 ).toInt();
					el.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
					(*it).authorList.append( el );
				}
			}
		}
	}

	//Load the ratings
	if ( items & RecipeDB::Ratings ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];

			command = QString( "SELECT id,comment,rater FROM ratings WHERE recipe_id=%1 ORDER BY created DESC" ).arg( (*it).recipeID );
			QSqlQuery query( command, *database);
			if ( query.isActive() ) {
				while ( query.next() ) {
					Rating r;
					r.setId(query.value( 0 ).toInt());
					r.setComment(unescapeAndDecode( query.value( 1 ).toByteArray() ));
					r.setRater(unescapeAndDecode( query.value( 2 ).toByteArray() ));

					command = QString( "SELECT rc.id,rc.name,rl.stars FROM rating_criteria rc, rating_criterion_list rl WHERE rating_id=%1 AND rl.rating_criterion_id=rc.id" ).arg(r.id());
					QSqlQuery criterionQuery( command, *database);
					if ( criterionQuery.isActive() ) {
						while ( criterionQuery.next() ) {
							RatingCriteria rc;
							rc.setId( criterionQuery.value( 0 ).toInt() );
							rc.setName( unescapeAndDecode( criterionQuery.value( 1 ).toByteArray() ) );
							rc.setStars( criterionQuery.value( 2 ).toString().toDouble() );
							r.append( rc );
						}
					}

					(*it).ratingList.append( r );
				}
			}
		}
	}

	if ( items & RecipeDB::Properties ) {
		for ( RecipeList::iterator recipe_it = rlist->begin(); recipe_it != rlist->end(); ++recipe_it ) {
			RecipeList::iterator it = recipeIterators[ (*recipe_it).recipeID ];
			calculateProperties( *it, this );
		}
	}
}

void QSqlRecipeDB::loadIngredientGroups( ElementList *list )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredient_groups ORDER BY name;";
	m_query->exec( command );

	if ( m_query->isActive() ) {
		while ( m_query->next() ) {
			Element group;
			group.id = m_query->value( 0 ).toInt();
			group.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
			list->append( group );
		}
	}
}

void QSqlRecipeDB::loadIngredients( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM ingredients ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query->exec( command );

	if ( m_query->isActive() ) {
		while ( m_query->next() ) {
			Element ing;
			ing.id = m_query->value( 0 ).toInt();
			ing.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
			list->append( ing );
		}
	}
}

void QSqlRecipeDB::loadPrepMethods( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM prep_methods ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query->exec( command );

	if ( m_query->isActive() ) {
		while ( m_query->next() ) {
			Element prep_method;
			prep_method.id = m_query->value( 0 ).toInt();
			prep_method.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
			list->append( prep_method );
		}
	}
}

void QSqlRecipeDB::loadYieldTypes( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM yield_types ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	m_query->exec( command );

	if ( m_query->isActive() ) {
		while ( m_query->next() ) {
			Element el;
			el.id = m_query->value( 0 ).toInt();
			el.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::createNewPrepMethod( const QString &prepMethodName )
{
	QString command;
	QString real_name = prepMethodName.left( maxPrepMethodNameLength() );

	command = QString( "INSERT INTO prep_methods VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "prep_methods", "id" ) );
	QSqlQuery prepMethodToCreate( command, *database);

	emit prepMethodCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modPrepMethod( int prepMethodID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE prep_methods SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( prepMethodID );
	QSqlQuery prepMethodToCreate( command, *database);

	emit prepMethodRemoved( prepMethodID );
	emit prepMethodCreated( Element( newLabel, prepMethodID ) );
}

void QSqlRecipeDB::modProperty( int propertyID, const QString &newLabel, const QString &unit )
{
	QString command;

	if ( unit.isEmpty() )
		command = QString( "UPDATE ingredient_properties SET name='%1' WHERE id=%2" ).arg( escapeAndEncode( newLabel ) ).arg( propertyID );
	else
		command = QString( "UPDATE ingredient_properties SET name='%1',units='%2' WHERE id=%3" ).arg( escapeAndEncode( newLabel ) ).arg( escapeAndEncode( unit ) ).arg( propertyID );

	QSqlQuery createQuery( command, *database);

	emit propertyRemoved( propertyID );
	emit propertyCreated( propertyName( propertyID ) );
}

void QSqlRecipeDB::loadPossibleUnits( int ingredientID, UnitList *list )
{
	list->clear();

	QString command;

	command = QString( "SELECT u.id,u.name,u.plural,u.name_abbrev,u.plural_abbrev,u.type FROM unit_list ul, units u WHERE ul.ingredient_id=%1 AND ul.unit_id=u.id;" ).arg( ingredientID );

	QSqlQuery unitToLoad( command, *database);

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.setId(unitToLoad.value( 0 ).toInt());
			unit.setName(unescapeAndDecode( unitToLoad.value( 1 ).toByteArray() ));
			unit.setPlural(unescapeAndDecode( unitToLoad.value( 2 ).toByteArray() ));
			unit.setNameAbbrev(unescapeAndDecode( unitToLoad.value( 3 ).toByteArray() ));
			unit.setPluralAbbrev(unescapeAndDecode( unitToLoad.value( 4 ).toByteArray() ));
			unit.setType((Unit::Type) unitToLoad.value( 5 ).toInt());

			list->append( unit );
		}
	}


}

void QSqlRecipeDB::storePhoto( int recipeID, const QByteArray &data )
{
	QSqlQuery query( QString(), *database);

	query.prepare( "UPDATE recipes SET photo=?,ctime=ctime,atime=atime,mtime=mtime WHERE id=" + QString::number( recipeID ) );
	query.addBindValue( KCodecs::base64Encode( data ) );
	query.exec();
}

void QSqlRecipeDB::loadPhoto( int recipeID, QPixmap &photo )
{
	QString command = QString( "SELECT photo FROM recipes WHERE id=%1;" ).arg( recipeID );
	QSqlQuery query( command, *database);

	if ( query.isActive() && query.first() ) {
		QByteArray decodedPic;
		QPixmap pix;
		KCodecs::base64Decode( query.value( 0 ).toByteArray(), decodedPic );
		int len = decodedPic.size();

		if ( len > 0 ) {
			QByteArray picData;
			picData.resize( len );
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

	QSqlQuery query( command, *database);
	if ( query.isActive() && query.first() ) {
		recipe->ctime = QDateTime::fromString(query.value(0).toString(),Qt::ISODate);
		recipe->mtime = QDateTime::fromString(query.value(1).toString(),Qt::ISODate);
		recipe->atime = QDateTime::fromString(query.value(2).toString(),Qt::ISODate);
	}
}

void QSqlRecipeDB::saveRecipe( Recipe *recipe )
{
	// Check if it's a new recipe or it exists (supossedly) already.

	bool newRecipe;
	newRecipe = ( recipe->recipeID == -1 );
	// First check if the recipe ID is set, if so, update (not create)
	// Be carefull, first check if the recipe hasn't been deleted while changing.

	QSqlQuery recipeToSave( QString(), *database );

	QString command;

	QDateTime current_datetime = QDateTime::currentDateTime();
	QString current_timestamp = current_datetime.toString(Qt::ISODate);
	if ( newRecipe ) {
		command = QString( "INSERT INTO recipes VALUES ("+getNextInsertIDStr("recipes","id")+",'%1',%2,'%3','%4','%5',NULL,'%6','%7','%8','%9');" )  // Id is autoincremented
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount() )
		          .arg( recipe->yield.amountOffset() )
		          .arg( recipe->yield.typeId() )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( current_timestamp )
		          .arg( current_timestamp )
		          .arg( current_timestamp );
		recipe->mtime = recipe->ctime = recipe->atime = current_datetime;
	}
	else {
		command = QString( "UPDATE recipes SET title='%1',yield_amount='%2',yield_amount_offset='%3',yield_type_id='%4',instructions='%5',prep_time='%6',mtime='%8',ctime=ctime,atime=atime WHERE id=%7;" )
		          .arg( escapeAndEncode( recipe->title ) )
		          .arg( recipe->yield.amount() )
		          .arg( recipe->yield.amountOffset() )
		          .arg( recipe->yield.typeId() )
		          .arg( escapeAndEncode( recipe->instructions ) )
		          .arg( recipe->prepTime.toString( "hh:mm:ss" ) )
		          .arg( recipe->recipeID )
		          .arg( current_timestamp );
		recipe->mtime = current_datetime;
	}
	recipeToSave.exec( command );

	if ( !newRecipe ) {
		// Clean up yield_types which have no recipe that they belong to
		QStringList ids;
		command = QString( "SELECT DISTINCT(yield_type_id) FROM recipes" );
		recipeToSave.exec( command );
		if ( recipeToSave.isActive() ) {
			while ( recipeToSave.next() ) {
				if ( recipeToSave.value( 0 ).toInt() != -1 )
					ids << QString::number( recipeToSave.value( 0 ).toInt() );
			}
		}
		command = QString( "DELETE FROM yield_types WHERE id NOT IN ( %1 )" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
		recipeToSave.exec( command );
	}

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
		QBuffer buffer( &ba );
		buffer.open( QIODevice::WriteOnly );
		QImageWriter iio( &buffer, "JPEG" );
		iio.write( recipe->photo.toImage() );
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
	for ( IngredientList::const_iterator ing_it = recipe->ingList.constBegin(); ing_it != recipe->ingList.constEnd(); ++ing_it ) {
		order_index++;
		QString ing_list_id_str = getNextInsertIDStr("ingredient_list","id");
		command = QString( "INSERT INTO ingredient_list VALUES (%1,%2,%3,%4,%5,%6,%7,%8,NULL);" )
		          .arg( ing_list_id_str )
		          .arg( recipeID )
		          .arg( ( *ing_it ).ingredientID )
		          .arg( ( *ing_it ).amount )
		          .arg( ( *ing_it ).amount_offset )
		          .arg( ( *ing_it ).units.id() )
		          .arg( order_index )
		          .arg( ( *ing_it ).groupID );
		recipeToSave.exec( command );

		int ing_list_id = lastInsertID();
		int prep_order_index = 0;
		for ( ElementList::const_iterator prep_it = (*ing_it).prepMethodList.constBegin(); prep_it != (*ing_it).prepMethodList.constEnd(); ++prep_it ) {
			prep_order_index++;
			command = QString( "INSERT INTO prep_method_list VALUES (%1,%2,%3);" )
				.arg( ing_list_id )
				.arg( ( *prep_it ).id )
				.arg( prep_order_index );
			recipeToSave.exec( command );
		}

		for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.constBegin(); sub_it != (*ing_it).substitutes.constEnd(); ++sub_it ) {
			order_index++;
			QString ing_list_id_str = getNextInsertIDStr("ingredient_list","id");
			command = QString( "INSERT INTO ingredient_list VALUES (%1,%2,%3,%4,%5,%6,%7,%8,%9);" )
				.arg( ing_list_id_str )
				.arg( recipeID )
				.arg( ( *sub_it ).ingredientID )
				.arg( ( *sub_it ).amount )
				.arg( ( *sub_it ).amount_offset )
				.arg( ( *sub_it ).units.id() )
				.arg( order_index )
				.arg( ( *sub_it ).groupID )
				.arg( (*ing_it).ingredientID );
			recipeToSave.exec( command );

			int ing_list_id = lastInsertID();
			int prep_order_index = 0;
			for ( ElementList::const_iterator prep_it = (*sub_it).prepMethodList.constBegin(); prep_it != (*sub_it).prepMethodList.constEnd(); ++prep_it ) {
				prep_order_index++;
				command = QString( "INSERT INTO prep_method_list VALUES (%1,%2,%3);" )
					.arg( ing_list_id )
					.arg( ( *prep_it ).id )
					.arg( prep_order_index );
				recipeToSave.exec( command );
			}
		}
	}

	// Save the category list for the recipe (first delete, in case we are updating)
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1;" )
	          .arg( recipeID );
	recipeToSave.exec( command );

	ElementList::const_iterator cat_it = recipe->categoryList.constEnd(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--cat_it;
	for ( int i = 0; i < recipe->categoryList.count(); i++ ) {
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

	ElementList::const_iterator author_it = recipe->authorList.constEnd(); // Start from last, mysql seems to work in lifo format... so it's read first the latest inserted one (newest)
	--author_it;
	for ( int i = 0; i < recipe->authorList.count(); i++ ) {
		command = QString( "INSERT INTO author_list VALUES (%1,%2);" )
		          .arg( recipeID )
		          .arg( ( *author_it ).id );
		recipeToSave.exec( command );

		--author_it;
	}

	// Save the ratings (first delete criterion list if we are updating)
	command = QString( "SELECT id FROM ratings WHERE recipe_id=%1" ).arg(recipeID);
	recipeToSave.exec( command );
	if ( recipeToSave.isActive() ) {
		while ( recipeToSave.next() ) {

			command = QString("DELETE FROM rating_criterion_list WHERE rating_id=%1")
			  .arg(recipeToSave.value(0).toInt());
			database->exec(command);
		}
	}

	QStringList ids;

	for ( RatingList::iterator rating_it = recipe->ratingList.begin(); rating_it != recipe->ratingList.end(); ++rating_it ) {
		//double average = (*rating_it).average();
		if ( (*rating_it).id() == -1 ) //new rating
			command ="INSERT INTO ratings VALUES("+QString(getNextInsertIDStr("ratings","id"))+','+QString::number(recipeID)+",'"+QString(escapeAndEncode((*rating_it).comment()))+"','"+QString(escapeAndEncode((*rating_it).rater()))+/*"','"+QString::number(average)+*/"','"+current_timestamp+"')";
		else //existing rating
			command = "UPDATE ratings SET "
			  "comment='"+QString(escapeAndEncode((*rating_it).comment()))+"',"
			  "rater='"+QString(escapeAndEncode((*rating_it).rater()))+"',"
			  "created=created "
			  "WHERE id="+QString::number((*rating_it).id());

		recipeToSave.exec( command );

		if ( (*rating_it).id() == -1 )
			(*rating_it).setId(lastInsertID());

		foreach(RatingCriteria rc, (*rating_it).ratingCriterias()) {
			command = QString( "INSERT INTO rating_criterion_list VALUES("+QString::number((*rating_it).id())+','+QString::number(rc.id())+','+QString::number(rc.stars())+')' );
			recipeToSave.exec( command );
		}

		ids << QString::number((*rating_it).id());
	}

	// only delete those ratings that don't exist anymore
	command = QString( "DELETE FROM ratings WHERE recipe_id=%1 AND id NOT IN( %2 )" )
	          .arg( recipeID ).arg( ids.join(",") );
	recipeToSave.exec( command );

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
		QSqlQuery subcategories( QString("SELECT id FROM categories WHERE parent_id='%1'").arg(categoryID), *database);
		if ( subcategories.isActive() ) {
			while ( subcategories.next() ) {
				loadRecipeList(list,subcategories.value( 0 ).toInt(),true);
			}
		}
	}

	QSqlQuery recipeToLoad( command, *database);

	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			Element recipe;
			recipe.id = recipeToLoad.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( recipeToLoad.value( 1 ).toByteArray() );
			list->append( recipe );
		}
	}
}


void QSqlRecipeDB::loadUncategorizedRecipes( ElementList *list )
{
	list->clear();

	QString command =
	"SELECT cl.recipe_id,r.title FROM "
		"category_list cl join recipes r "
		"on cl.recipe_id = r.id "
	"GROUP BY cl.recipe_id,r.title HAVING COUNT(cl.recipe_id)=1 "
	"ORDER BY title";

	m_query->exec( command );
	if ( m_query->isActive() ) {
		while ( m_query->next() ) {
			Element recipe;
			recipe.id = m_query->value( 0 ).toInt();
			recipe.name = unescapeAndDecode( m_query->value( 1 ).toByteArray() );
			list->append( recipe );
		}
	}
}



void QSqlRecipeDB::removeRecipe( int id )
{
	emit recipeRemoved( id );

	QString command;

	command = QString( "DELETE FROM recipes WHERE id=%1;" ).arg( id );
	QSqlQuery recipeToRemove( command, *database);
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

	// Clean up yield_types which have no recipe that they belong to
	ids.clear();
	command = QString( "SELECT DISTINCT(yield_type_id) FROM recipes" );
	recipeToRemove.exec( command );
	if ( recipeToRemove.isActive() ) {
		while ( recipeToRemove.next() ) {
			if ( recipeToRemove.value( 0 ).toInt() != -1 )
				ids << QString::number( recipeToRemove.value( 0 ).toInt() );
		}
	}
	command = QString( "DELETE FROM yield_types WHERE id NOT IN ( %1 );" ).arg( ( ids.count() == 0 ) ? "-1" : ids.join( "," ) );
	recipeToRemove.exec( command );
}

void QSqlRecipeDB::removeRecipeFromCategory( int recipeID, int categoryID )
{
	QString command;
	command = QString( "DELETE FROM category_list WHERE recipe_id=%1 AND category_id=%2;" ).arg( recipeID ).arg( categoryID );
	QSqlQuery recipeToRemove( command, *database);

	emit recipeRemoved( recipeID, categoryID );
}

void QSqlRecipeDB::categorizeRecipe( int recipeID, const ElementList &categoryList )
{
	QString command;

	//emit recipeRemoved( recipeID, -1 );

	for ( ElementList::const_iterator it = categoryList.constBegin(); it != categoryList.constEnd(); ++it ) {
		command = QString( "INSERT INTO category_list VALUES(%1,%2)" ).arg( recipeID ).arg( (*it).id );
		database->exec( command );
	}

	emit recipeModified( Element(recipeTitle(recipeID),recipeID), categoryList );
}

void QSqlRecipeDB::createNewIngGroup( const QString &name )
{
	QString command;
	QString real_name = name.left( maxIngGroupNameLength() );

	command = QString( "INSERT INTO ingredient_groups VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredient_groups", "id" ) );
	QSqlQuery query( command, *database);

	emit ingGroupCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::createNewIngredient( const QString &ingredientName )
{
	QString command;
	QString real_name = ingredientName.left( maxIngredientNameLength() );

	command = QString( "INSERT INTO ingredients VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "ingredients", "id" ) );
	QSqlQuery ingredientToCreate( command, *database);

	emit ingredientCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::createNewRating( const QString &rating )
{
	QString command;
	QString real_name = rating/*.left( maxIngredientNameLength() )*/;

	command = QString( "INSERT INTO rating_criteria VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "rating_criteria", "id" ) );
	QSqlQuery toCreate( command, *database);

	emit ratingCriteriaCreated( Element( real_name, lastInsertID() ) );
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
	QSqlQuery ingredientToCreate( command, *database);

	emit ingGroupRemoved( groupID );
	emit ingGroupCreated( Element( newLabel, groupID ) );
}

void QSqlRecipeDB::modIngredient( int ingredientID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE ingredients SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( ingredientID );
	QSqlQuery ingredientToCreate( command, *database);

	emit ingredientRemoved( ingredientID );
	emit ingredientCreated( Element( newLabel, ingredientID ) );
}

void QSqlRecipeDB::addUnitToIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "INSERT INTO unit_list VALUES(%1,%2);" ).arg( ingredientID ).arg( unitID );
	QSqlQuery ingredientToCreate( command, *database);
}

void QSqlRecipeDB::loadUnits( UnitList *list, Unit::Type type, int limit, int offset )
{
	list->clear();

	QString command;

	command = "SELECT id,name,name_abbrev,plural,plural_abbrev,type FROM units "
	  +((type==Unit::All)?"":"WHERE type="+QString::number(type))
	  +" ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));

	QSqlQuery unitToLoad( command, *database);

	if ( unitToLoad.isActive() ) {
		while ( unitToLoad.next() ) {
			Unit unit;
			unit.setId(unitToLoad.value( 0 ).toInt());
			unit.setName(unescapeAndDecode( unitToLoad.value( 1 ).toByteArray() ));
			unit.setNameAbbrev(unescapeAndDecode( unitToLoad.value( 2 ).toByteArray() ));
			unit.setPlural(unescapeAndDecode( unitToLoad.value( 3 ).toByteArray() ));
			unit.setPluralAbbrev(unescapeAndDecode( unitToLoad.value( 4 ).toByteArray() ));
			unit.setType((Unit::Type)unitToLoad.value( 5 ).toInt());
			list->append( unit );
		}
	}
}

void QSqlRecipeDB::removeUnitFromIngredient( int ingredientID, int unitID )
{
	QString command;

	command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, *database);

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

void QSqlRecipeDB::removeIngredientGroup( int groupID )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredient_groups WHERE id=%1" ).arg( groupID );
	QSqlQuery toDelete( command, *database);

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
	QSqlQuery ingredientToDelete( command, *database);

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

void QSqlRecipeDB::removeIngredientWeight( int id )
{
	QString command;

	// First remove the ingredient

	command = QString( "DELETE FROM ingredient_weights WHERE id=%1" ).arg( id );
	QSqlQuery toDelete( command, *database);
}

void QSqlRecipeDB::addIngredientWeight( const Weight &w )
{
	QString command;
	if ( w.id() != -1 ) {
		command = QString( "UPDATE ingredient_weights SET ingredient_id=%1,amount=%2,unit_id=%3,weight=%4,weight_unit_id=%5,prep_method_id=%7 WHERE id=%6" )
		  .arg(w.ingredientId())
		  .arg(w.perAmount())
		  .arg(w.perAmountUnitId())
		  .arg(w.weight())
		  .arg(w.weightUnitId())
		  .arg(w.id())
		  .arg(w.prepMethodId());
	}
	else {
		command = QString( "INSERT INTO ingredient_weights VALUES(%6,%1,%2,%3,%4,%5,%7)" )
		  .arg(w.ingredientId())
		  .arg(w.perAmount())
		  .arg(w.perAmountUnitId())
		  .arg(w.weight())
		  .arg(w.weightUnitId())
		  .arg(getNextInsertIDStr( "ingredient_weights", "id" ))
		  .arg(w.prepMethodId());
	}
	QSqlQuery query( command, *database);
}

void QSqlRecipeDB::addProperty( const QString &name, const QString &units )
{
	QString command;
	QString real_name = name.left( maxPropertyNameLength() );

	command = QString( "INSERT INTO ingredient_properties VALUES(%3,'%1','%2');" )
	          .arg( escapeAndEncode( real_name ) )
	          .arg( escapeAndEncode( units ) )
	          .arg( getNextInsertIDStr( "ingredient_properties", "id" ) );
	QSqlQuery propertyToAdd( command, *database);

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
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,u.type,ii.amount,ii.ingredient_id  FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.ingredient_id=%1 AND ii.property_id=ip.id AND ii.per_units=u.id;" ).arg( ingredientID );
	}
	else if ( ingredientID == -1 )  // Load the properties of all the ingredients
	{
		usePerUnit = true;
		command = QString( "SELECT ip.id,ip.name,ip.units,ii.per_units,u.name,u.type,ii.amount,ii.ingredient_id FROM ingredient_properties ip, ingredient_info ii, units u WHERE ii.property_id=ip.id AND ii.per_units=u.id;" );
	}
	else // Load the whole property list (just the list of possible properties, not the ingredient properties)
	{
		usePerUnit = false;
		command = QString( "SELECT  id,name,units FROM ingredient_properties;" );
	}

	QSqlQuery propertiesToLoad ( command, *database);
	// Load the results into the list
	if ( propertiesToLoad.isActive() ) {
		while ( propertiesToLoad.next() ) {
			IngredientProperty prop;
			prop.id = propertiesToLoad.value( 0 ).toInt();
			prop.name = unescapeAndDecode( propertiesToLoad.value( 1 ).toByteArray() );
			prop.units = unescapeAndDecode( propertiesToLoad.value( 2 ).toByteArray() );
			if ( usePerUnit ) {
				prop.perUnit.setId(propertiesToLoad.value( 3 ).toInt());
				prop.perUnit.setName(unescapeAndDecode( propertiesToLoad.value( 4 ).toByteArray() ));
				prop.perUnit.setType((Unit::Type)propertiesToLoad.value( 5 ).toInt());
			}

			if ( ingredientID >= -1 )
				prop.amount = propertiesToLoad.value( 6 ).toString().toDouble();
			else
				prop.amount = -1; // Property is generic, not attached to an ingredient

			if ( ingredientID >= -1 )
				prop.ingredientID = propertiesToLoad.value( 7 ).toInt();

			list->append( prop );
		}
	}
}

void QSqlRecipeDB::changePropertyAmountToIngredient( int ingredientID, int propertyID, double amount, int per_units )
{
	QString command;
	command = QString( "UPDATE ingredient_info SET amount=%1 WHERE ingredient_id=%2 AND property_id=%3 AND per_units=%4;" ).arg( amount ).arg( ingredientID ).arg( propertyID ).arg( per_units );
	QSqlQuery infoToChange( command, *database);
}

void QSqlRecipeDB::addPropertyToIngredient( int ingredientID, int propertyID, double amount, int perUnitsID )
{
	QString command;

	command = QString( "INSERT INTO ingredient_info VALUES(%1,%2,%3,%4);" ).arg( ingredientID ).arg( propertyID ).arg( amount ).arg( perUnitsID );
	QSqlQuery propertyToAdd( command, *database);
}


void QSqlRecipeDB::removePropertyFromIngredient( int ingredientID, int propertyID, int perUnitID )
{
	QString command;
	// remove property from ingredient info. Note that there could be duplicates with different units (per_units). Remove just the one especified.
	command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitID );
	QSqlQuery propertyToRemove( command, *database);
}

void QSqlRecipeDB::removeProperty( int propertyID )
{
	QString command;

	// Remove property from the ingredient_properties
	command = QString( "DELETE FROM ingredient_properties WHERE id=%1;" ).arg( propertyID );
	QSqlQuery propertyToRemove( command, *database);

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
	QSqlQuery unitToRemove( command, *database);

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

	// Remove associated ingredient weights
	command = QString( "DELETE FROM ingredient_weights WHERE unit_id=%1" ).arg( unitID );
	unitToRemove.exec( command );

	emit unitRemoved( unitID );
}

void QSqlRecipeDB::removePrepMethod( int prepMethodID )
{
	QString command;
	// Remove the prep method first
	command = QString( "DELETE FROM prep_methods WHERE id=%1;" ).arg( prepMethodID );
	QSqlQuery prepMethodToRemove( command, *database);

	// Remove any recipe using that prep method in the ingredient list (user must have been warned before calling this function!)

	command = QString( "SELECT DISTINCT r.id FROM recipes r,ingredient_list il, prep_method_list pl WHERE r.id=il.recipe_id AND pl.ingredient_list_id=il.id AND pl.prep_method_id=%1;" ).arg( prepMethodID );
	prepMethodToRemove.exec( command );
	if ( prepMethodToRemove.isActive() ) {
		while ( prepMethodToRemove.next() ) {
			emit recipeRemoved( prepMethodToRemove.value( 0 ).toInt() );
			database->exec( QString( "DELETE FROM recipes WHERE id=%1;" ).arg( prepMethodToRemove.value( 0 ).toInt() ) );
		}
	}

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


void QSqlRecipeDB::createNewUnit( const Unit &unit )
{
	QString real_name = unit.name().left( maxUnitNameLength() ).trimmed();
	QString real_plural = unit.plural().left( maxUnitNameLength() ).trimmed();
	QString real_name_abbrev = unit.nameAbbrev().left( maxUnitNameLength() ).trimmed();
	QString real_plural_abbrev = unit.pluralAbbrev().left( maxUnitNameLength() ).trimmed();

	Unit new_unit( real_name, real_plural );
	new_unit.setNameAbbrev(real_name_abbrev);
	new_unit.setPluralAbbrev(real_plural_abbrev);
	new_unit.setType(unit.type());

	if ( real_name.isEmpty() )
		real_name = real_plural;
	else if ( real_plural.isEmpty() )
		real_plural = real_name;

	if ( real_name_abbrev.isEmpty() )
		real_name_abbrev = "NULL";
	else
		real_name_abbrev = '\''+escapeAndEncode(real_name_abbrev)+'\'';
	if ( real_plural_abbrev.isEmpty() )
		real_plural_abbrev = "NULL";
	else
		real_plural_abbrev = '\''+escapeAndEncode(real_plural_abbrev)+'\'';


	QString command = "INSERT INTO units VALUES(" + getNextInsertIDStr( "units", "id" )
	   + ",'" + escapeAndEncode( real_name )
	   + "'," + real_name_abbrev
	   + ",'" + escapeAndEncode( real_plural )
	   + "'," + real_plural_abbrev
	   + ',' + QString::number(unit.type())
	   + ");";

	QSqlQuery unitToCreate( command, *database);

	new_unit.setId(lastInsertID());
	emit unitCreated( new_unit );
}


void QSqlRecipeDB::modUnit( const Unit &unit )
{
	QSqlQuery unitQuery( QString(), *database);

	QString real_name = unit.name().left( maxUnitNameLength() ).trimmed();
	QString real_plural = unit.plural().left( maxUnitNameLength() ).trimmed();
	QString real_name_abbrev = unit.nameAbbrev().left( maxUnitNameLength() ).trimmed();
	QString real_plural_abbrev = unit.pluralAbbrev().left( maxUnitNameLength() ).trimmed();

	Unit newUnit( real_name, real_plural, unit.id() );
	newUnit.setType(unit.type());
	newUnit.setNameAbbrev(real_name_abbrev);
	newUnit.setPluralAbbrev(real_plural_abbrev);

	if ( real_name_abbrev.isEmpty() )
		real_name_abbrev = "NULL";
	else
		real_name_abbrev = '\''+escapeAndEncode(real_name_abbrev)+'\'';
	if ( real_plural_abbrev.isEmpty() )
		real_plural_abbrev = "NULL";
	else
		real_plural_abbrev = '\''+escapeAndEncode(real_plural_abbrev)+'\'';

	QString command = QString("UPDATE units SET name='%1',name_abbrev=%2,plural='%3',plural_abbrev=%4,type=%6 WHERE id='%5'")
	  .arg(escapeAndEncode(real_name))
	  .arg(real_name_abbrev)
	  .arg(escapeAndEncode(real_plural))
	  .arg(real_plural_abbrev)
	  .arg(unit.id())
	  .arg(unit.type());
	unitQuery.exec( command );

	emit unitRemoved( unit.id() );
	emit unitCreated( newUnit );
}

void QSqlRecipeDB::findUseOfIngGroupInRecipes( ElementList *results, int groupID )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.group_id=%1 ORDER BY r.title" ).arg( groupID );
	QSqlQuery query( command, *database);

	// Populate data
	if ( query.isActive() ) {
		while ( query.next() ) {
			Element recipe;
			recipe.id = query.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( query.value( 1 ).toByteArray() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::findUseOfCategoryInRecipes( ElementList *results, int catID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,category_list cl WHERE r.id=cl.recipe_id AND cl.category_id=%1 ORDER BY r.title" ).arg( catID );
	QSqlQuery query( command, *database);

	// Populate data
	if ( query.isActive() ) {
		while ( query.next() ) {
			Element recipe;
			recipe.id = query.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( query.value( 1 ).toByteArray() );
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

void QSqlRecipeDB::findUseOfAuthorInRecipes( ElementList *results, int authorID )
{
	QString command = QString( "SELECT r.id,r.title FROM recipes r,author_list al WHERE r.id=al.recipe_id AND al.author_id=%1 ORDER BY r.title" ).arg( authorID );
	QSqlQuery query( command, *database);

	// Populate data
	if ( query.isActive() ) {
		while ( query.next() ) {
			Element recipe;
			recipe.id = query.value( 0 ).toInt();
			recipe.name = unescapeAndDecode( query.value( 1 ).toByteArray() );
			results->append( recipe );
		}
	}
}

void QSqlRecipeDB::loadUnitRatios( UnitRatioList *ratioList, Unit::Type type )
{
	ratioList->clear();

	QString command;
	if ( type == Unit::All )
		command = "SELECT unit1_id,unit2_id,ratio FROM units_conversion";
	else
		command = "SELECT unit1_id,unit2_id,ratio FROM units_conversion,units unit1,units unit2 WHERE unit1_id=unit1.id AND unit1.type="+QString::number(type)+" AND unit2_id=unit2.id AND unit2.type="+QString::number(type);
	QSqlQuery ratiosToLoad( command, *database);

	if ( ratiosToLoad.isActive() ) {
		while ( ratiosToLoad.next() ) {
			UnitRatio ratio(ratiosToLoad.value( 0 ).toInt(),
			                ratiosToLoad.value( 1 ).toInt(),
			                ratiosToLoad.value( 2 ).toString().toDouble() );
			ratioList->add( ratio );
		}
	}
}

void QSqlRecipeDB::saveUnitRatio( const UnitRatio *ratio )
{
	QString command;

	// Check if it's a new ratio or it exists already.
	command = QString( "SELECT * FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2" ).arg( ratio->unitId1() ).arg( ratio->unitId2() ); // Find ratio between units

	QSqlQuery ratioFound( command, *database); // Find the entries
	bool newRatio = ( ratioFound.size() == 0 );

	if ( newRatio )
		command = QString( "INSERT INTO units_conversion VALUES(%1,%2,%3);" ).arg( ratio->unitId1() ).arg( ratio->unitId2() ).arg( ratio->ratio() );
	else
		command = QString( "UPDATE units_conversion SET ratio=%3 WHERE unit1_id=%1 AND unit2_id=%2" ).arg( ratio->unitId1() ).arg( ratio->unitId2() ).arg( ratio->ratio() );

	ratioFound.exec( command ); // Enter the new ratio
}

void QSqlRecipeDB::removeUnitRatio( int unitID1, int unitID2 )
{
	database->exec(QString( "DELETE FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2" ).arg( unitID1 ).arg( unitID2 ));
}

double QSqlRecipeDB::unitRatio( int unitID1, int unitID2 )
{

	if ( unitID1 == unitID2 )
		return ( 1.0 );
	QString command;

	command = QString( "SELECT ratio FROM units_conversion WHERE unit1_id=%1 AND unit2_id=%2;" ).arg( unitID1 ).arg( unitID2 );
	QSqlQuery ratioToLoad( command, *database);

	if ( ratioToLoad.isActive() && ratioToLoad.next() )
		return ( ratioToLoad.value( 0 ).toString().toDouble() );
	else
		return ( -1 );
}

double QSqlRecipeDB::ingredientWeight( const Ingredient &ing, bool *wasApproximated )
{
	QString command = QString( "SELECT amount,weight,prep_method_id,unit_id FROM ingredient_weights WHERE ingredient_id=%1 AND (unit_id=%2 OR weight_unit_id=%3)" )
	   .arg( ing.ingredientID )
	   .arg( ing.units.id() ).arg( ing.units.id() );

	QSqlQuery query( command, *database);

	if ( query.isActive() ) {
		//store the amount for the entry with no prep method.  If no other suitable entry is found, we'll guesstimate
		//the weight using this entry
		double convertedAmount = -1;
		while ( query.next() ) {
			int prepMethodID = query.value( 2 ).toInt();

			if ( ing.prepMethodList.containsId( prepMethodID ) ) {
				if ( wasApproximated ) *wasApproximated = false;
				double amount = query.value( 0 ).toString().toDouble();

				//'per_amount' -> 'weight' conversion
				if ( query.value( 3 ).toInt() == ing.units.id() )
					convertedAmount = query.value( 1 ).toString().toDouble() * ing.amount / amount;
				//'weight' -> 'per_amount' conversion
				else
					convertedAmount = amount * ing.amount / query.value( 1 ).toString().toDouble();

				return convertedAmount;
			}
			if ( prepMethodID == -1 ) {
				//'per_amount' -> 'weight' conversion
				if ( query.value( 3 ).toInt() == ing.units.id() )
					convertedAmount = query.value( 1 ).toString().toDouble() * ing.amount / query.value( 0 ).toString().toDouble();
				//'weight' -> 'per_amount' conversion
				else
					convertedAmount = query.value( 0 ).toString().toDouble() * ing.amount / query.value( 1 ).toString().toDouble();
			}
		}
		//no matching prep method found, use entry without a prep method if there was one
		if ( convertedAmount > 0 ) {
			if ( wasApproximated ) *wasApproximated = true;
			kDebug()<<"Prep method given, but no weight entry found that uses that prep method.  I'm fudging the weight with an entry without a prep method.";

			return convertedAmount;
		}
	}
	return -1;
}

WeightList QSqlRecipeDB::ingredientWeightUnits( int ingID )
{
	WeightList list;

	QString command = QString( "SELECT id,amount,unit_id,weight,weight_unit_id,prep_method_id FROM ingredient_weights WHERE ingredient_id=%1" ).arg( ingID );
	QSqlQuery query( command, *database);
	if ( query.isActive() ) {
		while ( query.next() ) {
			Weight w;
			w.setId(query.value(0).toInt());
			w.setPerAmount(query.value(1).toString().toDouble());
			w.setPerAmountUnitId(query.value(2).toInt());
			w.setWeight(query.value(3).toString().toDouble());
			w.setWeightUnitId(query.value(4).toInt());
			w.setPrepMethodId(query.value(5).toInt());
			w.setIngredientId(ingID);
			list.append(w);
		}
	}

	return list;
}

//Finds data dependant on this Ingredient/Unit combination
void QSqlRecipeDB::findIngredientUnitDependancies( int ingredientID, int unitID, ElementList *recipes, ElementList *ingredientInfo )
{

	// Recipes using that combination

	QString command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 AND il.unit_id=%2 ORDER BY r.title" ).arg( ingredientID ).arg( unitID );
	QSqlQuery unitToRemove( command, *database);
	loadElementList( recipes, &unitToRemove );
	// Ingredient info using that combination
	command = QString( "SELECT i.name,ip.name,ip.units,u.name FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.ingredient_id=%1 AND ii.per_units=%2 AND ii.property_id=ip.id AND ii.per_units=u.id ORDER BY i.name" ).arg( ingredientID ).arg( unitID );

	unitToRemove.exec( command );
	loadPropertyElementList( ingredientInfo, &unitToRemove );
}

void QSqlRecipeDB::findIngredientDependancies( int ingredientID, ElementList *recipes )
{
	QString command = QString( "SELECT DISTINCT r.id,r.title FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.ingredient_id=%1 ORDER BY r.title" ).arg( ingredientID );

	QSqlQuery ingredientToRemove( command, *database );
	loadElementList( recipes, &ingredientToRemove );
}



//Finds data dependant on the removal of this Unit
void QSqlRecipeDB::findUnitDependancies( int unitID, ElementList *properties, ElementList *recipes, ElementList *weights )
{

	// Ingredient-Info (ingredient->property) using this Unit

	QString command = QString( "SELECT i.name,ip.name,ip.units,u.name  FROM ingredients i, ingredient_info ii, ingredient_properties ip, units u WHERE i.id=ii.ingredient_id AND ii.per_units=%1 AND ii.property_id=ip.id  AND ii.per_units=u.id ORDER BY i.name" ).arg( unitID );
	QSqlQuery unitToRemove( command, *database);
	loadPropertyElementList( properties, &unitToRemove );

	// Recipes using this Unit
	command = QString( "SELECT DISTINCT r.id,r.title  FROM recipes r,ingredient_list il WHERE r.id=il.recipe_id AND il.unit_id=%1 ORDER BY r.title" ).arg( unitID ); // Without "DISTINCT" we get duplicates since ingredient_list has no unique recipe_id's
	unitToRemove.exec( command );
	loadElementList( recipes, &unitToRemove );

	// Weights using this unit
	command = QString( "SELECT i.name,weight_u.name,per_u.name,w.prep_method_id FROM ingredients i,ingredient_weights w,units weight_u,units per_u WHERE i.id=w.ingredient_id AND w.unit_id=per_u.id AND w.weight_unit_id=weight_u.id AND (weight_u.id=%1 OR per_u.id=%2) ORDER BY i.name" )
	  .arg( unitID )
	  .arg( unitID );
	unitToRemove.exec( command );
	if ( unitToRemove.isActive() ) {
		while ( unitToRemove.next() ) {
			Element el;

			QString ingName = unescapeAndDecode( unitToRemove.value( 0 ).toByteArray() );
			QString weightUnit = unescapeAndDecode( unitToRemove.value( 1 ).toByteArray() );
			QString perUnit = unescapeAndDecode( unitToRemove.value( 2 ).toByteArray() );

			int prepID = unitToRemove.value( 3 ).toInt();
			QString prep;
			if ( prepID != -1 ) {
				command = QString( "SELECT p.name FROM prep_methods p, ingredient_weights w WHERE p.id = w.prep_method_id AND w.prep_method_id=%1" )
					.arg( prepID );
				QSqlQuery query( command, *database);
				if ( query.isActive() && query.first() )
					prep = unescapeAndDecode( query.value( 0 ).toByteArray() );
			}

			el.name = i18n("In ingredient '%1': weight [%2/%3%4]", ingName , weightUnit , perUnit, (prepID == -1)?QString():"; "+prep );
			weights->append( el );
		}
	}

}

void QSqlRecipeDB::findPrepMethodDependancies( int prepMethodID, ElementList *recipes )
{
	//get just the ids first so that we can use DISTINCT
	QString command = QString( "SELECT DISTINCT r.id FROM recipes r,ingredient_list il, prep_method_list pl WHERE r.id=il.recipe_id AND pl.ingredient_list_id=il.id AND pl.prep_method_id=%1;" ).arg( prepMethodID );

	QStringList ids;
	QSqlQuery query( command, *database);
	if ( query.isActive() ) {
		while ( query.next() ) {
			ids << QString::number(query.value( 0 ).toInt());
		}
	}

	//now get the titles of the ids
	command = QString( "SELECT r.id, r.title FROM recipes r WHERE r.id IN ("+ids.join(",")+')' );
	QSqlQuery prepMethodToRemove( command, *database);
	loadElementList( recipes, &prepMethodToRemove );
}


void QSqlRecipeDB::loadElementList( ElementList *elList, QSqlQuery *query )
{
	if ( query->isActive() ) {
		while ( query->next() ) {
			Element el;
			el.id = query->value( 0 ).toInt();
			el.name = unescapeAndDecode( query->value( 1 ).toByteArray() );
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
			QString ingName = unescapeAndDecode( query->value( 0 ).toByteArray() );
			QString propName = unescapeAndDecode( query->value( 1 ).toByteArray() );
			QString propUnits = unescapeAndDecode( query->value( 2 ).toByteArray() );
			QString propPerUnits = unescapeAndDecode( query->value( 3 ).toByteArray() );

			el.name = i18n("In ingredient '%1': property \"%2\" [%3/%4]" , ingName, propName, propUnits, propPerUnits );
			elList->append( el );
		}
	}
}


//The string going into the database is utf8 text interpreted as latin1
QString QSqlRecipeDB::escapeAndEncode( const QString &s ) const
{
	QString s_escaped = s;

	s_escaped.replace ( '\\', "\\\\" );
	s_escaped.replace ( '\'', "\\'" );
	s_escaped.replace ( ';', "\";@" ); // Small trick for only for parsing later on

	return QString::fromLatin1( s_escaped.toUtf8() );
}

//The string coming out of the database is utf8 text, interpreted as though latin1.  Calling fromUtf8() on this gives us back the original utf8.
QString QSqlRecipeDB::unescapeAndDecode( const QByteArray &s ) const
{
	return QString::fromUtf8( s ).replace( "\";@", ";" ); // Use unicode encoding
}

bool QSqlRecipeDB::ingredientContainsUnit( int ingredientID, int unitID )
{
	QString command = QString( "SELECT *  FROM unit_list WHERE ingredient_id= %1 AND unit_id=%2;" ).arg( ingredientID ).arg( unitID );
	QSqlQuery recipeToLoad( command, *database);
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

bool QSqlRecipeDB::ingredientContainsProperty( int ingredientID, int propertyID, int perUnitsID )
{
	QString command = QString( "SELECT *  FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2 AND per_units=%3;" ).arg( ingredientID ).arg( propertyID ).arg( perUnitsID );
	QSqlQuery recipeToLoad( command, *database);
	if ( recipeToLoad.isActive() ) {
		return ( recipeToLoad.size() > 0 );
	}
	return false;
}

QString QSqlRecipeDB::categoryName( int ID )
{
	QString command = QString( "SELECT name FROM categories WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() && toLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode( toLoad.value( 0 ).toByteArray() ) );

	return ( QString() );
}

QString QSqlRecipeDB::ingredientName( int ID )
{
	QString command = QString( "SELECT name FROM ingredients WHERE id=%1" ).arg( ID );
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() && toLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode( toLoad.value( 0 ).toByteArray() ) );

	return ( QString() );
}

QString QSqlRecipeDB::prepMethodName( int ID )
{
	QString command = QString( "SELECT name FROM prep_methods WHERE id=%1" ).arg( ID );
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() && toLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode( toLoad.value( 0 ).toByteArray() ) );

	return ( QString() );
}

IngredientProperty QSqlRecipeDB::propertyName( int ID )
{
	QString command = QString( "SELECT name,units FROM ingredient_properties WHERE id=%1;" ).arg( ID );
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		return ( IngredientProperty( unescapeAndDecode( toLoad.value( 0 ).toByteArray() ), unescapeAndDecode( toLoad.value( 1 ).toByteArray() ), ID ) );
	}

	return ( IngredientProperty( QString(), QString() ) );
}

Unit QSqlRecipeDB::unitName( int ID )
{
	QString command = QString( "SELECT name,plural,name_abbrev,plural_abbrev,type FROM units WHERE id=%1" ).arg( ID );
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() && toLoad.next() ) { // Go to the first record (there should be only one anyway.
		Unit unit( unescapeAndDecode( toLoad.value( 0 ).toByteArray() ), unescapeAndDecode( toLoad.value( 1 ).toByteArray() ) );

		//if we don't have both name and plural, use what we have as both
		if ( unit.name().isEmpty() )
			unit.setName(unit.plural());
		else if ( unit.plural().isEmpty() )
			unit.setPlural(unit.name());

		unit.setNameAbbrev(unescapeAndDecode( toLoad.value( 2 ).toByteArray() ));
		unit.setPluralAbbrev(unescapeAndDecode( toLoad.value( 3 ).toByteArray() ));
		unit.setType((Unit::Type) toLoad.value( 4 ).toInt());
		unit.setId(ID);

		return unit;
	}

	return Unit();
}

int QSqlRecipeDB::getCount( const QString &table_name )
{
	m_command = "SELECT COUNT(1) FROM "+table_name;
	QSqlQuery count( m_command, *database);
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

int QSqlRecipeDB::categoryTopLevelCount()
{
	m_command = "SELECT COUNT(1) FROM categories WHERE parent_id='-1'";
	QSqlQuery count( m_command, *database);
	if ( count.isActive() && count.next() ) { // Go to the first record (there should be only one anyway.
		return count.value( 0 ).toInt();
	}

	return -1;
}

bool QSqlRecipeDB::checkIntegrity( void )
{
	kDebug();
	// Check existence of the necessary tables (the database may be created, but empty)
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredient_weights" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "db_info" << "prep_methods" << "ingredient_groups" << "yield_types" << "prep_method_list" << "ratings" << "rating_criteria" << "rating_criterion_list";

	QStringList existingTableList = database->tables();
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		bool found = false;

		for ( QStringList::Iterator ex_it = existingTableList.begin(); ( ( ex_it != existingTableList.end() ) && ( !found ) ); ++ex_it ) {
			found = ( *ex_it == *it );
		}

		if ( !found ) {
			kDebug() << "Recreating missing table: " << *it << "\n";
			createTable( *it );
		}
	}

	QStringList newTableList = database->tables();
	if ( newTableList.isEmpty() )
		return false;


	// Check for older versions, and port

	kDebug() << "Checking database version...\n";
	float version = databaseVersion();
	kDebug() << "version found... " << version << " \n";
	kDebug() << "latest version... " << latestDBVersion() ;
	if ( int( qRound( databaseVersion() * 1e5 ) ) < int( qRound( latestDBVersion() * 1e5 ) ) ) { //correct for float's imprecision
		switch ( KMessageBox::questionYesNo( 0,
		i18nc("@info", "<p>The database was created with a previous version of Krecipes. "
		"Would you like Krecipes to update this database to work with this version of "
		"Krecipes?  Depending on the number of recipes and amount of data, this could "
		"take some time.</p>"
		"<warning>"
		"<p>After updating, this database will no longer be compatible with "
		"previous versions of Krecipes.</p>"
		"<p>Cancelling this operation may result in corrupting the database.</p>"
		"</warning>" ) ) ) {
		case KMessageBox::Yes:
			emit progressBegin(0,QString(),i18n("Porting database structure..."),50);
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
	if (s.isEmpty())
		sl = QStringList();
	else
		sl = s.split( QRegExp( ";{1}(?!@)" ), QString::SkipEmptyParts);
}

float QSqlRecipeDB::databaseVersion( void )
{
	kDebug();
	QString command = "SELECT ver FROM db_info";
	QSqlQuery dbVersion( command, *database);
	kDebug()<<"  dbVersion.isActive() :"<< dbVersion.isActive()<<" database :"<<database;
	kDebug()<<" dbVersion.isSelect() :"<<dbVersion.isSelect();
	if ( dbVersion.isActive() && dbVersion.isSelect() && dbVersion.next() ) {
		kDebug()<<" dbVersion.value( 0 ).toString().toDouble( :"<<dbVersion.value( 0 ).toString().toDouble();
		return ( dbVersion.value( 0 ).toString().toDouble() ); // There should be only one (or none for old DB) element, so go to first
	}
	else {
		kDebug()<<" old version";
		return ( 0.2 ); // if table is empty, assume oldest (0.2), and port
	}
}

void QSqlRecipeDB::loadRatingCriterion( ElementList *list, int limit, int offset )
{
	list->clear();

	QString command = "SELECT id,name FROM rating_criteria ORDER BY name"
		+((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery toLoad( command, *database);
	if ( toLoad.isActive() ) {
		while ( toLoad.next() ) {
			Element el;
			el.id = toLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( toLoad.value( 1 ).toByteArray() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::loadCategories( ElementList *list, int limit, int offset )
{
	list->clear();

	m_command = "SELECT id,name FROM categories ORDER BY name"
	  +((limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset));
	QSqlQuery categoryToLoad( m_command, *database);
	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			Element el;
			el.id = categoryToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toByteArray() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::loadCategories( CategoryTree *list, int limit, int offset, int parent_id, bool recurse )
{
	QString limit_str;
	if ( parent_id == -1 ) {
		emit progressBegin(0,QString(),i18n("Loading category list"));
		list->clear();

		//only limit the number of top-level categories
		limit_str = (limit==-1)?"":" LIMIT "+QString::number(limit)+" OFFSET "+QString::number(offset);
	}

	m_command = "SELECT id,name,parent_id FROM categories WHERE parent_id='"+QString::number(parent_id)+"' ORDER BY name "+limit_str;

	QSqlQuery categoryToLoad( QString(), *database);
	//categoryToLoad.setForwardOnly(true); //FIXME? Subcategories aren't loaded if this is enabled, even though we only go forward

	categoryToLoad.exec(m_command);

	if ( categoryToLoad.isActive() ) {
		while ( categoryToLoad.next() ) {
			emit progress();

			int id = categoryToLoad.value( 0 ).toInt();
			Element el;
			el.id = id;
			el.name = unescapeAndDecode( categoryToLoad.value( 1 ).toByteArray() );
			CategoryTree *list_child = list->add( el );

			if ( recurse ) {
				//QTime dbg_timer; dbg_timer.start(); kDebug()<<"   calling QSqlRecipeDB::loadCategories";
				loadCategories( list_child, -1, -1, id ); //limit and offset won't be used
				// kDebug()<<"   done in "<<dbg_timer.elapsed()<<" ms";
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
	QSqlQuery categoryToCreate( command, *database);

	emit categoryCreated( Element( real_name, lastInsertID() ), parent_id );
}

void QSqlRecipeDB::modCategory( int categoryID, const QString &newLabel )
{
	QString command = QString( "UPDATE categories SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, *database);

	emit categoryModified( Element( newLabel, categoryID ) );
}

void QSqlRecipeDB::modCategory( int categoryID, int new_parent_id, bool emit_cat_modified )
{
	QString command = QString( "UPDATE categories SET parent_id=%1 WHERE id=%2;" ).arg( new_parent_id ).arg( categoryID );
	QSqlQuery categoryToUpdate( command, *database);

	if ( emit_cat_modified )
		emit categoryModified( categoryID, new_parent_id );
}

void QSqlRecipeDB::removeCategory( int categoryID )
{
	QString command;

	command = QString( "DELETE FROM categories WHERE id=%1;" ).arg( categoryID );
	QSqlQuery categoryToRemove( command, *database);

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
	QSqlQuery authorToLoad( command, *database);
	if ( authorToLoad.isActive() ) {
		while ( authorToLoad.next() ) {
			Element el;
			el.id = authorToLoad.value( 0 ).toInt();
			el.name = unescapeAndDecode( authorToLoad.value( 1 ).toByteArray() );
			list->append( el );
		}
	}
}

void QSqlRecipeDB::createNewAuthor( const QString &authorName )
{
	QString command;
	QString real_name = authorName.left( maxAuthorNameLength() );

	command = QString( "INSERT INTO authors VALUES(%2,'%1');" ).arg( escapeAndEncode( real_name ) ).arg( getNextInsertIDStr( "authors", "id" ) );
	QSqlQuery authorToCreate( command, *database);

	emit authorCreated( Element( real_name, lastInsertID() ) );
}

void QSqlRecipeDB::modAuthor( int authorID, const QString &newLabel )
{
	QString command;

	command = QString( "UPDATE authors SET name='%1' WHERE id=%2;" ).arg( escapeAndEncode( newLabel ) ).arg( authorID );
	QSqlQuery authorToCreate( command, *database);

	emit authorRemoved( authorID );
	emit authorCreated( Element( newLabel, authorID ) );
}

void QSqlRecipeDB::removeAuthor( int authorID )
{
	QString command;

	command = QString( "DELETE FROM authors WHERE id=%1;" ).arg( authorID );
	QSqlQuery authorToRemove( command, *database);

	emit authorRemoved( authorID );
}

int QSqlRecipeDB::findExistingAuthorByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxAuthorNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM authors WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingCategoryByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxCategoryNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM categories WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingIngredientGroupByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxIngGroupNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_groups WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingIngredientByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxIngredientNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredients WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPrepByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxPrepMethodNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM prep_methods WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingPropertyByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxPropertyNameLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM ingredient_properties WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingUnitByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxUnitNameLength() ) ); //truncate to the maximum size db holds

	QString command = "SELECT id FROM units WHERE name LIKE '" + search_str
		  + "' OR plural LIKE '" + search_str
		  + "' OR name_abbrev LIKE '" + search_str
		  + "' OR plural_abbrev LIKE '" + search_str
		  + '\'';

	QSqlQuery elementToLoad( command, *database); // Run the query
	int id = -1;

	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingRatingByName( const QString& name )
{
	QString search_str = escapeAndEncode( name ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM rating_criteria WHERE name LIKE '%1'" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingRecipeByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxRecipeTitleLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM recipes WHERE title LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

int QSqlRecipeDB::findExistingYieldTypeByName( const QString& name )
{
	QString search_str = escapeAndEncode( name.left( maxYieldTypeLength() ) ); //truncate to the maximum size db holds

	QString command = QString( "SELECT id FROM yield_types WHERE name LIKE '%1';" ).arg( search_str );
	QSqlQuery elementToLoad( command, *database); // Run the query

	int id = -1;
	if ( elementToLoad.isActive() && elementToLoad.first() )
		id = elementToLoad.value( 0 ).toInt();

	return id;
}

void QSqlRecipeDB::mergeAuthors( int id1, int id2 )
{
	QSqlQuery update( QString(), *database);

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
				int count = -1;
				command = QString( "SELECT COUNT(1) FROM author_list WHERE author_id=%1 AND recipe_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove( command, *database );
				if ( remove.isActive() && remove.first() )
					count = remove.value(0).toInt();
				if ( count > 1 ) {
					command = QString( "DELETE FROM author_list WHERE author_id=%1 AND recipe_id=%2" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );

					command = QString( "INSERT INTO author_list VALUES(%1,%2)" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );
				}
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
	QSqlQuery update( QString(), *database);

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
				int count = -1;
				command = QString( "SELECT COUNT(1) FROM category_list WHERE category_id=%1 AND recipe_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove( command, *database );
				if ( remove.isActive() && remove.first() )
					count = remove.value(0).toInt();
				if ( count > 1 ) {
					command = QString( "DELETE FROM category_list WHERE category_id=%1 AND recipe_id=%2" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );

					command = QString( "INSERT INTO category_list VALUES(%1,%2)" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );
				}
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
	QSqlQuery update( QString(), *database);

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
	QSqlQuery update( QString(), *database);

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
				int count = -1;
				command = QString( "SELECT COUNT(1) FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove( command, *database );
				if ( remove.isActive() && remove.first() )
					count = remove.value(0).toInt();
				if ( count > 1 ) {
					command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );

					command = QString( "INSERT INTO unit_list VALUES(%1,%2)" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );
				}
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
				int count = -1;
				command = QString( "SELECT COUNT(1) FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove( command, *database );
				if ( remove.isActive() && remove.first() )
					count = remove.value(0).toInt();
				if ( count > 1 ) {
					command = QString( "DELETE FROM ingredient_info WHERE ingredient_id=%1 AND property_id=%2" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );

					command = QString( "INSERT INTO ingredient_info VALUES(%1,%2)" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );
				}

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
	QSqlQuery update( QString(), *database);

	//change all instances of 'id2' to 'id1' in ingredient list
	QString command = QString( "UPDATE prep_method_list SET prep_method_id=%1 WHERE prep_method_id=%2" )
	                  .arg( id1 )
	                  .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in ingredient weights
	command = QString( "UPDATE ingredient_weights SET prep_method_id=%1 WHERE prep_method_id=%2" )
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
	QSqlQuery update( QString(), *database);

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
	QSqlQuery update( QString(), *database);

	//change all instances of 'id2' to 'id1' in unit list
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
				int count = -1;
				command = QString( "SELECT COUNT(1) FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2" )
				          .arg( id1 )
				          .arg( last_id );
				QSqlQuery remove( command, *database );
				if ( remove.isActive() && remove.first() )
					count = remove.value(0).toInt();
				if ( count > 1 ) {
					command = QString( "DELETE FROM unit_list WHERE ingredient_id=%1 AND unit_id=%2" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );

					command = QString( "INSERT INTO unit_list VALUES(%1,%2)" )
						.arg( id1 )
						.arg( last_id );
					database->exec( command );
				}
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

	//update ingredient weights
	command = QString( "UPDATE ingredient_weights SET unit_id=%1 WHERE unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );
	command = QString( "UPDATE ingredient_weights SET weight_unit_id=%1 WHERE weight_unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	//change all instances of 'id2' to 'id1' in ingredient weights
	command = QString( "UPDATE ingredient_weights SET unit_id=%1 WHERE unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
	update.exec( command );

	command = QString( "UPDATE ingredient_weights SET weight_unit_id=%1 WHERE weight_unit_id=%2" )
	          .arg( id1 )
	          .arg( id2 );
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

	QString command = QString( "SELECT COUNT(1) FROM recipes WHERE title LIKE '%1 (%)';" ).arg( escapeAndEncode( recipe_title ) );

	QSqlQuery alikeRecipes( command, *database);
	if ( alikeRecipes.isActive() && alikeRecipes.first() )
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
	QSqlQuery recipeToLoad( command, *database);
	if ( recipeToLoad.isActive() && recipeToLoad.next() )  // Go to the first record (there should be only one anyway.
		return ( unescapeAndDecode(recipeToLoad.value( 0 ).toByteArray()) );

	return ( QString() );
}

void QSqlRecipeDB::emptyData( void )
{
	kDebug();
	QStringList tables;
	tables << "ingredient_info" << "ingredient_list" << "ingredient_properties" << "ingredients" << "recipes" << "unit_list" << "units" << "units_conversion" << "categories" << "category_list" << "authors" << "author_list" << "prep_methods" << "ingredient_groups" << "yield_types" << "ratings" << "rating_criteria" << "rating_criterion_list";
	QSqlQuery tablesToEmpty( QString(), *database);
	for ( QStringList::Iterator it = tables.begin(); it != tables.end(); ++it ) {
		QString command = QString( "DELETE FROM %1;" ).arg( *it );
		tablesToEmpty.exec( command );
	}
}

void QSqlRecipeDB::empty( void )
{
	kDebug();
	QSqlQuery tablesToEmpty( QString(), *database);

	QStringList list = database->tables();
	QStringList::const_iterator it = list.constBegin();
	while( it != list.constEnd() ) {
		QString command = QString( "DROP TABLE %1;" ).arg( *it );
		tablesToEmpty.exec( command );

		if ( !tablesToEmpty.isActive() )
			kDebug()<<tablesToEmpty.lastError().databaseText();

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
	kDebug();

	QString query = buildSearchQuery(parameters);

	QList<int> ids;
	QSqlQuery recipeToLoad( query, *database);
	if ( recipeToLoad.isActive() ) {
		while ( recipeToLoad.next() ) {
			ids << recipeToLoad.value( 0 ).toInt();
		}
	}

	if ( ids.count() > 0 )
		loadRecipes( list, items, ids );
}

#include "qsqlrecipedb.moc"
