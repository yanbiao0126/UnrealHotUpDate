// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncNetWork.h"
#include "HttpModule.h"

UAsyncNetWork::UAsyncNetWork()
{
	// 不准GC回收
	AddToRoot();
}

UAsyncNetWork* UAsyncNetWork::AsyncHttpDownload(FString URL, FString SavePath)
{
	UAsyncNetWork* AsyncHttpObject = NewObject<UAsyncNetWork>();
	AsyncHttpObject->Start(URL, SavePath);
	return AsyncHttpObject;
}

void UAsyncNetWork::OnRequestComplete(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (bArg && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		IFileHandle* FileHandle = PlatformFile.OpenWrite(*FileSavePath);
		if (FileHandle)
		{
			FileHandle->Write(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength());
			delete FileHandle;
			UE_LOG(LogTemp, Warning, TEXT("下载成功"));
			OnComplete.ExecuteIfBound(true);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("下载失败"));
		}
		OnComplete.ExecuteIfBound(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("下载失败"));
	}
	// GC回收
	RemoveFromRoot();
}

void UAsyncNetWork::OnRequestProgress(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	if (HttpRequest->GetResponse()->GetContentLength() > 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("下载进度:%f"), (float)BytesSent / (float)BytesReceived);
		OnProgress.ExecuteIfBound(BytesSent, BytesReceived);
	}
}

void UAsyncNetWork::Start(const FString URL, const FString SavePath)
{
	FileSavePath = SavePath;
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetVerb("GET");
	request->SetURL(URL);
	request->OnProcessRequestComplete().BindUObject(this, &UAsyncNetWork::OnRequestComplete);
	request->OnRequestProgress().BindUObject(this, &UAsyncNetWork::OnRequestProgress);
	request->ProcessRequest();
}
