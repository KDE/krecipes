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

#include <kdebug.h>
#include <KToolInvocation>
#include <QPointer>
#include <QSet>


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
	m_incompleteText = "";
	m_intermediateText = "";
	ui->m_textBrowser->setPlainText("");
}

void NutrientInfoDetailsDialog::addIncompleteText( const QString & text )
{
	m_incompleteText.append("<li class=\"ingredient_li\">");
	m_incompleteText.append(text);
	m_incompleteText.append("</li>");
}

void NutrientInfoDetailsDialog::addIntermediateText( const QString & text )
{
	m_intermediateText.append("<li class=\"ingredient_li\">");
	m_intermediateText.append(text);
	m_intermediateText.append("</li>");
}

void NutrientInfoDetailsDialog::displayText()
{
	QString finalText;

	//header
	finalText = "<html><style type=\"text/css\">"
		"p { margin-top: 0; margin-bottom: 0 }"
		".ingredient_li { margin-top: 4; margin-bottom: 4 }"
		"</style>";

	//incomplete text
	if ( !m_incompleteText.isEmpty() ) {
		finalText.append( i18nc("@info",
			"The nutrient information for this recipe is incomplete "
			"because the following information is missing:") );
		finalText.append("<ul>");
		finalText.append( m_incompleteText );
		finalText.append("</ul>");
	}

	//intermediate text
	if ( !m_intermediateText.isEmpty() ) {
		finalText.append( i18nc("@info",
			"The following approximations will be made when determining "
			"nutrient information:") );
		finalText.append("<ul>");
		finalText.append( m_intermediateText );
		finalText.append("</ul>");
	}

	//text ending
	finalText.append("</html>");

	//display the text in the viewer
	ui->m_textBrowser->setHtml( finalText );
}

NutrientInfo::Status NutrientInfoDetailsDialog::checkIngredientStatus(
	const Ingredient & ingredient, RecipeDB * database,
	QString * incompleteMessage, QString * intermediateMessage )
{
	//Clear error messages
	*incompleteMessage = "";
	*intermediateMessage = "";

	IngredientPropertyList ingPropertyList;
	database->loadProperties( &ingPropertyList, ingredient.ingredientID );

	if ( ingPropertyList.isEmpty() ) {
		*incompleteMessage = QString(
			i18nc("@info", "<b>%1:</b> No nutrient information available. "
			"<a href=\"ingredient#%2\">Provide nutrient information.</a>",
			Qt::escape( ingredient.name ),
			QString::number(ingredient.ingredientID)));
		return NutrientInfo::Incomplete;
	}

	//Find out the units used in "per amount" and store them in 'targetUnits'.
	//We will have to check that we can perform conversions from the ingredient unit
	//to these units, otherwise we won't be able to get the amounts of each nutritive
	//property for the ingredient.
	QSet<RecipeDB::IdType> perUnits_Ids;
	QList<Unit> targetUnits;
	IngredientPropertyList::const_iterator prop_it = ingPropertyList.constBegin();
	while (prop_it != ingPropertyList.constEnd() ) {
		if ( !perUnits_Ids.contains( prop_it->perUnit.id() ) ) {
			targetUnits << prop_it->perUnit;
			perUnits_Ids << prop_it->perUnit.id();
		}
		++prop_it;
	}

	//Check each unit we have to convert
	Ingredient dummyIngredient; //This is to store the result of the conversions
	RecipeDB::ConversionStatus status;
	QList<Unit>::const_iterator targetUnits_it = targetUnits.constBegin();
	while ( targetUnits_it != targetUnits.constEnd() ) {
		status = database->convertIngredientUnits( ingredient, *targetUnits_it, dummyIngredient );
		switch ( status ) {
		case RecipeDB::Success: break;
		case RecipeDB::MissingUnitConversion:
			if ( (ingredient.units.type() != Unit::Other) && (ingredient.units.type() == targetUnits_it->type()) ) {
//					propertyStatusMapRed.insert(ing.ingredientID,
//						i18nc( "@info", "<b>%3:</b> Unit conversion missing for conversion from '%1' to '%2'"
//						,(ing.units.name().isEmpty()?i18n("-No unit-"):ing.units.name())
//						,((*prop_it).perUnit.name())
//						,ing.name));
			} else {
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
			}
			break;
		case RecipeDB::MissingIngredientWeight:
//				propertyStatusMapRed.insert(ing.ingredientID, QString(
//					i18nc("@info", "<b>%1:</b> No ingredient weight entries. <a href=\"ingredient#%2\">Provide "
//					"ingredient weight.</a>",
//					ing.name, QString::number(ing.ingredientID))));
			break;
		case RecipeDB::MismatchedPrepMethod:
			if ( ingredient.prepMethodList.isEmpty() ) {
//					propertyStatusMapRed.insert(ing.ingredientID,QString(
//						i18nc("@info", "<b>%1:</b> There is no ingredient weight entry for when no "
//						"preparation method is specified. <a href=\"ingredient#%2\">Provide "
//						"ingredient weight.</a>",
//						ing.name, QString::number(ing.ingredientID))));
			} else {
//					propertyStatusMapRed.insert(ing.ingredientID,QString(
//						i18nc("@info", "<b>%1:</b> There is no ""ingredient weight entry for when prepared "
//						"in any of the following manners: %2<a href=\"ingredient#%3\">Provide "
//						"ingredient weight.</a>",
//						ing.name,
//						"<ul><li>"+ing.prepMethodList.join("</li><li>")+"</li></ul>",
//						QString::number(ing.ingredientID))));
			}
			break;
		case RecipeDB::MismatchedPrepMethodUsingApprox:
			intermediateMessage->append(
				i18nc("@info", "<p><b>%1:</b> There is no ingredient weight entry for when prepared in any of "
					"the following manners (defaulting to a weight entry without a preparation "
					"method specified):</p>"
					"%2<a href=\"ingredient#%3\">Provide ingredient weight.</a>")
					.arg( Qt::escape(ingredient.name) )
					.arg( "<ul><li>"+ingredient.prepMethodList.joinHtmlEscaped("</li><li>")+"</li></ul>" )
					.arg( QString::number(ingredient.ingredientID) )
			);
			break;
		default: kDebug()<<"Code error: Unhandled conversion status code "<<status; break;
		}
		++targetUnits_it;
	}

	//Return the status depending on what we found out above.
	if ( !incompleteMessage->isEmpty() ) {
		return NutrientInfo::Incomplete;
	} else if ( !intermediateMessage->isEmpty() ) {
		return NutrientInfo::Intermediate;
	} else {
		return NutrientInfo::Complete;
	}
}

QString NutrientInfoDetailsDialog::conversionPath( const QString &ingUnit,
	const QString &toUnit, const QString &fromUnit,
	const QString &propUnit ) const
{
	QString path = '\''+ingUnit+'\'';

	QString lastUnit = ingUnit;
	if ( lastUnit != toUnit ) {
		path.append(" =&gt; '"+toUnit+'\'');
		lastUnit = toUnit;
	}
	if ( lastUnit != fromUnit ) {
		path.append(" =&gt; '"+fromUnit+'\'');
		lastUnit = fromUnit;
	}
	if ( lastUnit != propUnit ) {
		path.append(" =&gt; '"+propUnit+'\'');
		lastUnit = propUnit;
	}
	return path;
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

