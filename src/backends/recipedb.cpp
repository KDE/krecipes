/***************************************************************************
*   Copyright (C) 2003                                                    *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2003-2006 Jason Kivlighn (jkivlighn@gmail.com)          *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "backends/recipedb.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprogress.h>
#include <kglobal.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kprocess.h>
#include <kprocio.h>
#include <kfilterdev.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <map>

#include "importers/kreimporter.h"

#if HAVE_POSTGRESQL
#include "PostgreSQL/psqlrecipedb.h"
#endif

#if HAVE_MYSQL
#include "MySQL/mysqlrecipedb.h"
#endif

#if HAVE_SQLITE || HAVE_SQLITE3
#include "SQLite/literecipedb.h"
#endif

#include "datablocks/categorytree.h"
#include "datablocks/ingredientpropertylist.h"
#include "datablocks/weight.h"

#include "searchparameters.h"

#include "usda_property_data.h"
#include "usda_ingredient_data.h"
#include "usda_unit_data.h"

#define DB_FILENAME "krecipes.krecdb"

struct ingredient_nutrient_data
{
	int usda_id;
	QString name;
	QValueList<double> data;
	WeightList weights;
};

RecipeDB::RecipeDB() : 
	DCOPObject(),
	QObject(), m_categoryCache(0), haltOperation(false)
{
	dbOK = false;
	dbErr = "";
}

RecipeDB::~RecipeDB()
{
}

double RecipeDB::latestDBVersion() const
{
	return 0.96;
}

QString RecipeDB::krecipes_version() const
{
	KInstance * this_instance = KGlobal::instance();
	if ( this_instance && this_instance->aboutData() )
		return this_instance->aboutData() ->version();

	return QString::null; //Oh, well.  We couldn't get the version (shouldn't happen).
}

RecipeDB* RecipeDB::createDatabase( const QString &dbType, const QString &file )
{
	KConfig * config = kapp->config();
	config->setGroup( "Server" );
	QString host = config->readEntry( "Host", "localhost" );
	QString user = config->readEntry( "Username", QString::null );
	QString pass = config->readEntry( "Password", QString::null );
	QString dbname = config->readEntry( "DBName", DEFAULT_DB_NAME );
	int port = config->readNumEntry( "Port", 0 );

	QString f = file;
	if ( f.isEmpty() )
		f = config->readEntry( "DBFile", locateLocal ( "appdata", DB_FILENAME ) );

	return createDatabase( dbType, host, user, pass, dbname, port, f );
}

RecipeDB* RecipeDB::createDatabase( const QString &dbType, const QString &host, const QString &user, const QString &pass, const QString &dbname, int port, const QString &file )
{
	RecipeDB * database = 0;

	if ( 0 )
		; //we need some condition here
#if HAVE_SQLITE || HAVE_SQLITE3

	else if ( dbType == "SQLite" ) {
		database = new LiteRecipeDB( file );
	}
#endif //HAVE_SQLITE || HAVE_SQLITE3
	#if HAVE_MYSQL
	else if ( dbType == "MySQL" ) {
		database = new MySQLRecipeDB( host, user, pass, dbname, port );
	}
#endif //HAVE_MYSQL
	#if HAVE_POSTGRESQL
	else if ( dbType == "PostgreSQL" ) {
		database = new PSqlRecipeDB( host, user, pass, dbname, port );
	}
#endif //HAVE_POSTGRESQL
	else {
		kdDebug() << "No database support included (or available) for the " << dbType << " database." << endl;
	}

	return database;
}

void RecipeDB::updateCategoryCache( int limit )
{
	m_categoryCache = new CategoryTree;
	loadCategories( m_categoryCache, limit, 0, -1, true );
}

void RecipeDB::clearCategoryCache()
{
	delete m_categoryCache;
	m_categoryCache = 0;
}

void RecipeDB::loadCachedCategories( CategoryTree **list, int limit, int offset, int parent_id, bool recurse )
{
	if ( m_categoryCache ) {
		if ( parent_id == -1 )
			*list = m_categoryCache;
		else //FIXME?: how slow is this find() call?  the cache is loaded in sequential order, so should we iterate over the cache?
			*list = m_categoryCache->find(parent_id);
		//kdDebug() << "Loading category tree from the cache" << endl;
	}
	else {
		loadCategories( *list, limit, offset, parent_id, recurse );
	}
}

RecipeDB::ConversionStatus RecipeDB::convertIngredientUnits( const Ingredient &from, const Unit &to, Ingredient &result )
{
	result = from;

	if ( from.units.id == to.id )
		return Success;

	if ( from.units.type == to.type && to.type != Unit::Other ) {
		double ratio = unitRatio( from.units.id, to.id );
		if ( ratio > 0 ) {
			result.amount = from.amount * ratio;
			result.units = to;

			kdDebug()<<"Unit conversion SUCCESSFUL, from "<<unitName(from.units.id).name<<" to "<<unitName(to.id).name<<" for ingredient "<<ingredientName(from.ingredientID)<<" (ingredient not used in conversion)"<<endl;

			return Success;
		}
		else {
			kdDebug()<<"Unit conversion failed, you should probably update your unit conversion table."<<endl;
			kdDebug()<<from.units.id<<" to "<<to.id<<endl;
			return MissingUnitConversion;
		}
	}
	else if ( to.type == Unit::Mass || from.units.type == Unit::Mass ) {
		if ( from.ingredientID == -1 )
			return MissingIngredient;

		double fromToWeightRatio, weightToToRatio;
		int unitID = -1;
		int prepID = -2;

		WeightList idList = ingredientWeightUnits( from.ingredientID );

		if ( idList.count() == 0 )
			return MissingIngredientWeight;

		for ( WeightList::const_iterator it = idList.begin(); it != idList.end(); ++it ) {
			//get conversion order correct (i.e., Mass -> Volume instead of Volume -> Mass, depending on unit type)
			int first   = (to.type == Unit::Mass)?(*it).perAmountUnitID:(*it).weightUnitID;
			int second  = (to.type == Unit::Mass)?(*it).weightUnitID:(*it).perAmountUnitID;
			double tryFromToWeightRatio = unitRatio( from.units.id, first );
			if ( tryFromToWeightRatio > 0 ) {
				weightToToRatio = unitRatio( second, to.id );
				fromToWeightRatio = tryFromToWeightRatio;
				unitID = first;

				kdDebug()<<"units work, is it the right prep method..."<<endl;
				if ( from.prepMethodList.containsId( (*it).prepMethodID ) ) {
					kdDebug()<<"   yes"<<endl;
					prepID = (*it).prepMethodID;
					break;
				}
				kdDebug()<<"   no, keep going"<<endl;
			}
		}
		if ( unitID == -1 )
			return MissingUnitConversion;

		bool wasApproximated;

		Ingredient i;
		i.ingredientID = from.ingredientID;
		i.units.id = unitID;
		i.amount = from.amount * fromToWeightRatio;
		i.prepMethodList = from.prepMethodList;
		result.amount = ingredientWeight( i, &wasApproximated ) * weightToToRatio;
		result.units = to;

		if ( result.amount < 0 )
			return MismatchedPrepMethod;
		else if ( wasApproximated )
			return MismatchedPrepMethodUsingApprox;

		return Success;
	}
	else {
		QString to_str;
		switch ( to.type ) {
		case Unit::Other: to_str = "Other"; break;
		case Unit::Mass: to_str = "Mass"; break;
		case Unit::Volume: to_str = "Volume"; break;
		case Unit::All: kdDebug()<<"Code error: trying to convert to unit of type 'All'"<<endl; return InvalidTypes;
		}
		QString from_str;
		switch ( from.units.type ) {
		case Unit::Other: from_str = "Other"; break;
		case Unit::Mass: from_str = "Mass"; break;
		case Unit::Volume: from_str = "Volume"; break;
		case Unit::All: kdDebug()<<"Code error: trying to convert from unit of type 'All'"<<endl; return InvalidTypes;
		}
		kdDebug()<<"Can't handle conversion from "<<from_str<<"("<<from.units.id<<") to "<<to_str<<"("<<to.id<<")"<<endl;

		return InvalidTypes;
	}
}

bool RecipeDB::backup( const QString &backup_file, QString *errMsg )
{
	kdDebug()<<"Backing up current database to "<<backup_file<<endl;

	KProcess *p = new KProcess;
	//p->setUseShell(true);

	QIODevice *dumpFile = KFilterDev::deviceForFile(backup_file,"application/x-gzip");
	if ( !dumpFile->open( IO_WriteOnly ) ) {
		kdDebug()<<"Couldn't open "<<backup_file<<endl;
		return false;
	}

	dumpStream = new QTextStream( dumpFile );

	QStringList command = backupCommand();
	if ( command.count() == 0 ) {
		kdDebug()<<"Backup not available for this database backend"<<endl;
		return false;
	}

	KConfig * config = kapp->config();
	config->setGroup( "DBType" );

	(*dumpStream) << "-- Generated for Krecipes v"<<krecipes_version()<<endl;
	(*dumpStream) << "-- Krecipes database schema: "<<latestDBVersion()<<endl;
	(*dumpStream) << "-- Krecipes database backend: "<<config->readEntry( "Type" )<<endl;

	kdDebug()<<"Running '"<<command.first()<<"' to create backup file"<<endl;
	*p << command /*<< ">" << backup_file*/;

	QApplication::connect( p, SIGNAL(receivedStdout(KProcess*,char*,int)), this, SLOT(processDumpOutput(KProcess*,char*,int)) );
	QApplication::connect( p, SIGNAL(receivedStderr(KProcess*,char*,int)), this, SLOT(processDumpOutput(KProcess*,char*,int)) );

	emit progressBegin(0,QString::null,
		QString("<center><b>%1</b></center>%2")
			.arg(i18n("Creating complete backup"))
			.arg(i18n("Depending on the number of recipes and amount of data, this could take some time.")),50);

	bool success = p->start( KProcess::Block, KProcess::AllOutput );
	if ( !success ) {
		if ( errMsg ) *errMsg = QString(i18n("Unable to find or run the program '%1'.  Either it is not installed on your system or it is not in $PATH.")).arg(command.first());
		delete p;
		delete dumpStream;
		delete dumpFile;
		QFile::remove(backup_file);
		emit progressDone();
		return false;
	}

	emit progressDone();

	//User cancelled it; we'll still consider the operation successful,
	//but delete the file we created
	if ( !p->normalExit() ) {
		kdDebug()<<"Process killed, deleting partial backup."<<endl;
		QFile::remove(backup_file);
	}

	if ( p->exitStatus() != 0 ) {
		//Since the process failed, dumpStream should have output from the app as to why it did
		QString appOutput;
		dumpFile->close();
		if ( dumpFile->open( IO_ReadOnly ) ) {
			QTextStream appErrStream( dumpFile );

			//ignore our own versioning output
			appErrStream.readLine();
			appErrStream.readLine();
			appErrStream.readLine();

			appOutput = appErrStream.read();
		}
		else
			kdDebug()<<"Unable to open file to get error output."<<endl;

		if ( errMsg ) *errMsg = QString("%1\n%2").arg(i18n("Backup failed.")).arg(appOutput);
		QFile::remove(backup_file);
		delete p;
		delete dumpStream;
		delete dumpFile;
		return false;
	}

	delete p;
	delete dumpStream;
	delete dumpFile;
	return true;
}

