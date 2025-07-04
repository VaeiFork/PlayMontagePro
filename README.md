# Grasp <img align="right" width=128, height=128 src="https://github.com/Vaei/PlayMontagePro/blob/main/Resources/Icon128.png">

> [!CAUTION]
> Do not use this plugin, it is not ready. At all.

> [!IMPORTANT]
> **Play Montage Pro**
> <br>TODO
> <br>And its **FREE!**

> [!CAUTION]
> Did you know?
> <br>Anim Notifies are not guaranteed to trigger. Have you heard "Animation should not affect gameplay"?
> <br>But with Unreal's powerful tools, you've built notifies that reload your weapon, and more! And you probably haven't experienced any issues. So it's fine, right?
> <br>In fact, the heavier your game gets, the less likely Anim Notifies are to fire. Its a trap that catches you later in production.
> <br>What if you had a notify that changes the movement mode to/from Flying? You can get stuck in Flying!
> <br>This is true for Queued notifies. The alternative is Branching Point notifies.
> <br>When two Branching Point notifies trigger on the same frame, only one will trigger!
> <br>There is no anim notify system in Unreal that ensures your notifies will fire reliably. PMP is the solution to this.

> [!WARNING]
> Use `git clone` instead of downloading as a zip, or you will not receive content
> <br>[Or download the pre-compiled binaries here](https://github.com/Vaei/PlayMontagePro/wiki/How-to-Use)
> <br>Install this as a project plugin, not an engine plugin

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

## DOES NOT SUPPORT TODO
* Ticking anim notify state
* NotifyTriggerChance
* bTriggerOnDedicatedServer
* SimulatedProxies and editor notifies use UE's notify pathing
* No FAnimNotifyEventReference
* Only supports montages, notifies on sequences will do nothing, only through provided nodes

## ADDITIONAL THINGS PMP CAN DO
* Trigger notifies earlier than StartTimeSeconds when the montage starts (ensure all notifies on the montage trigger)

## Changelog

### 1.0.0
* Initial Release