/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "nutrientinfodetailsdialog.h"

#include "ui_nutrientinfodetailsdialog.h"

#include "dialogs/editpropertiesdialog.h"
#include "dialogs/createunitconversiondialog.h"
#include "backends/recipedb.h"

#include <KToolInvocation>
#include <QPointer>


NutrientInfoDetailsDialog::NutrientInfoDetailsDialog( QWidget *parent )
		: QDialog( parent ),
		m_database( 0 )
{
	ui = new Ui::NutrientInfoDetailsDialog;
	QWidget * mainWidget = new QWidget( this );
	ui->setupUi( mainWidget );
	QGridLayout * mainLayout = new QGridLayout;
	setLayout( mainLayout );
	mainLayout->addWidget( mainWidget );

	connect( ui->m_textBrowser, SIGNAL(anchorClicked(const QUrl &)),
		this, SLOT(linkClickedSlot(const QUrl &)) );

	connect( ui->m_updateButton, SIGNAL(clicked()),
		this, SIGNAL(updateRequested()) );

	connect( ui->m_closeButton, SIGNAL(clicked()),
		this, SLOT(hide()) );

	connect( ui->m_helpButton, SIGNAL(clicked()),
		this, SLOT(helpButtonClickedSlot()) );
}

void NutrientInfoDetailsDialog::setDatabase( RecipeDB * database )
{
	m_database = database;
}

void NutrientInfoDetailsDialog::clear()
{
	m_text = "";
	m_text.append( i18nc("@info",
		"The nutrient information for this recipe is incomplete "
		"because the following information is missing:") );
	m_text.append("<ul>");
	ui->m_textBrowser->setPlainText("");
}

void NutrientInfoDetailsDialog::addText( const QString & text )
{
	m_text.append("<li>");
	m_text.append(text);
	m_text.append("</li>");
}

void NutrientInfoDetailsDialog::displayText()
{
	m_text.append("</ul>");
	ui->m_textBrowser->setText( m_text );
}

