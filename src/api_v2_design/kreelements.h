class KreElement {
	public:
		KreElement();
		KreElement( const QVariant & id, const QString & name );

		QVariant id() const;
		void setId( const QVariant & id );

		QString name() const;
		void setName( const QString & name );

	private:
		QVariant id;
		QVariant name;
};

class KreIngredient: public KreElement {
	public:
		KreIngredient();
		KreIngredient( const QVariant & id, const QString & name );
}

class KreIngHeader: public KreElement {
	public
		KreIngHeader();
		KreIngHeader( const QVariant & id, const QString & name );
}

class KreUnit: public KreElement {
	...
}

class KreIngredientPropertyEntry {
	public:
		KrePropertyEntry();

		QVariant propertyId();
		void setPropertyId( const QVariant & propertyId );

		double amount();
		void setAmount( double amount );

		QVariant unitId()
		void setUnitId( const QVariant & unitId );

	private:
		...
}

class KreRecipeIngredientEntry {
	public:
		KreIngredientEntry();
		KreIngredientEntry( const KreIngredient & ingredient,
			const KreMixedNumberRange & amount,
			const KreUnit & unit,
			const QList<KrePrepMethod> & prepMethods,
			const QList<KreIngredient> & substitutesList = QList<KreIngredient>(),
			const KreIngHeader & ingHeader = KreIngHeader() );
	...
}

class KreRecipe: public KreElement {
	public:
		...
		QList<KreIngredientEntry> ingredientEntryList() const;
		void addIngredientEntry( const KreIngredientEntry & ingredientEntry );
		...

	private:
		...
		QList<KreIngredientEntry> m_ingredientEntryList;
		...
}
