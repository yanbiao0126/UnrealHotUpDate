// Fill out your copyright notice in the Description page of Project Settings.

#include "HotGameModeBase.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "FileToStorageDownloader.h"
#include "HotUpdateFunction.h"

void AHotGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetServerJson();
}

AHotGameModeBase::AHotGameModeBase()
{
	OnComplete.BindDynamic(this, &AHotGameModeBase::HotComplete);
	OnDownloadProgress.BindDynamic(this, &AHotGameModeBase::HotDownloadProgress);
}

void AHotGameModeBase::DownLoadGameFile(int32 i)
{
	// 判断是否下载完毕
	if (i >= GameDataList.Num())
	{
		if (DownLoadCompleteNum == GameDataList.Num())
		{
			// 更新结束
			OnUpDateEnd.Broadcast();
		}
		return;
	};
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FGameDataList data = GameDataList[i];
	// pakURL
	FString pakURL = ServerURL + TEXT("/") + data.assetUrl;
	// 获取pak目录
	FString pakPath = FPaths::ProjectContentDir().Append(TEXT("Paks/")).Append(data.assetName);
	UE_LOG(LogTemp, Warning, TEXT("pakPath:%s     urlPak:%s"), *pakPath, *pakURL);
	if (PlatformFile.FileExists(*pakPath))
	{
		if (!CheckMd5(pakPath, data.assetMd5))
		{
			UE_LOG(LogTemp, Warning, TEXT("pakPath:%s"), *pakPath);
			// 删除文件
			PlatformFile.DeleteFile(*pakPath);
		}
		else
		{
			DownLoadCompleteNum++;
			// 下载下一个文件
			DownLoadGameFile(i + 1);
			return;
		}
	}

	// 下载文件
	UFileToStorageDownloader::DownloadFileToStorage(pakURL, pakPath, 0, TEXT(""), OnDownloadProgress, OnComplete);
	// 下载下一个文件
	DownLoadGameFile(i + 1);
}

bool AHotGameModeBase::CheckMd5(FString filePath, FString md5)
{
	return UHotUpdateFunction::GetFileMd5(filePath).Equals(md5);
}

void AHotGameModeBase::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest,
																					TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
{
	if (bArg && HttpResponse.IsValid())
	{
		if (HttpResponse->GetResponseCode() == 200)
		{
			FString JsonStr = HttpResponse->GetContentAsString();
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonStr);
			TSharedPtr<FJsonObject> RootJsonObj = MakeShareable(new FJsonObject);
			if (FJsonSerializer::Deserialize(JsonReader, RootJsonObj))
			{
				bool isRun = RootJsonObj->GetBoolField(TEXT("isRenew"));
				if (!isRun) return;
				GameNewVersion = RootJsonObj->GetStringField(TEXT("version"));
				// if (GameVersion >= FCString::Atod(*GameNewVersion)) return;
				TSharedPtr<FJsonObject> versionMap = RootJsonObj->GetObjectField(TEXT("versionMap"));
				// TSharedPtr<FJsonObject> VersionData = versionMap->GetObjectField(GameNewVersion);
				// 获取json的key
				TArray<FString> keys;
				versionMap->Values.GetKeys(keys);
				for (auto key : keys)
				{
					TArray<FGameDataList> GameList;
					if (FCString::Atof(*key) <= FCString::Atof(*GameNewVersion))
					{
						NeedDownLoadVersion.Add(key);
						TSharedPtr<FJsonObject> VersionData = versionMap->GetObjectField(key);
						GameVersionDescribe = VersionData->GetStringField(TEXT("describe"));
						TSharedPtr<FJsonObject> AssetsList = VersionData->GetObjectField(TEXT("assetsList"));
						TArray<TSharedPtr<FJsonValue>> DataList = AssetsList->GetArrayField(TEXT("Game"));
						FJsonObjectConverter::JsonArrayToUStruct(DataList, &GameList, 0, 0);
						GameDataList.Append(GameList);
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("JsonStr: %s"), *JsonStr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HttpResponse is not valid"));
	}
	DownLoadGameFile(0);
}

void AHotGameModeBase::GetServerJson()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetVerb("GET");
	UE_LOG(LogTemp, Display, TEXT("ServerURL: %s/BuildManifest.json"), *ServerURL);
	request->SetURL(ServerURL + "/BuildManifest.json");
	request->OnProcessRequestComplete().BindUObject(this, &AHotGameModeBase::OnResponseReceived);
	request->ProcessRequest();
}

void AHotGameModeBase::HotComplete(EDownloadToStorageResult result)
{
	UE_LOG(LogTemp, Warning, TEXT("HotComplete"));
	DownLoadCompleteNum++;
	HotDownloadComplete();
}

void AHotGameModeBase::HotDownloadProgress(int32 BytesReceived, int32 ContentLength)
{
	UE_LOG(LogTemp, Warning, TEXT("HotDownloadProgress: %d/%d"), BytesReceived, ContentLength);
	CurrentProgress = BytesReceived;
	FileLength = ContentLength;
}

void AHotGameModeBase::HotDownloadComplete()
{
	if (DownLoadCompleteNum == GameDataList.Num())
	{
		// 更新结束
		OnUpDateEnd.Broadcast();
	}
}
