// Copyright (c) Jared Taylor


#include "PlayMontageTypes.h"

#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayMontageTypes)

FDrivenMontagePair::FDrivenMontagePair(UAnimMontage* InMontage, USkeletalMeshComponent* InSkeletalMeshComponent)
	: Montage(InMontage)
	, Mesh(InSkeletalMeshComponent)
{}
