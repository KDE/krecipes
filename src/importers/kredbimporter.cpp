/***************************************************************************
 *   Copyright (C) 2004 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kredbimporter.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>

#include "datablocks/recipelist.h"
#include "DBBackend/recipedb.h"

KreDBImporter::KreDBImporter( const QString &_dbType, const QString &_host, const QString &_user, const QString &_pass ) : BaseImporter(),
  dbType(_dbType),
  host(_host),
  user(_user),
  pass(_pass)
{
}

KreDBImporter::~KreDBImporter()
{
}

void KreDBImporter::parseFile( const QString &file ) //this is either a database file or a database table
{
	RecipeDB *database = RecipeDB::createDatabase(dbType,host,user,pass,file,file); //uses 'file' as either table or file name, depending on the database

	if ( database ) {
		database->connect();
	
		if ( database->ok() ) {
			//get the ids
			ElementList list;
			database->loadRecipeList(&list);
		
			QValueList<int> ids;
			for ( ElementList::const_iterator it = list.begin(); it != list.end(); ++it ) {
				ids << (*it).id;
			}
		
			//now load recipes
			RecipeList recipes;
			database->loadRecipes(&recipes,ids);
		
			//now add these recipes to the importer
			add( recipes );
		}
		else
			; //TODO: Give an error
	}
}