void RecipeDB::processDumpOutput( KProcess *p, char *buffer, int buflen )
{
	int written = dumpStream->device()->writeBlock(buffer,buflen);
	if ( written != buflen )
		kdDebug()<<"Data lost: written ("<<written<<") != buflen ("<<buflen<<")"<<endl;

	if ( haltOperation ) { haltOperation=false; p->kill(); return; }
	emit progress();
}

void RecipeDB::initializeData( void )
{
	// Populate with data

	// Read the commands form the data file
	QFile datafile( KGlobal::dirs() ->findResource( "appdata", "data/data.sql" ) );
	if ( datafile.open( IO_ReadOnly ) ) {
		QTextStream stream( &datafile );
		execSQL(stream);
		datafile.close();
	}
}

bool RecipeDB::restore( const QString &file, QString *errMsg )
{
	QIODevice *dumpFile = KFilterDev::deviceForFile(file,"application/x-gzip");
	if ( dumpFile->open( IO_ReadOnly ) ) {

		QTextStream stream( dumpFile );
		QString firstLine = stream.readLine().stripWhiteSpace();
		QString dbVersion = stream.readLine().stripWhiteSpace();
		dbVersion = dbVersion.right( dbVersion.length() - dbVersion.find(":") - 2 );
		if ( qRound(dbVersion.toDouble()*1e5) > qRound(latestDBVersion()*1e5) ) { //correct for float's imprecision
			if ( errMsg ) *errMsg = i18n( "This backup was created with a newer version of Krecipes and cannot be restored." );
			delete dumpFile;
			return false;
		}

		KConfig * config = kapp->config();
		config->setGroup( "DBType" );
		QString dbType = stream.readLine().stripWhiteSpace();
		dbType = dbType.right( dbType.length() - dbType.find(":") - 2 );
		if ( dbType.isEmpty() || !firstLine.startsWith("-- Generated for Krecipes") ) {
			if ( errMsg ) *errMsg = i18n("This file is not a Krecipes backup file or has become corrupt.");
			delete dumpFile;
			return false;
		}
		else if ( dbType != config->readEntry("Type",QString::null) ) {
			if ( errMsg ) *errMsg = QString(i18n("This backup was created using the \"%1\" backend.  It can only be restored into a database using this backend." )).arg(dbType);
			delete dumpFile;
			return false;
		}

		
		//We have to first wipe the database structure.  Note that if we load a dump
		//with from a previous version of Krecipes, the difference in structure
		// wouldn't allow the data to be inserted.  This remains forward-compatibity
		//by loading the old schema and then porting it to the current version.
		empty(); //the user had better be warned!

		KProcIO *process = new KProcIO;

		QStringList command = restoreCommand();
		kdDebug()<<"Restoring backup using: "<<command[0]<<endl;
		*process << command;
		
		//process->setComm( KProcess::Stdin );
		if ( process->start( KProcess::NotifyOnExit ) ) {
			emit progressBegin(0,QString::null,
				QString("<center><b>%1</b></center>%2")
					.arg(i18n("Restoring backup"))
					.arg(i18n("Depending on the number of recipes and amount of data, this could take some time.")));

			do {
				QByteArray array(4096);
				int len = dumpFile->readBlock(array.data(),array.size());
				array.resize(len);

				if ( !process->writeStdin(array) )
					kdDebug()<<"Yikes! Some input couldn't be written to the process!"<<endl;

				if ( haltOperation ) { break; }
				emit progress();
			}
			while ( !stream.atEnd() );

			process->closeWhenDone();

			//Since the process will exit when all stdin has been sent and processed,
			//just loop until the process is no longer running.  If something goes
			//wrong, the user can still hit cancel.
			int prog = 0;
			while ( process->isRunning() ){
				if ( haltOperation ) { break; }
				kapp->processEvents();
				if ( prog % 100 == 0 ) {
					emit progress();
					prog = 0;
				}
				++prog;
			}
		}
		else
			kdDebug()<<"Unable to start process"<<endl;

		delete process;
		emit progressDone();

		//Since we just loaded part of a file, the database won't be in a usable state.
		//We'll wipe out the database structure and recreate it, leaving no data.
		if ( haltOperation ) {
			haltOperation=false;
			empty();
			checkIntegrity();
			delete dumpFile;
			if ( errMsg ) { *errMsg = i18n("Restore Failed"); }
			return false;
		}

		dumpFile->close();

		checkIntegrity();
	}
	else {
		kdDebug()<<"Unable to open the selected backup file"<<endl;
		return false;
	}

	delete dumpFile;
	return true;
}

