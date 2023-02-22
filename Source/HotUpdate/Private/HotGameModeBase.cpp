// Fill out your copyright notice in the Description page of Project Settings.


#include "HotGameModeBase.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"

void AHotGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetServerJson();
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
				TSharedPtr<FJsonObject> versionMap = RootJsonObj->GetObjectField(TEXT("versionMap"));
				TSharedPtr<FJsonObject> VersionData = versionMap->GetObjectField(GameNewVersion);
				GameVersionDescribe = VersionData->GetStringField(TEXT("describe"));
				TSharedPtr<FJsonObject> AssetsList = VersionData->GetObjectField(TEXT("assetsList"));
				TArray<TSharedPtr<FJsonValue>> DataList = AssetsList->GetArrayField(TEXT("Game"));
				FJsonObjectConverter::JsonArrayToUStruct(DataList, &GameDataList);
				OnUpDateGame.Broadcast(isRun);
			}
			UE_LOG(LogTemp, Warning, TEXT("JsonStr: %s"), *JsonStr);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HttpResponse is not valid"));
	}
}

void AHotGameModeBase::GetServerJson()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetVerb("GET");
	UE_LOG(LogTemp, Display, TEXT("ServerURL: %s/BuildManifest.json"), *ServerURL);
	request->SetURL(ServerURL.Append(FString("/BuildManifest.json")));
	request->OnProcessRequestComplete().BindUObject(this, &AHotGameModeBase::OnResponseReceived);
	request->ProcessRequest();
}
