# Grasp <img align="right" width=128, height=128 src="https://github.com/Vaei/PlayMontagePro/blob/main/Resources/Icon128.png">

> [!CAUTION]
> WIP. Under heavy development.

> [!IMPORTANT]
> **Play Montage Pro (PMP)**
> <br>TODO
> <br>And its **FREE!**

> [!NOTE]
> Did you know? Anim Notifies are not guaranteed to trigger.
> <br>Have you heard the common phrase "Animation should not affect gameplay"?
> <br>But you've probably already built notifies that reload your weapon and more without experiencing issues.
> <br><br>Its a trap that catches you later in production. The heavier your game gets, the less likely Anim Notifies are to fire.
> <br><br>This is true for Queued notifies. The alternative is Branching Point notifies.
> <br>When two or more Branching Point notifies trigger on the same frame, only one will trigger!
> <br><br>What if you had a notify that changes the movement mode to or from Flying? You can get **stuck** in Flying!
> <br>There is no anim notify system in Unreal that ensures your notifies will fire reliably.
> <br>Play Montage Pro is the solution to this.

> [!TIP]
> At it's core PMP uses timers to trigger notifies when you play a montage or change montage sections
> <br>Timers are reliable!

> [!WARNING]
> [Download the pre-compiled binaries here](https://github.com/Vaei/PlayMontagePro/wiki/How-to-Use)

> [!TIP]
> Suitable for both singleplayer and multiplayer games
> <br>Supports UE5.4+

> [!CAUTION]
> PlayMontagePro is currently in beta
> <br>There are many non-standard setups that have not been fully tested
> <br>PlayMontagePro has not been tested at scale in production
> <br>Feedback is wanted on PlayMontagePro's workflow

## How to Use
> [!IMPORTANT]
> [Read the Wiki to Learn How to use PlayMontagePro](https://github.com/Vaei/PlayMontagePro/wiki/How-to-Use)

## TODO DOES NOT SUPPORT
* Ticking anim notify state
* NotifyTriggerChance and all trigger settings TODO you can override ShouldTrigger etc to add this in
* SimulatedProxies and editor notifies use UE's notify pathing
* No FAnimNotifyEventReference
* Only supports montages, notifies on sequences will do nothing, only through provided nodes

## TODO ADDITIONAL THINGS PMP CAN DO
* Trigger notifies earlier than StartTimeSeconds when the montage starts (ensure all notifies on the montage trigger)
* Ensure notifies trigger on end (TODO disclaimer ordinary play montage leaves alive etc. when stopped?)

## TODO ADDITIONS
* Test stopping montage; why do timers continue??
* Test firing in editor and sim proxies

## TODO CONSIDERATIONS
* The system isn't identical to Epic's
	* Behaviour may not always be as intended
	* Use this for gameplay critical systems, use Epic's system for cosmetics
	* Because sim proxies use Epic's system, results may mismatch in some cases
		* It is not expected that the type of use-case for this should be pertinent to sim proxies also
		* This can be reported as an issue hopefully it will become 1:1 within reason
* CustomTimeDilation per-actor; there are no callbacks, in fact, its a property without a setter, sloppy coding on Epic's part. This is supported as optional, but relies on `USkinnedMeshComponent::OnTickPose` to detect changes. If your dedi server doesn't tick mesh pose it will not work.

## Changelog

### 1.0.0
* Initial Release