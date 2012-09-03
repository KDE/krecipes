/*****************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>        *
*                                                                            *
*   This program is free software; you can redistribute it and/or modify     *
*   it under the terms of the GNU General Public License as published by     *
*   the Free Software Foundation; either version 2 of the License, or        *
*   (at your option) any later version.                                      *
******************************************************************************/

#include "nukeconfigwidget.h"

#include "ui_nukeconfigwidget.h"

#include <KApplication>
#include <KGlobal>
#include <QStringList>
#include <KStandardDirs>
#include <KConfigGroup>
#include <KMessageBox>

#include "backends/recipedb.h"


NukeConfigWidget::NukeConfigWidget( QWidget * parent ) : 
	QWidget( parent ), 
	ui(new Ui::NukeConfigWidget),
	m_totalCategories(0),
	m_totalRecipes(0),
	m_totalAuthors(0)
{
	//Setup UI
	ui->setupUi( this );

	//Conect signals/slots.
	connect( ui->m_rootCategoriesNumberInput, SIGNAL(valueChanged(int)),
		this, SLOT(updateTotals(int)) );
	connect( ui->m_subCategoriesNumberInput, SIGNAL(valueChanged(int)),
		this, SLOT(updateTotals(int)) );
	connect( ui->m_recipesNumInput, SIGNAL(valueChanged(int)),
		this, SLOT(updateTotals(int)) );
	connect( ui->m_recipeAuthorsNumInput, SIGNAL(valueChanged(int)),
		this, SLOT(updateTotals(int)) );
	connect( ui->m_standaloneAuthorsNumInput, SIGNAL(valueChanged(int)),
		this, SLOT(updateTotals(int)) );

	connect( ui->m_wipeDatabaseButton, SIGNAL(clicked()), this, SLOT(wipeDatabase()) );
	connect( ui->m_proceedButton, SIGNAL(clicked()), this, SLOT(proceed()) );
	connect( ui->m_quitButton, SIGNAL(clicked()), kapp, SLOT(quit()) );
}

void NukeConfigWidget::connectToDatabase()
{
        QString krecipesConfigFileName =
                KStandardDirs::locateLocal( "config", "krecipesrc" );

        QStringList strList;
        strList << krecipesConfigFileName;
        KGlobal::config()->addConfigSources( strList );

        KConfigGroup dbtypeGroup = KGlobal::config()->group( "DBType" );
        QString dbType = dbtypeGroup.readEntry( "Type", "" );

        KConfigGroup serverGroup = KGlobal::config()->group( "Server" );
        QString host = serverGroup.readEntry( "Host", "localhost" );
        QString user = serverGroup.readEntry( "Username", QString() );
        QString pass = serverGroup.readEntry( "Password", QString() );
        QString dbname = serverGroup.readEntry( "DBName", "Krecipes" );
        int port = serverGroup.readEntry( "Port", 0 );
        QString dbfile = serverGroup.readEntry( "DBFile",
                KStandardDirs::locateLocal ( "appdata", "krecipes.krecdb" ) );

        database = RecipeDB::createDatabase( dbType, host, user, pass, dbname, port, dbfile );

        database->connect();

        if ( !database->ok() ) {
                KMessageBox::error( this, "Error opening the Krecipes database.");
        }
}


