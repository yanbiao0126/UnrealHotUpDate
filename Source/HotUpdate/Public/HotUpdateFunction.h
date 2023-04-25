// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HotUpdateFunction.generated.h"

/**
 * 
 */
UCLASS()
class HOTUPDATE_API UHotUpdateFunction : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	// 获取文件md5
	UFUNCTION(BlueprintCallable, Category = "HotUpdate", meta=(DisplayName="计算md5"))
	static FString GetFileMd5(FString filePath);
};
