// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GraphPrinterUtils.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHPRINTER_API UGraphPrinterUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GraphPrinter")
	static void PrintGraph();
};