void RecipeDB::execSQL( QTextStream &stream )
{
	QString line, command;
	while ( (line = stream.readLine()) != QString::null ) {
		command += " "+line;
		if ( command.startsWith(" --") ) {
			command = QString::null;
		}
		else if ( command.endsWith(";") ) {
			execSQL( command );
			command = QString::null;
		}
	}
}

void RecipeDB::loadRecipe( Recipe *recipe, int items, int id )
{
	RecipeList rlist;
	QValueList<int> ids; ids << id;
	loadRecipes( &rlist, items, ids );

	*recipe = *rlist.begin();
}

int RecipeDB::categoryCount()
{
	return getCount("categories");
}

int RecipeDB::authorCount()
{
	return getCount("authors");
}

int RecipeDB::ingredientCount()
{
	return getCount("ingredients");
}

int RecipeDB::prepMethodCount()
{
	return getCount("prep_methods");
}

int RecipeDB::unitCount()
{
	return getCount("units");
}

void RecipeDB::importSamples()
{
	QString sample_recipes = locate( "appdata", "data/samples-" + KGlobal::locale() ->language() + ".kreml" );
	if ( sample_recipes.isEmpty() ) {
		//TODO: Make this a KMessageBox??
		kdDebug() << "NOTICE: Samples recipes for the language \"" << KGlobal::locale() ->language() << "\" are not available.  However, if you would like samples recipes for this language included in future releases of Krecipes, we invite you to submit your own.  Just save your favorite recipes in the kreml format and e-mail them to jkivlighn@gmail.com." << endl;

		sample_recipes = locate( "appdata", "data/samples-en_US.kreml" ); //default to English
	}
	if ( !sample_recipes.isEmpty() ) {
		KreImporter importer;

		QStringList file;
		file << sample_recipes;
		importer.parseFiles( file );

		importer.import( this, true );
	}
	else
		kdDebug() << "Unable to find samples recipe file (samples-en_US.kreml)" << endl;
}

