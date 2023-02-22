// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HotGameModeBase.generated.h"

// 动态委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpDateGame, bool, isUpdate);

USTRUCT(BlueprintType)
struct FGameDataList
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate")
	FString assetName;
	
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate")
	FString assetDescribe;

	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate")
	FString assetMd5;

	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate")
	FString assetUrl;
	
};

/**
 * 
 */
UCLASS()
class HOTUPDATE_API AHotGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void StartPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "HotUpdate", meta=(DisplayName="更新游戏内容"))
	FUpDateGame OnUpDateGame;

	// 服务器json路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotUpdate")
	FString ServerURL="http://led226.com/ue";

	// 版本号
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="游戏版本号"))
	FString GameVersion="1.0.0";

	// 更新版本号
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="更新版本号"))
	FString GameNewVersion;

	// 版本更新信息
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="版本更新信息"))
	FString GameVersionDescribe;

	// 游戏更新列表
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="游戏更新列表"))
	TArray<FGameDataList> GameDataList;
	
protected:
	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	// 获取服务器json文件
	void GetServerJson();
};
