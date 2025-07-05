// Copyright (c) Jared Taylor


#include "Ability/PMPGameplayAbility.h"

// Most of this is from GASShooter and therefore also Copyright 2024 Dan Kestranek.
// https://github.com/tranek/GASShooter

#include UE_INLINE_GENERATED_CPP_BY_NAME(PMPGameplayAbility)

UPMPGameplayAbility::UPMPGameplayAbility(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UPMPGameplayAbility::FindAbilityMeshMontage(const USkeletalMeshComponent* InMesh,
	FAbilityMeshMontage& InAbilityMontage)
{
	for (const FAbilityMeshMontage& MeshMontage : CurrentAbilityMeshMontages)
	{
		if (MeshMontage.Mesh == InMesh)
		{
			InAbilityMontage = MeshMontage;
			return true;
		}
	}

	return false;
}

UAnimMontage* UPMPGameplayAbility::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbilityMeshMontage(InMesh, AbilityMeshMontage))
	{
		return AbilityMeshMontage.Montage;
	}

	return nullptr;
}

void UPMPGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* InCurrentMontage)
{
	ensure(IsInstantiated());

	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbilityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InCurrentMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FAbilityMeshMontage(InMesh, InCurrentMontage));
	}
}
