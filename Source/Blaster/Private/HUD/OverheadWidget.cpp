#include "HUD/OverheadWidget.h"

#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(const FText& InTextToDisplay)
{
	if (IsValid(DisplayText))
	{
		DisplayText->SetText(InTextToDisplay);
	}
}

void UOverheadWidget::ShowPlayerNetRole(const APawn* InPawn)
{
	ENetRole localRole = InPawn->GetLocalRole();

	FString role;

	switch (localRole)
	{
	case ROLE_None:
		role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		role = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		role = FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		role = FString("Authority");
		break;
	case ROLE_MAX:
		role = FString("MAX");
		break;
	default:
		break;
	}

	FString localRoleString = FString::Printf(TEXT("Local Role: %s"), *role);

	SetDisplayText(FText::FromString(localRoleString));
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}