void RecipeDB::getIDList( const CategoryTree *categoryTree, QStringList &ids )
{
	for ( CategoryTree * child_it = categoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
		ids << QString::number(child_it->category.id);
		getIDList(child_it,ids );
	}
}

QString RecipeDB::buildSearchQuery( const RecipeSearchParameters &p ) const
{
	QStringList queryList, conditionList, tableList;

	if ( p.ingsOr.count() != 0 ) {
		tableList << "ingredient_list il" << "ingredients i";
		conditionList << "il.ingredient_id=i.id" << "il.recipe_id=r.id";

		QString condition = "(";
		for ( QStringList::const_iterator it = p.ingsOr.begin(); it != p.ingsOr.end();) {
			condition += "i.name LIKE '%"+escapeAndEncode(*it)+"%' ";
			if ( ++it != p.ingsOr.end() ) {
				condition += "OR ";
			}
		}
		condition += ")";

		conditionList << condition;
	}

	if ( p.catsOr.count() != 0 ) {
		tableList << "category_list cl" << "categories c";
		conditionList << "cl.category_id=c.id" << "cl.recipe_id=r.id";

		QString condition = "(";
		for ( QStringList::const_iterator it = p.catsOr.begin(); it != p.catsOr.end();) {
			condition += "c.name LIKE '%"+escapeAndEncode(*it)+"%' ";
			if ( ++it != p.catsOr.end() ) {
				condition += "OR ";
			}
		}
		condition += ")";

		conditionList << condition;
	}

	if ( p.authorsOr.count() != 0 ) {
		tableList << "author_list al" << "authors a";
		conditionList << "al.author_id=a.id" << "al.recipe_id=r.id";

		QString condition = "(";
		for ( QStringList::const_iterator it = p.authorsOr.begin(); it != p.authorsOr.end();) {
			condition += "a.name LIKE '%"+escapeAndEncode(*it)+"%'";
			if ( ++it != p.authorsOr.end() ) {
				condition += "OR ";
			}
		}
		condition += ")";

		conditionList << condition;
	}

	if ( p.titleKeywords.count() != 0 ) {
		QString op = (p.requireAllTitleWords) ? "AND " : "OR ";

		QString condition = "(";
		for ( QStringList::const_iterator it = p.titleKeywords.begin(); it != p.titleKeywords.end();) {
			condition += "r.title LIKE '%"+escapeAndEncode(*it)+"%' ";
			if ( ++it != p.titleKeywords.end() ) {
				condition += op;
			}
		}
		condition += ")";
		conditionList << condition;
	}

	if ( p.instructionsKeywords.count() != 0 ) {
		QString op = (p.requireAllInstructionsWords) ? "AND " : "OR ";

		QString condition = "(";
		for ( QStringList::const_iterator it = p.instructionsKeywords.begin(); it != p.instructionsKeywords.end();) {
			condition += "r.instructions LIKE '%"+escapeAndEncode(*it)+"%' ";
			if ( ++it != p.instructionsKeywords.end() ) {
				condition += op;
			}
		}
		condition += ")";
		conditionList << condition;
	}

	if ( !p.prep_time.isNull() ) {
		QString op;
		switch ( p.prep_param ) {
			case 0: op = "<= "+p.prep_time.toString( "'hh:mm:ss'" ); break;
			case 1: //TODO: have a configurable 'about'.  It tests within 15 minutes for now.
				QTime lower = p.prep_time; lower.addSecs( 60*15 );
				QTime upper = p.prep_time; upper.addSecs( 60*-15 );
				op = "BETWEEN "+lower.toString( "'hh:mm:ss'" )+" AND "+upper.toString( "'hh:mm:ss'" );
				break;
		}
		conditionList << "r.prep_time "+op;
	}

	if ( p.servings > 0 ) {
		QString op;
		switch ( p.servings_param ) {
			case 0: op = "> "+QString::number(p.servings); break;
			case 1: op = "< "+QString::number(p.servings); break;
			case 2: op = "BETWEEN "+QString::number(p.servings-5)+" AND "+QString::number(p.servings+5); break;
		}
		conditionList << "r.yield_amount "+op;
	}

	if ( p.createdDateBegin.isValid() ) {
		if ( p.createdDateEnd.isValid() ) {
			conditionList << "r.ctime >= '"+p.createdDateBegin.toString(Qt::ISODate)+"'";
			conditionList << "r.ctime <= '"+p.createdDateEnd.toString(Qt::ISODate)+"'";
		}
		else {
			if ( p.createdDateBegin.time().isNull() ) { //we just want something on a particular date, not time
				QDateTime end = p.createdDateBegin.addDays(1);
				conditionList << "r.ctime >= '"+p.createdDateBegin.toString(Qt::ISODate)+"'";
				conditionList << "r.ctime <= '"+end.toString(Qt::ISODate)+"'";
			}
			else //use the exact time
				conditionList << "r.ctime = '"+p.createdDateBegin.toString(Qt::ISODate)+"'";
		}
	}

	if ( p.modifiedDateBegin.isValid() ) {
		if ( p.modifiedDateEnd.isValid() ) {
			conditionList << "r.mtime >= '"+p.modifiedDateBegin.toString(Qt::ISODate)+"'";
			conditionList << "r.mtime <= '"+p.modifiedDateEnd.toString(Qt::ISODate)+"'";
		}
		else {
			if ( p.modifiedDateBegin.time().isNull() ) { //we just want something on a particular date, not time
				QDateTime end = p.modifiedDateBegin.addDays(1);
				conditionList << "r.mtime >= '"+p.modifiedDateBegin.toString(Qt::ISODate)+"'";
				conditionList << "r.mtime <= '"+end.toString(Qt::ISODate)+"'";
			}
			else //use the exact time
				conditionList << "r.mtime = '"+p.modifiedDateBegin.toString(Qt::ISODate)+"'";
		}
	}

	if ( p.accessedDateBegin.isValid() ) {
		if ( p.accessedDateEnd.isValid() ) {
			conditionList << "r.atime >= '"+p.accessedDateBegin.toString(Qt::ISODate)+"'";
			conditionList << "r.atime <= '"+p.accessedDateEnd.toString(Qt::ISODate)+"'";
		}
		else {
			if ( p.accessedDateBegin.time().isNull() ) { //we just want something on a particular date, not time
				QDateTime end = p.accessedDateBegin.addDays(1);
				conditionList << "r.atime >= '"+p.accessedDateBegin.toString(Qt::ISODate)+"'";
				conditionList << "r.atime <= '"+end.toString(Qt::ISODate)+"'";
			}
			else //use the exact time
				conditionList << "r.atime = '"+p.accessedDateBegin.toString(Qt::ISODate)+"'";
		}
	}

	QString wholeQuery = "SELECT r.id FROM recipes r"
		+QString(tableList.count()!=0?","+tableList.join(","):"")
		+QString(conditionList.count()!=0?" WHERE "+conditionList.join(" AND "):"");

	kdDebug()<<"calling: "<<wholeQuery<<endl;
	return wholeQuery+";";
}

