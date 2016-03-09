/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Cyril Bosselut <bosselut@b1project.com>         *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipegeneralinfoeditor.h"
#include "ui_recipegeneralinfoeditor.h"

#include "dialogs/recipeinput/selectauthorsdialog.h"
#include "selectcategoriesdialog.h"
#include "datablocks/recipe.h"

#include <KFileDialog>
#include <KMessageBox>
#include <kio/netaccess.h>


RecipeGeneralInfoEditor::RecipeGeneralInfoEditor( QWidget * parent, RecipeDB * db ):
	QWidget( parent ),
	m_recipe( 0 ),
	m_database( db )
{
	//Set up GUI
	ui = new Ui::RecipeGeneralInfoEditor;
	ui->setupUi( this );

	//Connect signals/slots to detect changes
	connect( ui->m_titleEdit, SIGNAL(textChanged(const QString&)),
		this, SLOT(titleChangedSlot(const QString&)) );

	connect( ui->m_photoLabel, SIGNAL(changed()),
		this, SIGNAL(changed()) );

	connect( ui->m_yieldNumInput, SIGNAL( textChanged(const QString &) ),
		this, SIGNAL(changed()) );
	connect( ui->m_yieldTypeEdit, SIGNAL( textChanged(const QString &) ),
		this, SIGNAL(changed()) );

	connect( ui->m_prepTimeEdit, SIGNAL( timeChanged(const QTime &) ),
		this, SIGNAL(changed()) );

	//Connect signals/slots to perform changes
	connect( ui->m_changePhotoButton, SIGNAL(clicked()),
		this, SLOT(changePhotoSlot()) );

	connect( ui->m_savePhotoAsButton, SIGNAL(clicked()),
		this, SLOT(savePhotoAsSlot()) );

	connect( ui->m_clearPhotoButton, SIGNAL(clicked()),
		this, SLOT(clearPhotoSlot()) );

	connect( ui->m_editAuthorsButton, SIGNAL(clicked()),
		this, SLOT(editAuthorsSlot()) );

	connect( ui->m_editCategoriesButton, SIGNAL(clicked()),
		this, SLOT(editCategoriesSlot()) );
}

void RecipeGeneralInfoEditor::loadRecipe( Recipe * recipe )
{
	m_recipe = recipe;

	//Set title in the GUI
	ui->m_titleEdit->setText( recipe->title );

	Q_INIT_RESOURCE(resources);
	//Display photo in the GUI
	ui->m_photoLabel->setPhoto( &recipe->photo );
	if ( recipe->photo.isNull() ) {
		ui->m_photoLabel->setPixmap(QPixmap(QString::fromUtf8(":/default_recipe_photo.png")));
	} else {
		ui->m_photoLabel->refresh();
	}

	//Display authors in the GUI
	showAuthors();

	//Display categories in the GUI
	showCategories();

	//Display yield in the GUI
	ui->m_yieldNumInput->setValue( m_recipe->yield.amount(), m_recipe->yield.amountOffset() );
	ui->m_yieldTypeEdit->setText( m_recipe->yield.type() );

	//Display preparation time in the GUI
	ui->m_prepTimeEdit->setTime( m_recipe->prepTime );
}

void RecipeGeneralInfoEditor::updateRecipe()
{
	//Update the edited recipe with the stuff which wasn't saved before
	m_recipe->title = ui->m_titleEdit->text();
	double amount = 0.0, amountOffset = 0.0;
	ui->m_yieldNumInput->value(amount, amountOffset);
	m_recipe->yield.setAmount(amount);
	m_recipe->yield.setAmountOffset(amountOffset);
	m_recipe->yield.setTypeId(createNewYieldIfNecessary(ui->m_yieldTypeEdit->text()));
	m_recipe->prepTime = ui->m_prepTimeEdit->time();
}

int RecipeGeneralInfoEditor::createNewYieldIfNecessary( const QString &yield )
{
	if ( yield.trimmed().isEmpty() ) { //no yield
		return RecipeDB::InvalidId;
	} else {
		int id = m_database->findExistingYieldTypeByName( yield );
		if ( id == RecipeDB::InvalidId ) { //creating new
			id = m_database->createNewYieldType( yield );
		}
		return id;
	}
}

