	jvxErrorType component_class(jvxComponentType tp, jvxComponentTypeClass& tpCls) override
	{
		return this->_component_class(tp, tpCls);
	};

	jvxErrorType add_type_host(jvxComponentType* tp, jvxSize numtp, const char* description, const char* tokenConfig, 
		jvxComponentTypeClass classType, 
		jvxSize numSlotsMax, jvxSize numSubslotsMax, jvxComponentTypeClass childClassType) override
	{
		return this->_add_type_host(static_cast<IjvxObject*>(this), tp, numtp, description, tokenConfig, 
			classType, numSlotsMax, numSubslotsMax, childClassType);
	};

	jvxErrorType number_types_host(jvxSize* num, jvxComponentTypeClass classType)override
	{
		return this->_number_types_host(num, classType);
	};

	jvxErrorType description_type_host(jvxSize idx, jvxApiString* theDescription, jvxApiString* theTokenInConfig, jvxComponentType* tp, jvxSize numtp, jvxComponentTypeClass classType)override
	{
		return this->_description_type_host(idx, theDescription, theTokenInConfig, tp, numtp, classType);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION remove_all_host_types()override
	{
		return this->_remove_all_host_types();
	};