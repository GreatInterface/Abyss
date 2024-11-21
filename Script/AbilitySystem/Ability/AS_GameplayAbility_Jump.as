class UAS_GameplayAbility_Jump : UAbyssGameplayAbility_Jump
{
    UFUNCTION(BlueprintOverride)
    void ActivateAbility()
    {
        PrintToScreen(f"Jump", 2.0f);

        CharacterJumpStart();

        UAbilityTask_StartAbilityState State =
            AngelscriptAbilityTask::StartAbilityState(this, n"Jumping", true);

        State.OnStateEnded.AddUFunction(this, n"CharacterJumpStop");
        State.OnStateInterrupted.AddUFunction(this, n"CharacterJumpStop");

        AngelscriptAbilityTask::WaitInputRelease(this, true)
            .OnRelease
            .AddUFunction(this, n"HandleInputRelease");
    }

    UFUNCTION()
    void HandleInputRelease(float32 TimeHeld)
    {
        EndAbility();
    }
}