// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "AsyncNetWork.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FHttp_Progress, int32 , I, int32, Arg);
DECLARE_DYNAMIC_DELEGATE_OneParam(FHttp_Complete, bool, bArg);
/**
 * 
 */
UCLASS()
class HOTUPDATE_API UAsyncNetWork : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	UAsyncNetWork();
public:
	FHttp_Progress OnProgress;
	FHttp_Complete OnComplete;
	UFUNCTION(BlueprintCallable, Category="AsnycNetWork", meta=(BlueprintInternalUseOnly="true"))
	static UAsyncNetWork* AsyncHttpDownload(FString URL, FString SavePath);

	void OnRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg);
	void OnRequestProgress(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, int32 BytesSent, int32 BytesReceived);
	void Start(const FString URL, const FString SavePath);

	FString FileSavePath;
};
