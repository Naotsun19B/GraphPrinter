// Copyright 2020-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace GraphPrinter
{
	/**
	 * A bool type that can change the value only once from the default value.
	 */
	struct FOneWayBool
	{
	public:
		FOneWayBool(bool bInDefaultValue)
			: bDefaultValue(bInDefaultValue)
			, bValue(bInDefaultValue)
		{
		}

		bool GetValue() const
		{
			return bValue;
		}
		
		void Switch()
		{
			if (bDefaultValue == bValue)
			{
				bValue = !bValue;
			}
		}
		
	private:
		bool bDefaultValue;
		bool bValue;
	};
}
