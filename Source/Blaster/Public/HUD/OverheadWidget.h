#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"


UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DisplayText;

	UFUNCTION(BlueprintCallable)
	void SetDisplayText(const FText& InTextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(const APawn* InPawn, bool bShowLocalRole = true);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole_Local(const APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole_Remote(const APawn* InPawn);

protected:
	virtual void NativeDestruct() override;

};
