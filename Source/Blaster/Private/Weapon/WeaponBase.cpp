#include "Weapon/WeaponBase.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Character/BlasterCharacter.h"


AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnSphereEndOverlap);

	}

	if (IsValid(PickupWidget))
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::OnSphereOverlap
(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
)
{
	ABlasterCharacter* blasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	if (IsValid(blasterCharacter) && IsValid(PickupWidget))
	{
		blasterCharacter->SetOverlappingWeapon(this);
	}

}

void AWeaponBase::OnSphereEndOverlap
(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex
)
{
	ABlasterCharacter* blasterCharacter = Cast<ABlasterCharacter>(OtherActor);

	if (IsValid(blasterCharacter) && IsValid(PickupWidget))
	{
		blasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeaponBase::ShowPickupWidget(bool bShowWidget)
{
	if (IsValid(PickupWidget))
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

