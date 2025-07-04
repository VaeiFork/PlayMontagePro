// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_PlayMontagePro.h"

#include "Containers/UnrealString.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/Platform.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
#include "PlayMontageProCallbackProxy.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_PlayMontagePro::UK2Node_PlayMontagePro(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UPlayMontageProCallbackProxy, CreateProxyObjectForPlayMontagePro);
	ProxyFactoryClass = UPlayMontageProCallbackProxy::StaticClass();
	ProxyClass = UPlayMontageProCallbackProxy::StaticClass();
}

FText UK2Node_PlayMontagePro::GetTooltipText() const
{
	return LOCTEXT("K2Node_PlayMontagePro_Tooltip", "Plays a Montage on a SkeletalMeshComponent");
}

FText UK2Node_PlayMontagePro::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("PlayMontagePro", "Play Montage Pro");
}

FText UK2Node_PlayMontagePro::GetMenuCategory() const
{
	return LOCTEXT("PlayMontageProCategory", "Animation|Montage");
}

void UK2Node_PlayMontagePro::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontagePro_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageProNotify or PlayMontageProNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		const FText ToolTipText = LOCTEXT("K2Node_PlayMontagePro_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageProNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

#undef LOCTEXT_NAMESPACE