//These are helper functions solely for use by the USDA data importer
void getIngredientNameAndID( std::multimap<int, QString> * );
int createUnit( const QString &name, Unit::Type, RecipeDB* );
int createIngredient( const QString &name, RecipeDB*, bool do_checks );
void create_properties( RecipeDB* );

void RecipeDB::importUSDADatabase()
{
	//check if the data file even exists before we do anything
	QString abbrev_file = locate( "appdata", "data/abbrev.txt" );
	if ( abbrev_file.isEmpty() ) {
		kdDebug() << "Unable to find abbrev.txt data file." << endl;
		return ;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) ) {
		kdDebug() << "Unable to open data file: " << abbrev_file << endl;
		return ;
	}

	create_properties( this );

	std::multimap<int, QString> *ings_and_ids = new std::multimap<int, QString>;
	getIngredientNameAndID( ings_and_ids );

	QTextStream stream( &file );
	QValueList<ingredient_nutrient_data> *data = new QValueList<ingredient_nutrient_data>;

	kdDebug() << "Parsing abbrev.txt" << endl;
	while ( !stream.atEnd() ) {
		QStringList fields = QStringList::split( "^", stream.readLine(), true );

		int id = fields[ 0 ].mid( 1, fields[ 0 ].length() - 2 ).toInt();

		std::multimap<int, QString>::iterator current_pair;
		while ( ( current_pair = ings_and_ids->find( id ) ) != ings_and_ids->end() )  //there may be more than one ingredients with the same id
		{
			ingredient_nutrient_data current_ing;
			current_ing.name = ( *current_pair ).second.latin1();

			for ( int i = 2; i < TOTAL_USDA_PROPERTIES + 2; i++ )  //properties start at the third field (index 2)
				current_ing.data << fields[ i ].toDouble();

			Weight w;
			w.weight = fields[ TOTAL_USDA_PROPERTIES + 2 ].toDouble();

			QString amountAndWeight = fields[ TOTAL_USDA_PROPERTIES + 3 ].mid( 1, fields[ TOTAL_USDA_PROPERTIES + 3 ].length() - 2 );
			if ( !amountAndWeight.isEmpty() ) {
				int spaceIndex = amountAndWeight.find(" ");
				w.perAmount = amountAndWeight.left(spaceIndex).toDouble();

				QString perAmountUnit = amountAndWeight.right(amountAndWeight.length()-spaceIndex-1);

				if ( parseUSDAUnitAndPrep( perAmountUnit, w.perAmountUnit, w.prepMethod ) )
					current_ing.weights << w;
			}

			w = Weight();
			w.weight = fields[ TOTAL_USDA_PROPERTIES + 4 ].toDouble();
			amountAndWeight = fields[ TOTAL_USDA_PROPERTIES + 5 ].mid( 1, fields[ TOTAL_USDA_PROPERTIES + 5 ].length() - 2 );
			if ( !amountAndWeight.isEmpty() ) {
				int spaceIndex = amountAndWeight.find(" ");
				w.perAmount = amountAndWeight.left(spaceIndex).toDouble();
				QString perAmountUnit = amountAndWeight.right(amountAndWeight.length()-spaceIndex-1);

				if ( parseUSDAUnitAndPrep( perAmountUnit, w.perAmountUnit, w.prepMethod ) )
					current_ing.weights << w;
			}

			current_ing.usda_id = id;

			data->append( current_ing );

			ings_and_ids->erase( current_pair );
		}
	}

	delete ings_and_ids;

	//there's 13009 lines in the weight file
	emit progressBegin( data->count(), i18n( "Nutrient Import" ), i18n( "Importing USDA nutrient data" ) );

	//if there is no data in the database, we can really speed this up with this
	bool do_checks = true;
	{
		ElementList ing_list;
		loadIngredients( &ing_list );

		if ( ing_list.count() == 0 ) {
			kdDebug()<<"Found an empty database... enabling fast nutrient import"<<endl;
			do_checks = false;
		}
	}

	//since this is the only unit used, lets just create it and store its id for speed
	int unit_g_id = createUnit( QString::fromLatin1("g"), Unit::Mass, this );

	QValueList<ingredient_nutrient_data>::const_iterator it;
	QValueList<ingredient_nutrient_data>::const_iterator data_end = data->end();
	const int total = data->count();
	int counter = 0;

	for ( it = data->begin(); it != data_end; ++it ) {
		counter++;
		kdDebug() << "Inserting (" << counter << " of " << total << "): " << ( *it ).name << endl;

		if ( haltOperation ) { haltOperation=false; break;}
		emit progress();

		int assigned_id = createIngredient( ( *it ).name, this, do_checks );

		//for now, only check if there is any info on the ingredient to see whether or not we will import this data,
		//because checking to see that each property exists is quite slow
		IngredientPropertyList ing_properties;
		if ( do_checks ) loadProperties( &ing_properties, assigned_id );
		if ( ing_properties.count() == 0 )  //ingredient doesn't already have any properties
		{
			QValueList<double>::const_iterator property_it;
			QValueList<double>::const_iterator property_end = ( *it ).data.end();
			int i = 0;
			for ( property_it = ( *it ).data.begin(); property_it != property_end; ++property_it, ++i )
				addPropertyToIngredient( assigned_id, property_data_list[ i ].id, ( *property_it ) / 100.0, unit_g_id );
		}

		WeightList existingWeights = ingredientWeightUnits( assigned_id );
		const WeightList weights = (*it).weights;
		for ( WeightList::const_iterator weight_it = weights.begin(); weight_it != weights.end(); ++weight_it ) {
			Weight w = *weight_it;
			w.perAmountUnitID = createUnit( w.perAmountUnit, Unit::Other, this );
			w.weightUnitID = unit_g_id;
			w.ingredientID = assigned_id;

			//TODO optimze by creating all prep methods and storing them for faster non-db access
			if ( !w.prepMethod.isEmpty() ) {
				int prepID = findExistingPrepByName( w.prepMethod );
				if ( prepID == -1 ) {
					createNewPrepMethod( w.prepMethod );
					prepID = lastInsertID();
				}
				w.prepMethodID = prepID;
			}

			bool exists = false;
			for ( WeightList::const_iterator it = existingWeights.begin(); it != existingWeights.end(); ++it ) {
				if ( (*it).perAmountUnitID == w.perAmountUnitID && (*it).prepMethodID == w.prepMethodID ) {
					exists = true;
					break;
				}
			}
			if ( exists )
				continue;

			addIngredientWeight( w );
		}
	}

	delete data;

	kdDebug() << "USDA data import successful" << endl;

	emit progressDone();
}

