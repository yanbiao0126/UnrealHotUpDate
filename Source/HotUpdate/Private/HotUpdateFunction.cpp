// Fill out your copyright notice in the Description page of Project Settings.

#include "HotUpdateFunction.h"

FString UHotUpdateFunction::GetFileMd5(const FString filePath)
{
	TArray<uint8> Result;
	FFileHelper::LoadFileToArray(Result, *filePath);
	return FMD5::HashBytes(Result.GetData(), Result.Num());
}
