// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "GameFramework/GameModeBase.h"
#include "AsyncNetWork.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "HotGameModeBase.generated.h"

// 动态委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpDateEnd);

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
	
	// UPROPERTY(BlueprintAssignable, Category = "HotUpdate", meta=(DisplayName="更新游戏内容"))
	// FUpDateGame OnUpDateGame;

	// 更新结束
	UPROPERTY(BlueprintAssignable, Category = "HotUpdate", meta=(DisplayName="更新结束"))
	FUpDateEnd OnUpDateEnd;

	// 服务器json路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotUpdate")
	FString ServerURL="http://199.255.99.237/ue";

	// 版本号
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HotUpdate", meta=(DisplayName="游戏版本号"))
	float GameVersion=1.0;

	// 更新版本号
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="更新版本号"))
	FString GameNewVersion;

	// 版本更新信息
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="版本更新信息"))
	FString GameVersionDescribe;

	// 游戏更新列表
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="游戏更新列表"))
	TArray<FGameDataList> GameDataList;

	// 下载更新文件
	UFUNCTION(BlueprintCallable, Category = "HotUpdate", meta=(DisplayName="下载更新文件"))
	void DownLoadGameFile(int32 i=0);

	// 校验md5
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HotUpdate", meta=(DisplayName="校验md5"))
	bool CheckMd5(FString filePath, FString md5);
	
	// 需要下载的版本号
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="需要下载的版本号"))
	TArray<FString> NeedDownLoadVersion;

	// 下载完数量
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="下载完数量"))
	int32 DownLoadCompleteNum=0;

	
	void HotComplete(bool result);

	void HotDownloadProgress(int32 BytesReceived, int32 ContentLength);

	// 文件长度
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="文件长度"))
	int32 FileLength=0;
	
	// 当前下载进度
	UPROPERTY(BlueprintReadOnly, Category = "HotUpdate", meta=(DisplayName="当前下载进度"))
	int32 CurrentProgress=0;
	
protected:
	void OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	// 获取服务器json文件
	void GetServerJson();
};
