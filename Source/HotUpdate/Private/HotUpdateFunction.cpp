// Fill out your copyright notice in the Description page of Project Settings.

#include "HotUpdateFunction.h"
#include "BaseFilesDownloader.h"

FString UHotUpdateFunction::GetFileMd5(FString filePath)
{
	TArray<uint8> Result;
	UBaseFilesDownloader::LoadFileToArray(filePath, Result);
	return FMD5::HashBytes(Result.GetData(), Result.Num());
}