void RecipeGeneralInfoEditor::titleChangedSlot( const QString & title )
{
	emit titleChanged( title );
}

void RecipeGeneralInfoEditor::changePhotoSlot()
{
	// Get the image with a standard filedialog
	KUrl url = KFileDialog::getOpenUrl( KUrl() , QString(
		"*.png *.jpg *.jpeg *.xpm *.gif|%1 (*.png *.jpg *.jpeg *.xpm *.gif)"
		).arg( i18n( "Images" ) ), this );
	if ( url.isEmpty() )
		return;
	QString filename;
	if (!url.isLocalFile()) {
		if (!KIO::NetAccess::download(url,filename,this)) {
			KMessageBox::error(this, KIO::NetAccess::lastErrorString() );
			return;
		}
	} else {
		filename = url.path();
	}

	//Set the image in the GUI
	QPixmap pixmap ( filename );
	if ( !pixmap.isNull() ) {
		m_recipe->photo = pixmap;
		ui->m_photoLabel->setPhoto( &(m_recipe->photo) );
		ui->m_photoLabel->refresh();
		emit changed();
	}

	//Delete temp file
	if (!url.isLocalFile()) {
		KIO::NetAccess::removeTempFile( filename );
	}
}

void RecipeGeneralInfoEditor::savePhotoAsSlot()
{

	KUrl url = KFileDialog::getSaveUrl( KUrl(),
		QString( "*.jpg *.jpeg |%1 (*.jpg *.jpeg)" ).arg( i18n( "Images" ) ),
		this,
		i18nc("@title:window", "Save photo as..."));

	if ( url.isEmpty() )
		return;

	QString filename = url.path();
	QFile outputFile (filename);
	if (outputFile.exists()) {
		int r = KMessageBox::warningYesNo(this,
			i18nc("@info", "The file already exists, do you want to overwrite it?")
		);
		if (r == KMessageBox::No)
			return;
	}

	if ( !ui->m_photoLabel->pixmap()->save( filename, "JPEG" ) ) {
		KMessageBox::error(this,
			i18nc("@info", "The photo cannot be saved in %1", filename) );
	}
}

void RecipeGeneralInfoEditor::clearPhotoSlot()
{
	m_recipe->photo = QPixmap();
	ui->m_photoLabel->setPixmap(QPixmap(QString::fromUtf8(":/default_recipe_photo.png")));
	emit changed();
}

void RecipeGeneralInfoEditor::editAuthorsSlot()
{
	QPointer<SelectAuthorsDialog> editAuthorsDialog =
		new SelectAuthorsDialog( this, m_recipe->authorList, m_database );

	if ( editAuthorsDialog->exec() == QDialog::Accepted ) { // user presses Ok
		m_recipe->authorList.clear();
		editAuthorsDialog->getSelectedAuthors( &m_recipe->authorList );
		emit( changed() ); //Indicate that the recipe changed
	}

	delete editAuthorsDialog;

	// show authors list
	showAuthors();
}

void RecipeGeneralInfoEditor::showAuthors()
{
	QString authors;
	ElementList::const_iterator it;
	for ( it = m_recipe->authorList.constBegin(); it != m_recipe->authorList.constEnd(); ++it ) {
		if ( !authors.isEmpty() )
			authors += ", ";
		authors += it->name;
	}
	ui->m_authorsDisplay->setText( authors );
}

void RecipeGeneralInfoEditor::editCategoriesSlot()
{
	QPointer<SelectCategoriesDialog> editCategoriesDialog =
		new SelectCategoriesDialog( this, m_recipe->categoryList, m_database );

	if ( editCategoriesDialog->exec() == QDialog::Accepted ) { // user presses Ok
		m_recipe->categoryList.clear();
		editCategoriesDialog->getSelectedCategories( &m_recipe->categoryList );
		emit( changed() ); //Indicate that the recipe changed
	}

	delete editCategoriesDialog;

	// show category list
	showCategories();
}

void RecipeGeneralInfoEditor::showCategories()
{
	QString categories;
	ElementList::const_iterator it;
	for ( it = m_recipe->categoryList.constBegin(); it != m_recipe->categoryList.constEnd(); ++it ) {
		if ( !categories.isEmpty() )
			categories += ", ";
		categories += it->name;
	}
	ui->m_categoriesDisplay->setText( categories );
}

