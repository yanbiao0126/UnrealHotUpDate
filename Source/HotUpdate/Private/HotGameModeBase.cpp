// Fill out your copyright notice in the Description page of Project Settings.

#include "HotGameModeBase.h"
#include "JsonObjectConverter.h"
#include "HotUpdateFunction.h"
#include "HttpModule.h"

void AHotGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetServerJson();
}

void AHotGameModeBase::DownLoadGameFile()
{
	// 判断是否下载完毕
	if (DownLoadCompleteNum == GameDataList.Num())
	{
		// 更新结束
		OnUpDateEnd.Broadcast();
		return;
	}
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FGameDataList data = GameDataList[DownLoadCompleteNum];
	// pakURL
	FString pakURL = ServerURL + TEXT("/") + data.assetUrl;
	// 获取pak目录
	FString pakPath = FPaths::ProjectContentDir().Append(TEXT("Paks/")).Append(data.assetName);
	UE_LOG(LogTemp, Warning, TEXT("pakPath:%s     urlPak:%s"), *pakPath, *pakURL);
	FString Path, Filename, Extension; 
	FPaths::Split(pakPath, Path, Filename, Extension);
	// 判断目录是否存在
	if (!PlatformFile.DirectoryExists(*Path))
	{
		PlatformFile.CreateDirectory(*Path);
	}
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
			// 下载下一个文件
			DownLoadCompleteNum++;
			// DownLoadGameFile(DownLoadCompleteNum);
			// return;
		}
	}
	// 下载文件
	UAsyncNetWork* asyncNetWork = UAsyncNetWork::AsyncHttpDownload(pakURL, pakPath);
	asyncNetWork->OnProgress.BindDynamic(this, &AHotGameModeBase::HotDownloadProgress);
	asyncNetWork->OnComplete.BindDynamic(this, &AHotGameModeBase::HotComplete);
	// UFileToStorageDownloader::DownloadFileToStorage(pakURL, pakPath, 0, TEXT(""), OnDownloadProgress, OnComplete);
	// 下载文件
	// DownLoadGameFile();
}

bool AHotGameModeBase::CheckMd5(FString filePath, FString md5)
{
	return UHotUpdateFunction::GetFileMd5(filePath).Equals(md5);
}

void AHotGameModeBase::OnResponseReceived(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> HttpResponse, bool bArg)
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
	DownLoadGameFile();
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

void AHotGameModeBase::HotComplete(bool result)
{
	UE_LOG(LogTemp, Warning, TEXT("HotComplete"));
	DownLoadCompleteNum++;
	// 下载下一个文件
	DownLoadGameFile();
	// if (DownLoadCompleteNum == GameDataList.Num())
	// {
	// 	// 更新结束
	// 	OnUpDateEnd.Broadcast();
	// }
}

void AHotGameModeBase::HotDownloadProgress(int32 ContentLength, int32 BytesReceived)
{
	UE_LOG(LogTemp, Warning, TEXT("HotDownloadProgress: %d / %d"), BytesReceived, ContentLength);
	CurrentProgress = BytesReceived;
	FileLength = ContentLength;
}
