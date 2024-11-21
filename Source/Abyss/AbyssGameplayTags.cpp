#include "AbyssGameplayTags.h"

namespace AbyssGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned,"InitState.Spawned","1: Actor/component has initially spawned and can be extended")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable,"InitState.DataAvailable","2: All required data has been loaded/replicated and is ready for initialization")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized,"InitState.DataInitialized","3: The available data has been initialized for this actor/component, but it is not ready for full gameplay")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady,"InitState.GameplayReady","4: The actor/component is fully ready for active gameplay")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move Input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look(mouse) Input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Stick, "InputTag.Look.Stick", "Look(Stick) Input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "Crouch Input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AutoRun, "InputTag.AutoRun", "Auto-Run Input.");

}