void getIngredientNameAndID( std::multimap<int, QString> *data )
{
	for ( int i = 0; !ingredient_data_list[ i ].name.isEmpty(); i++ )
		data->insert( std::make_pair( ingredient_data_list[ i ].usda_id, ingredient_data_list[ i ].name ) );
}

int createIngredient( const QString &name, RecipeDB *database, bool do_checks )
{
	bool ingredientExisted = true;
	int assigned_id = -1;
	if ( do_checks )
		assigned_id = database->findExistingIngredientByName( name );

	if ( assigned_id == -1 ) {
		ingredientExisted = false;
		database->createNewIngredient( name );
		assigned_id = database->lastInsertID();
	}

	return assigned_id;
}

int createUnit( const QString &name, Unit::Type type, RecipeDB *database )
{
	int assigned_id = database->findExistingUnitByName( name );

	if ( assigned_id == -1 )  //create unit since it doesn't exist
	{
		Unit unit(name, name);
		unit.type = type;
		database->createNewUnit( unit );
		assigned_id = database->lastInsertID();
	}
	//keep what the user specified if the type here is Other
	else if ( type != Unit::Other ) {
		Unit unit = database->unitName(assigned_id);
		if ( unit.type != type ) {
			unit.type = type;
			database->modUnit( unit );
		}
	}

	return assigned_id;
}