void NukeConfigWidget::proceed()
{

	connectToDatabase();
	
	//Gather parameters.
	int root_categories_number = ui->m_rootCategoriesNumberInput->value();
	int subcategories_number = ui->m_subCategoriesNumberInput->value();
	int recipes_number = ui->m_recipesNumInput->value();
	int recipe_authors_number = ui->m_recipeAuthorsNumInput->value();
	int standalone_authors_number = ui->m_standaloneAuthorsNumInput->value();
	QString rootCategoryNamePattern = ui->m_rootCategoryNamePattern->text();
	QString subcategoryNamePattern = ui->m_subcategoryNamePattern->text();
	QString recipeNamePattern = ui->m_recipeNamePattern->text();
	QString authorNamePattern = ui->m_recipeAuthorNamePattern->text();
	QString standaloneAuthorNamePattern = ui->m_standaloneAuthorNamePattern->text();


	//Write the data to the database.
	int i, j, k, l, progress;
	int last_id;

	ui->m_progressBar->setValue( 0 );

	database->transaction();

	QString categoryName;
	progress = 0;
	for ( i = 0; i < root_categories_number; i++ ) {
		categoryName = rootCategoryNamePattern + " " + QString::number( i );
		last_id = database->createNewCategory( categoryName );
		progress++;
		ui->m_progressBar->setValue( progress );
		Element category;
		category.id = last_id;
		category.name = categoryName;
		for ( j = 0; j < recipes_number; j++ ) {
			Recipe recipe;
			recipe.recipeID = -1; //This means it's a new recipe.
			recipe.title = recipeNamePattern + " "
				+ QString::number( i ) + " " + QString::number( j );
			recipe.categoryList << category;
			for (k = 0; k < recipe_authors_number; k++) {
				Element author;
				author.name = authorNamePattern + " "
					+ QString::number( i ) + " " + QString::number( j )
					+ " " + QString::number( k );
				author.id = database->createNewAuthor( author.name );
				recipe.authorList << author;
			}
			database->saveRecipe( &recipe );
			progress++;
			ui->m_progressBar->setValue( progress );
		}
		for ( j = 0; j < subcategories_number; j++ ) {
			categoryName = subcategoryNamePattern + " " 
				+ QString::number( i ) + " " + QString::number( j );
			Element subcategory;
			subcategory.id = database->createNewCategory( categoryName, last_id );
			subcategory.name = categoryName;
			progress++;
			ui->m_progressBar->setValue( progress );
			for ( k = 0; k < recipes_number; k++ ) {
				Recipe recipe;
				recipe.recipeID = -1; //This means it's a new recipe.
				recipe.title = recipeNamePattern + " "
					+ QString::number( i ) + " " + QString::number( j ) +
					+ " " + QString::number( k );
				recipe.categoryList << subcategory;
				for (l = 0; l < recipe_authors_number; l++) {
					Element author;
					author.name = authorNamePattern + " "
						+ QString::number( i ) + " " + QString::number( j )
						+ " " + QString::number( k ) + " " + QString::number( l );
					author.id = database->createNewAuthor( author.name );
					recipe.authorList << author;
				}
				database->saveRecipe( &recipe );
				progress++;
				ui->m_progressBar->setValue( progress );
			}
		}
	}

	QString authorName;
	for (k = 0; k < standalone_authors_number; k++ ) {
		authorName = standaloneAuthorNamePattern + " " + QString::number( k );
		database->createNewAuthor( authorName );
		progress++;
		ui->m_progressBar->setValue( progress );
	}

	database->commit();

}


void NukeConfigWidget::wipeDatabase()
{
	connectToDatabase();
	database->wipeDatabase();
}


void NukeConfigWidget::updateTotals( int value )
{
	Q_UNUSED(value)

        int root_categories_number = ui->m_rootCategoriesNumberInput->value();
        int subcategories_number = ui->m_subCategoriesNumberInput->value();
        int recipes_number = ui->m_recipesNumInput->value();
        int recipe_authors_number = ui->m_recipeAuthorsNumInput->value();
        int standalone_authors_number = ui->m_standaloneAuthorsNumInput->value();

	int total_categories = root_categories_number * subcategories_number
		+ root_categories_number;
	int total_recipes = total_categories * recipes_number;
	int total_authors = total_recipes * recipe_authors_number
		+ standalone_authors_number;

	ui->m_totalCategoriesLabel->setText( QString::number(total_categories) );
	ui->m_totalRecipesLabel->setText( QString::number(total_recipes) );
	ui->m_totalAuthorsLabel->setText( QString::number(total_authors) );
	ui->m_progressBar->setMaximum( total_categories + total_recipes
		+ standalone_authors_number );
}


#include "nukeconfigwidget.moc"
