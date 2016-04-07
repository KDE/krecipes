template<T> class KreAllElementsModels {
	public:
		KreGenericModel( RecipeDB * database );

		QStringListModel * nameModel();
		QSortFilterProxyModel * nameModelSorted();
		KCompletion * nameCompletion();

		QList<QVariant> idsForName( const QString & name );

	protected:
		QHash<QVariant,T> m_idToElementMap;
		QMultiHash<QString,QVariant> m_nameToIdMap;

		QStringListModel * m_nameModel;
		QSortFilterProxyModel * m_nameModelSorted;
		KCompletion * m_nameCompletion;
}


class KreAllIngredientsModels: public KreGenericModels<KreIngredient> {
	public:
		KreIngredientModel( RecipeDB * database );

}

class KreAllHeadersModels

class KreAllUnitsModels: public KreAllElementsModels {
	public:
		KreAllUnitsModels( RecipeDB * database );

		QStringListModel * pluralNameModel();
		QSortFilterProxyModel * pluralNameModelSorted();
		KCompletion * pluralNameCompletion();
		
		QList<QVariant> idsForPlural( const QString & pluralName );

	protected:
		QMultiHash<QString,QVariant> m_pluralNameToIdMap;

		QStringListModel * m_pluralNameModel;
		QSortFilterProxyModel * m_pluralNameModelSorted;
		KCompletion * m_pluralNameCompletion;
}

class KreAllMassUnitConversionsModel
class KreAllVolumeConversionsModel

class KreAllPrepMethodsModels

class KreAllPropertiesModels

class KreAllAuthorsModels

class KreRecipeTreeModel .... //would store the categories/recipe tree

//Model to get the set of properties for a given ingredient
class KrePropertiesForIngredientModel {
	public:
		KrePropertiesForIngredientModel( RecipeDB * database.
			const QVariant & ingredientId );

		...
	private:
		QHash<QVariant,KreIngredientPropertyEntry> m_propertyIdToEntryMap;
}

//Similar ro KrePropertiesForIngredientModel
class KreWeightsForIngredientModel

class KreRecipeIngredientsEditorModel {
	...
	private:
		QHash<QVariant,KrePropertiesForIngredientModel> m_ingredientIdToPropertiesMap;
}