void create_properties( RecipeDB *database )
{
	IngredientPropertyList property_list;
	database->loadProperties( &property_list );

	for ( int i = 0; !property_data_list[ i ].name.isEmpty(); i++ ) {
		property_data_list[ i ].id = property_list.findByName( property_data_list[ i ].name );
		if ( property_data_list[ i ].id == -1 ) //doesn't exist, so insert it and set property_data_list[i].id
		{
			database->addProperty( property_data_list[ i ].name, QString::fromUtf8(property_data_list[ i ].unit) );
			property_data_list[ i ].id = database->lastInsertID();
		}
	}
}

bool parseUSDAUnitAndPrep( const QString &string, QString &unit, QString &prep )
{
	int commaIndex = string.find(",");
	QString unitPart = string.left(commaIndex);
	QString prepPart = string.right(string.length()-commaIndex-2).stripWhiteSpace();

	bool acceptable = false;
	for ( int i = 0; unit_data_list[ i ].name; ++i ) {
		if ( unitPart == unit_data_list[ i ].name || unitPart == unit_data_list[ i ].plural )
			acceptable = true;
	}
	if ( !acceptable )
		return false;

	acceptable = false;
	if ( prepPart.isEmpty() )
		acceptable = true;
	else {
		for ( int i = 0; prep_data_list[ i ]; ++i ) {
			if ( prepPart == prep_data_list[ i ] )
				acceptable = true;
		}
	}
	if ( !acceptable )
		prepPart = QString::null;

	unit = unitPart;
	prep = prepPart;
	return true;
}

//Fix property units from databases <= 0.95
void RecipeDB::fixUSDAPropertyUnits()
{
	IngredientPropertyList property_list;
	loadProperties( &property_list );

	for ( int i = 0; !property_data_list[ i ].name.isEmpty(); i++ ) {
		property_data_list[ i ].id = property_list.findByName( property_data_list[ i ].name );
		if ( property_data_list[ i ].id != -1 ) //doesn't exist, so insert it and set property_data_list[i].id
		{
			modProperty( property_data_list[ i ].id, property_data_list[ i ].name, QString::fromUtf8(property_data_list[ i ].unit) );
		}
	}
}

#include "recipedb.moc"