NutrientInfo::Status NutrientInfoDetailsDialog::checkIngredientStatus(
	const Ingredient & ingredient, RecipeDB * database, QString * message )
{
	//Clear error message
	*message = "";

	IngredientPropertyList ingPropertyList;
	database->loadProperties( &ingPropertyList, ingredient.ingredientID );

	if ( ingPropertyList.isEmpty() ) {
		*message = QString(
			i18nc("@info", "<b>%1:</b> No nutrient information available. "
			"<a href=\"ingredient#%2\">Provide nutrient information.</a>",
			Qt::escape( ingredient.name ),
			QString::number(ingredient.ingredientID)));
		return NutrientInfo::Incomplete;
	}

//	QMap<int,bool> ratioCache; //unit->conversion possible
//	IngredientPropertyList::const_iterator prop_it;
//	for ( prop_it = ingPropertyList.constBegin(); prop_it != ingPropertyList.constEnd(); ++prop_it ) {
//		Ingredient result;
//
//		QMap<int,bool>::const_iterator cache_it = ratioCache.constFind((*prop_it).perUnit.id());
//		if ( cache_it == ratioCache.constEnd() ) {
//			RecipeDB::ConversionStatus status = database->convertIngredientUnits( ing, (*prop_it).perUnit, result );
//			ratioCache.insert((*prop_it).perUnit.id(),status==RecipeDB::Success||status==RecipeDB::MismatchedPrepMethod);
//
//			switch ( status ) {
//			case RecipeDB::Success: break;
//			case RecipeDB::MissingUnitConversion: {
//				if ( ing.units.type() != Unit::Other && ing.units.type() == (*prop_it).perUnit.type() ) {
//					propertyStatusMapRed.insert(ing.ingredientID,
//						i18nc( "@info", "<b>%3:</b> Unit conversion missing for conversion from '%1' to '%2'"
//						,(ing.units.name().isEmpty()?i18n("-No unit-"):ing.units.name())
//						,((*prop_it).perUnit.name())
//						,ing.name));
//				} else {
//					WeightList weights = database->ingredientWeightUnits( ing.ingredientID );
//					Q3ValueList< QPair<int,int> > usedIds;
//					QStringList missingConversions;
//					for ( WeightList::const_iterator weight_it = weights.constBegin(); weight_it != weights.constEnd(); ++weight_it ) {
//						//skip entries that only differ in how it's prepared
//						QPair<int,int> usedPair((*weight_it).perAmountUnitId(),(*weight_it).weightUnitId());
//						if ( usedIds.find(usedPair) != usedIds.end() )
//							continue;
//
//						QString toUnit = database->unitName((*weight_it).perAmountUnitId()).name();
//						if ( toUnit.isEmpty() ) toUnit = i18nc( "@info", "-No unit-");
//
//						QString fromUnit = database->unitName((*weight_it).weightUnitId()).name();
//						if ( fromUnit.isEmpty() ) fromUnit = i18nc( "@info", "-No unit-");
//
//						QString ingUnit = ing.units.name();
//						if ( ingUnit.isEmpty() ) ingUnit = i18nc( "@info", "-No unit-");
//
//						QString propUnit = (*prop_it).perUnit.name();
//						if ( propUnit.isEmpty() ) propUnit = i18nc( "@info", "-No unit-");
//
//						missingConversions << conversionPath( ingUnit, toUnit, fromUnit, propUnit);
//					}
//					propertyStatusMapRed.insert(ing.ingredientID,
//						i18nc("@info", "<b>%1:</b> Either <a href=\"ingredient#%3\">enter an appropriate "
//						"ingredient weight entry</a>, or provide conversion information to "
//						"perform one of the following conversions: %2",
//					  	ing.name,
//						("<ul><li>"+missingConversions.join("</li><li>")+"</li></ul>"),
//						QString::number(ing.ingredientID))
//					);
//				}
//				break;
//			}
//			case RecipeDB::MissingIngredientWeight:
//				propertyStatusMapRed.insert(ing.ingredientID, QString(
//					i18nc("@info", "<b>%1:</b> No ingredient weight entries. <a href=\"ingredient#%2\">Provide "
//					"ingredient weight.</a>",
//					ing.name, QString::number(ing.ingredientID))));
//				break;
//			case RecipeDB::MismatchedPrepMethod:
//				if ( ing.prepMethodList.count() == 0 )
//					propertyStatusMapRed.insert(ing.ingredientID,QString(
//						i18nc("@info", "<b>%1:</b> There is no ingredient weight entry for when no "
//						"preparation method is specified. <a href=\"ingredient#%2\">Provide "
//						"ingredient weight.</a>",
//						ing.name, QString::number(ing.ingredientID))));
//				else
//					propertyStatusMapRed.insert(ing.ingredientID,QString(
//						i18nc("@info", "<b>%1:</b> There is no ""ingredient weight entry for when prepared "
//						"in any of the following manners: %2<a href=\"ingredient#%3\">Provide "
//						"ingredient weight.</a>",
//						ing.name,
//						"<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>",
//						QString::number(ing.ingredientID))));
//				break;
//			case RecipeDB::MismatchedPrepMethodUsingApprox:
//				propertyStatusMapYellow.insert(ing.ingredientID,QString(
//					i18nc("@info", "<b>%1:</b> There is no ingredient weight entry for when prepared in any of "
//					"the following manners (defaulting to a weight entry without a preparation "
//					"method specified): "
//					"%2<a href=\"ingredient#%3\">Provide ingredient weight.</a>",
//					ing.name,
//					"<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>",
//					QString::number(ing.ingredientID))));
//				break;
//			default: kDebug()<<"Code error: Unhandled conversion status code "<<status; break;
//			}
//		}
//	}

	return NutrientInfo::Complete;
}

void NutrientInfoDetailsDialog::helpButtonClickedSlot()
{
	KToolInvocation::invokeHelp("property-status");
}

void NutrientInfoDetailsDialog::linkClickedSlot( const QUrl & link )
{
	if ( !m_database ) {
		return;
	}

	QString linkString = link.toString();
	if (linkString.startsWith("ingredient#")) {
		RecipeDB::IdType ingID = linkString.mid(linkString.indexOf("#")+1).toInt();
		QString ingName = m_database->ingredientName(ingID);
		QPointer<EditPropertiesDialog> d = new EditPropertiesDialog( ingID, ingName, m_database, this );
		d->exec();
		delete d;
	} else if (linkString.startsWith("unit#")) { //FIXME: Not used?
		QString unitIDs = linkString.mid(linkString.indexOf("#")+1);
		QStringList idList = unitIDs.split(',', QString::SkipEmptyParts );
		RecipeDB::IdType unitFrom = idList[0].toInt();
		ElementList toUnits;
		RecipeDB::IdType lastUnit = RecipeDB::InvalidId;
		for (int i = 1; i < idList.count(); ++i ) {
			int id = idList[i].toInt();
			if ( id != lastUnit ) {
				toUnits << Element(m_database->unitName(id).name(),id);
				lastUnit = id;
			}
		}
		QPointer<CreateUnitConversionDialog> dlg =
			new CreateUnitConversionDialog( Element(m_database->unitName(unitFrom).name()), toUnits, this );
		if ( dlg->exec() == QDialog::Accepted ) {
			UnitRatio ratio(dlg->toUnitID(), unitFrom, dlg->ratio());
			if (ratio.ratio() >= 0 ) {
				m_database->saveUnitRatio(&ratio);
			}
		}
		delete dlg;
	}
}

