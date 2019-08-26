# GASDocumentation
My understanding of Unreal Engine 4's GameplayAbilitySystem plugin (GAS) with a simple multiplayer sample project. This is not official documentation and neither this project nor myself are affiliated with Epic Games. I make no guarantee for the accuracy of this information.

The goal of this documentation is to explain the major concepts and classes in GAS and provide some additional commentary based on my experience with it. There is a lot of 'tribal knowledge' of GAS among users in the community and I aim to share all of mine here.

The best documentation will always be the plugin source code.

<a name="table-of-contents"></a>
## Table of Contents

> 1. [Intro to the GameplayAbilitySystem Plugin](#intro)
> 1. [Sample Project](#sp)
> 1. [Concepts](#concepts)  
>    3.1 [Ability System Component](#concepts-asc)  
>    3.2 [Gameplay Tags](#concepts-gt)  
>    3.3 [Attributes](#concepts-a)  
>    &nbsp;&nbsp;&nbsp;&nbsp;3.3.1 [Attribute Definition](#concepts-a-definition)  
>    3.4 [Attribute Set](#concepts-as)  
>    3.5 [Gameplay Effects](#concepts-ge)  
>    3.6 [Gameplay Abilities](#concepts-ga)  
>    3.7 [Ability Tasks](#concepts-at)  
>    3.8 [Gameplay Cues](#concepts-gc)  
>    3.9 [Ability System Globals](#concepts-asg)  
>    3.10 [Prediction](#concepts-p)
> 1. [Commonly Implemented Abilities and Effects](#cae)
> 1. [Setting Up a Project Using GAS](#setup)
> 1. [Debugging](#debugging)
> 1. [Common GAS Acronymns](#acronyms)
> 1. [Other Resources](#resources)
         
<a name="intro"></a>
<a name="1"></a>
## 1. Intro to the GameplayAbilitySystem Plugin
From the official documentation (**LINK TO OFFICIAL DOCUMENTATION**):
>The Gameplay Ability System is a highly-flexible framework for building abilities and attributes of the type you might find in an RPG or MOBA title. You can build actions or passive abilities for the characters in your games to use, status effects that can build up or wear down various attributes as a result of these actions, implement "cooldown" timers or resource costs to regulate the usage of these actions, change the level of the ability and its effects at each level, activate particle or sound effects, and more. Put simply, this system can help you to design, implement, and efficiently network in-game abilities as simple as jumping or as complex as your favorite character's ability set in any modern RPG or MOBA title.

The GameplayAbilitySystem plugin is developed by Epic Games and comes with Unreal Engine 4 (UE4). It has been battle tested in AAA commercial games such as Paragon and Fortnite among others.

The plugin provides an out-of-the-box solution in single and multiplayer games for:
* Implementing level-based character abilities or skills (**LINK TO ABILITIES**) with optional costs and cooldowns
* Manipulating numerical `Attributes` belonging to actors (**LINK TO ATTRIBUTES**)
* Applying status effects to actors with `GameplayEffects` (**LINK TO GAMEPLAYEFFECTS**)
* Applying `GameplayTags` (**LINK TO GAMEPLAYTAGS**) to actors
* Spawning visual or sound effects with `GameplayCues` (**LINK TO GAMEPLAYCUES**)
* Replication of everything mentioned above

In multiplayer games, GAS provides support for local prediction (**LINK TO PREDICTION**) of:
* Ability activation
* Playing animation montages
* Changes to `Attributes`
* Applying `GameplayTags`
* Spawning `GameplayCues`
* Movement via `RootMotionSource` functions connected to the `CharacterMovementComponent`. **Note:** Predicting movement with `RootMotionSource` broke in UE 4.20.

**GAS must be set up in C++**, but `GameplayAbilities` and `GameplayEffects` can be created in Blueprint by the designers.

Current issues with GAS:
* Predicting `RootMotionSource` `AbilityTasks` broke in UE 4.20. This may not get fixed until the new Network Prediction plugin is integrated into GAS.
* `GameplayEffect` latency reconciliation (can't predict ability cooldowns resulting in players with higher latencies having lower rate of fire for low cooldown abilities compared to players with lower latencies).
* Cannot predict the removal of `GameplayEffects`. We can however predict adding `GameplayEffects` with the inverse effects, effectively removing them. This is not always appropriate or feasible and still remains an issue.
* Lack of boilerplate templates, multiplayer examples, and documentation. Hopefully this helps with that!

**[⬆ Back to Top](#table-of-contents)**

<a name="sp"></a>
<a name="2"></a>
## 2. Sample Project
A multiplayer third person shooter sample project is included with this documentation aimed at people new to the GameplayAbilitySystem Plugin but not new to Unreal Engine 4. Users are expected to know C++, Blueprints, UMG, Replication, and other intermediate topics in UE4. This project provides an example of how to set up a basic third person shooter mulitplayer-ready project with the `AbilitySystemComponent` (`ASC`) on the `PlayerState` class for player/AI controlled heroes and the `ASC` on the `Character` class for AI controlled minions.

The goal is to keep this project simple while showing the GAS basics and demonstrating some commonly requested abilities with well-commented code. Because of its beginner focus, the project does not show advanced topics like predicting (**LINK TO LOCAL PREDICTION DESCRIPTION**) projectiles.

Concepts demonstrated:
* `ASC` on `PlayerState` vs `Character`
* Replicated `Attributes`
* Replicated animation montages
* `GameplayTags`
* Applying and removing `GameplayEffects` inside of and externally from `GameplayAbilities`
* Applying damage mitigated by armor to change health of a character
* `GameplayEffectExecutionCalculations`
* Stun effect
* Death and respawn
* Spawning actors (projectiles) from an ability on the server
* Predictively changing the local player's speed with aim down sights and sprinting
* Constantly draining stamina to sprint
* Using mana to cast abilities
* Passive abilities
* Stacking `GameplayEffects`
* Targeting actors
* `GameplayAbilities` created in Blueprint
* `GameplayAbilities` created in C++
* Instanced per `Actor` `GameplayAbilities`
* Non-Instanced `GameplayAbilities` (Jump)

The hero class has the following abilities:

| Ability                    | Input Bind          | Predicted  | C++ / Blueprint | Description                                                                                                                                                                  |
| -------------------------- | ------------------- | ---------- | --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Jump                       | Space Bar           | Yes        | C++             | Makes the hero jump.                                                                                                                                                         |
| Gun                        | Left Mouse Button   | No         | C++             | Fires a projectile from the hero's gun. The animation is predicted but the projectile is not.                                                                                |
| Aim Down Sights            | Right Mouse Button  | Yes        | Blueprint       | While the button is held, the hero will walk slower and the camera will zoom in to allow more precise shots with the gun.                                                    |
| Sprint                     | Left Shift          | Yes        | Blueprint       | While the button is held, the hero will run faster draining stamina.                                                                                                         |
| Forward Dash               | Q                   | No*        | Blueprint       | The hero dashes forward. _*While this ability is set up to be predicted, GAS's `RootMotionSource` `AbilityTasks` are bugged (**LINK TO ROOT MOTION SOURCE ABILITY TASKS**)._ |
| Passive Armor Stacks       | Passive             | No         | Blueprint       | Every 4 seconds the hero gains a stack of armor up to a maximum of 4 stacks. Receiving damage removes one stack of armor.                                                    |
| Meteor                     | R                   | No         | Blueprint       | Player targets a location to drop a meteor on the enemies causing damage and stunning them. The targeting is predicted while spawning the meteor is not.        |

It does not matter if `GameplayAbilities` are created in C++ or Blueprint. A mixture of the two were used here for example of how to do them in each language.

Minions do not come with any predefined `GameplayAbilities`. The Red Minions have more health regen while the Blue Minions have higher starting health.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts"></a>
<a name="3"></a>
## 3. GAS Concepts

#### Sections

> 3.1 [Ability System Component](#concepts-asc)  
> 3.2 [Gameplay Tags](#concepts-gt)  
> 3.3 [Attributes](#concepts-a)  
> 3.4 [Attribute Set](#concepts-as)  
> 3.5 [Gameplay Effects](#concepts-ge)  
> 3.6 [Gameplay Abilities](#concepts-ga)  
> 3.7 [Ability Tasks](#concepts-at)  
> 3.8 [Gameplay Cues](#concepts-gc)  
> 3.9 [Ability System Globals](#concepts-asg)  
> 3.10 [Prediction](#concepts-p)

<a name="concepts-asc"></a>
<a name="3.1"></a>
### 3.1 Ability System Component
The `AbilitySystemComponent` (`ASC`) is the heart of GAS. It's a `USceneComponent` (`UAbilitySystemComponent`) (**LINK TO OFFICIAL API**) that handles all interactions with the system. Any `Actor` that wishes to use `Abilities` (**LINK TO ABILITIES**), have `Attributes`(**LINK TO ATTRIBUTES**), or receive `GameplayEffects`(**LINK TO GAMEPLAYEFFECTS**) must have one `ASC` attached to them. These objects all live inside of and are managed and replicated by (with the exception of `Attributes` which are replicated by their `AttributeSet`(**LINK TO ATTRIBUTESET**)) the `ASC`. Developers are expected but not required to subclass this.

The `Actor` with the `ASC` attached to it is referred to as the `OwnerActor` of the `ASC`. The physical representation `Actor` of the `ASC` is called the `AvatarActor`. The `OwnerActor` and the `AvatarActor` can be the same `Actor` as in the case of a simple AI minion in a MOBA game. They can also be different `Actors` as in the case of a player controlled hero in a MOBA game where the `OwnerActor` is the `PlayerState` and the `AvatarActor` is the hero's `Character` class. Most `Actors` will have the `ASC` on themselves. If your `Actor` will respawn and need persistence of `Attributes` or `GameplayEffects` between spawns (like a hero in a MOBA), then the ideal location for the `ASC` is on the `PlayerState`.

Both, the `OwnerActor` and the `AvatarActor` if different `Actors`, should implement the `IAbilitySystemInterface`. This interface has one function that must be overriden, `UAbilitySystemComponent* GetAbilitySystemComponent() const`, which returns a pointer to its `ASC`. `ASCs` interact with each other internally to the system by looking for this interface function.

`ASCs` are typically constructed in the `OwnerActor's` constructor and marked replicated. **This must be done in C++**.

The `ASC` defines three different replication modes for replicating `GameplayEffects`, `GameplayTags`, and `GameplayCues` - `Full`, `Mixed`, and `Minimal`. `Attributes` are replicated by their `AttributeSet`.

| Replication Mode   | When to Use                             | Description                                                                                                                                   |
| ------------------ | --------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `Full`             | Single Player                           | Every `GameplayEffect` is replicated to every client.                                                                                         |
| `Mixed`            | Multiplayer, player controlled `Actors` | `GameplayEffects` are only replicated to the owning client. Only `GameplayTags` and `GameplayCues` are replicated to everyone. |
| `Minimal`          | Multiplayer, AI controlled `Actors`     | `GameplayEffects` are never replicated to anyone. Only `GameplayTags` and `GameplayCues` are replicated to everyone.           |

**Note:** `Mixed` replication mode expects the `OwnerActor's` `Owner` to be the `Controller`. `PlayerState's` `Owner` is the `Controller` by default but `Character's` is not. If using `Mixed` replication mode with the `OwnerActor` not the `PlayerState`, then you need to call `SetOwner()` on the `OwnerActor` with a valid `Controller`.

The `ASC` holds its current active `GameplayEffects` in `FActiveGameplayEffectsContainer ActiveGameplayEffects`.

<a name="concepts-gt"></a>
<a name="3.2"></a>
### 3.2 Gameplay Tags
`FGameplayTags` (**LINK TO API**) are hierarchical names in the form of parent.child.grandchild... that are registered with the `GameplayTagManager`. These tags are incredibly useful for classifying and describing the state of an object. For example, if a character is stunned, we could give it a `State.Debuff.Stun` `GameplayTag` for the duration of the stun.

You will find yourself replacing things that you used to handle with booleans or enums with `GameplayTags` and doing boolean logic on whether or not objects have certain `GameplayTags`.

When giving tags to an object, we typically add them to its `ASC` if it has one so that GAS can interact with them. `UAbilitySystemComponent` implements the `IGameplayTagAssetInterface` giving functions to access its owned `GameplayTags`.

Multiple `GameplayTags` can be stored in an `FGameplayTagContainer`. It is preferable to use a `GameplayTagContainer` over a `TArray<FGameplayTag>` since the `GameplayTagContainers` add some efficiency magic. While tags are standard `FNames`, they can be efficiently packed together in `FGameplayTagContainers` for replication if `Fast Replication` is enabled in the project settings. `Fast Replication` requires that the server and the clients have the same list of `GameplayTags`. This generally shouldn't be a problem so you should enable this option. `GameplayTagContainers` can also return a `TArray<FGameplayTag>` for iteration. `GameplayTags` stored in `GameplayTagContainers` have a `TagMap` that stores the number of instances of that `GameplayTag`. A `GameplayTagContainer` may still have the `GameplayTag` in it but its `TagMapCount` is zero. You may encounter this while debugging if an `ASC` still has a `GameplayTag`. Any of the `HasTag()` or `HasMatchingTag()` or similar functions will check the `TagMapCount` and return false if the `GameplayTag` is not present or its `TagMapCount` is zero.

`GameplayTags` must be defined ahead of time in the `DefaultGameplayTags.ini`. The UE4 Editor provides an interface in the project settings to let developers manage `GameplayTags` without needing to manually edit the `DefaultGameplayTags.ini`. The `GameplayTag` editor can create, rename, search for references, and delete `GameplayTags`.

(**PIC OF GAMEPLAYTAG EDITOR IN PROJECT SETTINGS**)

Searching for `GameplayTag` references will bring up the familiar `Reference Viewer` graph in the Editor showing all the assets that reference the `GameplayTag`. This will not however show any C++ classes that reference the `GameplayTag`.

Renaming `GameplayTags` creates a redirect so that assets still referencing the original `GameplayTag` can redirect to the new `GameplayTag`. I prefer if possible to instead create a new `GameplayTag`, update all the references manually to the new `GameplayTag`, and then delete the old `GameplayTag` to avoid creating a redirect.

In addition to `Fast Replication`, the `GameplayTag` editor has an option to fill in commonly replicated `GameplayTags` to optimize them further.

`GameplayTags` are replicated if they're added from a `GameplayEffect`. The `ASC` allows you to add `LooseGameplayTags` that are not replicated and must be managed manually. The Sample Project uses a `LooseGameplayTag` for `State.Dead` so that the owning clients can immediately respond to when their health drops to zero. Respawning manually sets the `TagMapCount` back to zero. Only manually adjust the `TagMapCount` when working with `LooseGameplayTags`. It is preferable to use the `UAbilitySystemComponent::AddLooseGameplayTag()` and `UAbilitySystemComponent::RemoveLooseGameplayTag()` functions than manually adjusting the `TagMapCount`.

Getting a reference to a `GameplayTag` in C++:
```c++
FGameplayTag::RequestGameplayTag(FName("Your.GameplayTag.Name"))
```

The Sample Project extensively uses `GameplayTags`.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-a"></a>
<a name="3.3"></a>
### 3.3 Attributes

<a name="concepts-a-definition"></a>
<a name="3.3.1"></a>
#### 3.3.1 Attribute Definition
`Attributes` are float values defined by the struct `FGameplayAttributeData` (**LINK TO OFFICIAL API**). These can represent anything from the amount of health a character has to the character's level to the number of charges that a potion has. If it is a gameplay-related numerical value belonging to an `Actor`, you should consider using an `Attribute` for it. `Attributes` should generally only be modified by `GameplayEffects` (**LINK TO GAMEPLAYEFFECT**) so that the ASC can predict (**LINK TO PREDICTION**) the changes.

`Attributes` are defined by and live in an `AttributeSet` (**LINK TO ATTRIBUTESET**). The `AttributeSet` is reponsible for replicating `Attributes` that are marked for replication. See the section on `AttributeSets` (**LINK TO ATTRIBUTESETS**) for how to define `Attributes`.

<a name="concepts-a-value"></a>
<a name="3.3.2"></a>
#### 3.3.2 BaseValue vs CurrentValue
An `Attribute` is composed of two values - a `BaseValue` and a `CurrentValue`. The `BaseValue` is the permanent value of the `Attribute` whereas the `CurrentValue` is the `BaseValue` plus temporary modifications from `GameplayEffects`. For example, your `Character` may have a movespeed `Attribute` with a `BaseValue` of 600 units/second. Since there are no `GameplayEffects` modifying the movespeed yet, the `CurrentValue` is also 600 u/s. If she gets a temporary 50 u/s movespeed buff, the `BaseValue` stays the same at 600 u/s while the `CurrentValue` is now 600 + 50 for a total of 650 u/s. When the movespeed buff expires, the `CurrentValue` reverts back to the `BaseValue` of 600 u/s.

Often beginners to GAS will confuse `BaseValue` with a maximum value for an `Attribute` and try to treat it as such. This is an incorrect approach. Maximum values for `Attributes` that can change or are referenced in abilities or UI should be treated as separate `Attributes`. For hardcoded maximum and minimum values, there is a way to define a `DataTable` with `FAttributeMetaData` that can set maximum and minimum values, but Epic's comment above the struct calls it a "work in progress". See `AttributeSet.h` for more information. To prevent confusion, I recommend that maximum values that can be referenced in abilities or UI be made as separate `Attributes` and hardcoded maximum and minimum values that are only used for clamping `Attributes` be defined as hardcoded floats in the `AttributeSet`. Clamping of `Attributes` is discussed in (**LINK TO ATTRIBUTE SET PREATTRIBUTECHANGE/POSTGAMEPLAYEFFECT**).

Permanent changes to the `BaseValue` come from instant `GameplayEffects` whereas duration/infinite `GameplayEffects` change the `CurrentValue`. Periodic `GameplayEffects` are treated like instant `GameplayEffects` and change the `BaseValue`.

<a name="concepts-a-meta"></a>
<a name="3.3.3"></a>
#### 3.3.3 Meta Attributes
Some `Attributes` are treated as placeholders for temporary values that are intended to interact with `Attributes`. These are called `Meta Attributes`. For example, we commonly define damage as a `Meta Attribute`. Instead of a `GameplayEffect` directly changing our health `Attribute`, we use a `Meta Attribute` called damage as a placeholder. This way the damage value can be modified with buffs and debuffs in an `GameplayEffectExecutionCalculation` (**LINK TO EXECUTIONCALCULATIONS**) and can be further manipulated in the `AttributeSet` for example subtracting the damage from a current shield `Attribute` before finally subtracting the remainder from the health `Attribute`. The damage `Meta Attribute` has no persistence between `GameplayEffects` and is overriden by every one.

`Meta Attributes` are not typically replicated.

<a name="concepts-a-changes"></a>
<a name="3.3.4"></a>
#### 3.3.4 Responding to Attribute Changes
To listen for when an attribute changes to update the UI or other gameplay, use `UAbilitySystemComponent::GetGameplayAttributeValueChangeDelegate(FGameplayAttribute Attribute)`. This function returns a delegate that you can bind to that will be automatically called whenever an `Attribute` changes. The delegate provides a `FOnAttributeChangeData` parameter with the `NewValue`, `OldValue`, and `FGameplayEffectModCallbackData`. **Note:** The `FGameplayEffectModCallbackData` will only be set on the server.

The Sample Project binds to the attribute value changed delegates on the `GDPlayerState` to update the HUD and to respond to player death when health reaches zero. A custom Blueprint node that wraps this into an `ASyncTask` is included in the Sample Project. It is used in the `UI_HUD` UMG Widget to update the health, mana, and stamina values. See `AsyncTaskAttributeChanged.h/cpp`.

(**PICTURE OF BP NODE FOR ATTRIBUTE CHANGE**)

<a name="concepts-as"></a>
<a name="3.4"></a>
### 3.4 Attribute Set

<a name="concepts-as-definition"></a>
<a name="3.4.1"></a>
#### 3.4.1 Attribute Set Definition
The `AttributeSet` defines, holds, and manages changes to `Attributes`. Developers should subclass from `UAttributeSet` (**LINK TO API DOCUMENATION**). Creating an `AttributeSet` in an `OwnerActor's` constructor automatically registers it with its `ASC`. **This must be done in C++**.

<a name="concepts-as-design"></a>
<a name="3.4.2"></a>
#### 3.4.2 Attribute Set Design
An `ASC` may have one or many `AttributeSets`. AttributeSets have negligable memory overhead so how many `AttributeSets` to use is an organizational decision left up to the developer.

It is acceptable to have one large monolithic `AttributeSet` shared by every `Actor` in your game and only use attributes if needed while ignoring unused attributes.

Alternatively, you may choose to have more than one `AttributeSet` representing groupings of `Attributes` that you selectively add to your `Actors` as needed. For example, you could have an `AttributeSet` for health related `Attributes`, an `AttributeSet` for mana related `Attributes`, and so on. In a MOBA game, heroes might need mana but minions might not. Therefore the heroes would get the mana `AttributeSet` and minions would not.

Additionally, `AttributeSets` can be subclassed as another means of selectively choosing which `Attributes` an `Actor` has.

<a name="concepts-as-attributes"></a>
<a name="3.4.3"></a>
#### 3.4.3 Defining Attributes
**`Attributes` can only be defined in C++** in the `AttributeSet's` header file. It is recommended to add this block of macros to the top of every `AttributeSet` header file. It will automatically generate getter and setter functions for your `Attributes`.

```c++
// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
```

A replicated health attribute would be defined like this:

```c++
UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
FGameplayAttributeData Health;
ATTRIBUTE_ACCESSORS(UGDAttributeSetBase, Health)
```

Also define the `OnRep` function in the header:
```c++
UFUNCTION()
virtual void OnRep_Health();
```

The .cpp file for the `AttributeSet` should fill in the `OnRep` function with the `GAMEPLAYATTRIBUTE_REPNOTIFY` macro used by the prediction system:
```c++
void UGDAttributeSetBase::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGDAttributeSetBase, Health);
}
```

Finally, the `Attribute` needs to be added to `GetLifetimeReplicatedProps`:
```c++
void UGDAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGDAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
}
```

If the `Attribute` is not replicated like a `Meta Attribute`, then the `OnRep` and `GetLifetimeReplicatedProps` steps can be skipped.

<a name="concepts-as-init"></a>
<a name="3.4.4"></a>
#### 3.4.4 Initializing Attributes
There are multiple ways to initialize `Attributes` (set their `BaseValue` and consequently their `CurrentValue` to some initial value). Epic recommends using an instant `GameplayEffect`. This is the method used in the Sample Project too.

See `GE_HeroAttributes` Blueprint in the Sample Project for how to make an instant `GameplayEffect` to initialize `Attributes`. Application of this `GameplayEffect` happens in C++.

See `AttributeSet.h` for more ways to initialize `Attributes`.

**Note:** `FAttributeSetInitterDiscreteLevels` does not work with `FGameplayAttributeData`. It was created when `Attributes` were raw floats and will complain about `FGameplayAttributeData` not being `Plain Old Data` (`POD`). This is set to be fixed in 4.24 https://issues.unrealengine.com/issue/UE-76557.

<a name="concepts-as-preattributechange"></a>
<a name="3.4.5"></a>
#### 3.4.5 PreAttributeChange()
`PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)` is one of the main functions in the `AttributeSet` to respond to changes to an `Attribute's` `CurrentValue` before the change happens. It is the ideal place to clamp incoming changes to `CurrentValue` via the reference parameter `NewValue`.

For example to clamp movespeed modifiers the Sample Project does it like so:
```c++
if (Attribute == GetMoveSpeedAttribute())
{
	// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
	NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
}
```
The `GetMoveSpeedAttribute()` function is created by the macro block that we added to the `AttributeSet.h` (**LINK TO DEFINING ATTRIBUTES**).

This is triggered from any changes to `Attributes`, whether using `Attribute` setters (defined by the macro block in `AttributeSet.h` (**LINK TO DEFINING ATTRIBUTES**)) or using `GameplayEffects` (**LINK TO GAMEPLAYEFFECTS**).

**Note:** Any clamping that happens here does not permanently change the modifier on the `ASC`. It only changes the value returned from querying the modifier. This means anything that recalculates the `CurrentValue` from all of the modifiers like `GameplayEffectExecutionCalculations` (**LINK TO EXEC CALCS**) and `ModifierMagnitudeCalculations` (**LINK TO MODMAGCALCS**) need to implement clamping again.

**Note:** Epic's comments for `PreAttributeChange()` say not to use it for gameplay events and instead use it mainly for clamping. The recommended place for gameplay events on `Attribute` change is `UAbilitySystemComponent::GetGameplayAttributeValueChangeDelegate(FGameplayAttribute Attribute)` (**LINK TO RESPONDING TO ATTRIBUTE CHANGES**).

<a name="concepts-as-postgameplayeffectexecute"></a>
<a name="3.4.6"></a>
#### 3.4.6 PostGameplayEffectExecute()
`PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)` only triggers after changes to the `BaseValue` of an `Attribute` from an instant `GameplayEffect` (**LINK TO GAMEPLAYEFFECT**). This is a valid place to do more `Attribute` manipulation when they change from a `GameplayEffect`.

For example, in the Sample Project we subtract the final damage `Meta Attribute` from the health `Attribute` here. If there was a shield `Attribute`, we would subtract the damage from it first before subtracting the remainder from health. The Sample Project also uses this location to apply hit react animations, show floating Damage Numbers, and assign experience and gold bounties to the killer. By design, the damage `Meta Attribute` will always come through an instant `GameplayEffect` and never the `Attribute` setter.

Other `Attributes` that will only have their `BaseValue` changed from instant `GameplayEffects` like mana and stamina can also be clamped to their maximum value counterpart `Attributes` here.

**Note:** When `PostGameplayEffectExecute()` is called, changes to the `Attribute` have already happened, but they have not replicated back to clients yet so clamping values here will not cause two network updates to clients. Clients will only receive the update after clamping.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-ge"></a>
<a name="3.5"></a>
### 3.5 Gameplay Effects

<a name="concepts-ge-definition"></a>
<a name="3.5.1"></a>
#### 3.5.1 Gameplay Effect definition
`GameplayEffects` (`GEs`) (**LINK TO API**) are the vessels through which abilities change `Attributes` (**LINK TO ATTRIBUTES**) and `GameplayTags` (**LINK TO GAMEPLAYTAGS**) on themselves and others. They can cause immediate `Attribute` changes like damage or healing or apply long term status buff/debuffs like a movespeed boost or stunning. The `UGameplayEffect` class is a meant to be a **data-only** class that defines a single gameplay effect. No additional logic should be added to `GameplayEffects`. Typically designers will create many Blueprint child classes of `UGameplayEffect`.

`GameplayEffects` change `Attributes` through `Modifiers` (**LINK BELOW TO MODIFIERS**) and `Executions` (`GameplayEffectExecutionCalculation`) (**LINK BELOW TO EXECUTIONCALCS**).

`GameplayEffects` have three types of duration: `Instant`, `Duration`, and `Infinite`.

Additionally, `GameplayEffects` can add/execute `GameplayCues` (**LINK TO GAMEPLAYCUES**). An `Instant` `GameplayEffect` will call `Execute` on the `GameplayCue` `GameplayTags` whereas a `Duration` or `Infinite` `GameplayEffect` will call `Add` and `Remove` on the `GameplayCue` `GameplayTags`.

| Duration Type | GameplayCue Event | When to use                                                                                                                                                                                                                                |
| ------------- | ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `Instant`     | Execute           | For immediate permanent changes to `Attribute's` `BaseValue`. `GameplayTags` will not be applied, not even for a frame.                                                                                                                    |
| `Duration`    | Add & Remove      | For temporary changes to `Attribute's` `CurrentValue` and to apply `GameplayTags` that will be removed when the `GameplayEffect` expires or is manually removed. The duration is specified in the `UGameplayEffect` class/Blueprint.       |
| `Infinite`    | Add & Remove      | For temporary changes to `Attribute's` `CurrentValue` and to apply `GameplayTags` that will be removed when the `GameplayEffect` is removed. These will never expire on their own and must be manually removed by an ability or the `ASC`. |

`Duration` and `Infinite` `GameplayEffects` have the option of applying `Periodic Effects` that apply its `Modifiers` and `Executions` every `X` seconds as defined by its `Period`. `Periodic Effects` are treated as `Instant` `GameplayEffects` when it comes to changing the `Attribute's` `BaseValue` and `Executing` `GameplayCues`. **Note:** `Periodic Effects` cannot be predicted. (**LINK TO PREDICTION**)

`Duration` and `Infinite` `GameplayEffects` can be temporarily turned off and on after application if their `Ongoing Tag Requirements` are not met/met (**LINK TO GE TAGS**). Turning off a `GameplayEffect` removes the effects of its `Modifiers` and applied `GameplayTags` but does not remove the `GameplayEffect`. Turning the `GameplayEffect` back on reapplies its `Modifiers` and `GameplayTags`.

`GameplayEffects` are not typically instantiated. When an ability or `ASC` wants to apply a `GameplayEffect`, it creates a `GameplayEffectSpec` (**LINK TO GAMEPLAYEFFECTSPEC**) from the `GameplayEffect's` `ClassDefaultObject`. Successfully applied `GameplayEffectSpecs` are then added to a new struct called `FActiveGameplayEffect` which is what the `ASC` keeps track of in a special container struct called `ActiveGameplayEffects`.

<a name="concepts-ge-applying"></a>
<a name="3.5.2"></a>
#### 3.5.2 Applying Gameplay Effects
`GameplayEffects` can be applied in many ways from functions on `GameplayAbilities` (**LINK TO GAMEPLAYABILITIES**) and functions on the `ASC` and usually take the form of `ApplyGameplayEffectTo`. The different functions are essentially convenience functions that will eventually call `UAbilitySystemComponent::ApplyGameplayEffectSpecToSelf()` on the `Target`.

To apply `GameplayEffects` outside of a `GameplayAbility` for example from a projectile, you need to get the `Target's` `ASC` and use one of its functions to `ApplyGameplayEffectToSelf`.

<a name="concepts-ga-removing"></a>
<a name="3.5.3"></a>
#### 3.5.3 Removing Gameplay Effects
`GameplayEffects` can be removed in many ways from functions on `GameplayAbilities` (**LINK TO GAMEPLAYABILITIES**) and functions on the `ASC` and usually take the form of `RemoveActiveGameplayEffect`. The different functions are essentially convenience functions that will eventually call `FActiveGameplayEffectsContainer::RemoveActiveEffects()` on the `Target`.

To remove `GameplayEffects` outside of a `GameplayAbility`, you need to get the `Target's` `ASC` and use one of its functions to `RemoveActiveGameplayEffect`.

<a name="concepts-ge-mods"></a>
<a name="3.5.4"></a>
#### 3.5.4 Gameplay Effect Modifiers
`Modifiers` change an `Attribute` and are the only way to predictively (**LINK TO PREDICTION**) change an `Attribute`. A `GameplayEffect` can have zero or many `Modifiers`. Each `Modifier` is responsible for changing only one `Attribute` via a specified operation.

| Operation  | Description                                                                                                         |
| ---------- | ------------------------------------------------------------------------------------------------------------------- |
| `Add`      | Adds the result to the `Modifier's` specified `Attribute`. Use a negative value for subtraction.                    |
| `Multiply` | Multiplies the result to the `Modifier's` specified `Attribute`.                                                    |
| `Divide`   | Divides the result against the `Modifier's` specified `Attribute`.                                                  |
| `Override` | Overrides the `Modifier's` specified `Attribute` with the result.                                                   |

The `CurrentValue` of an `Attribute` is the aggregate result of all of its `Modifiers` added to its `BaseValue`. The forumla for how `Modifiers` are aggregated is defined as follows in `FAggregatorModChannel::EvaluateWithBase` in `GameplayEffectAggregator.cpp`:
```c++
((InlineBaseValue + Additive) * Multiplicitive) / Division
```

Any `Override` `Modifiers` will override the final value with the last applied `Modifier` taking precedence.

**Note:** For percentage based changes, make sure to use the `Multiply` operation so that it happens after addition.

**Note:** Prediction (**LINK TO PREDICTION**) has trouble with percentage changes.

There are four types of `Modifiers`: Scalable Float, Attribute Based, Custom Calculation Class, and Set By Caller. They all generate some float value that is then used to change the specified `Attribute` of the `Modifier` based on its operation.

| `Modifier` Type            | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `Scalable Float`           | `FScalableFloats` are a structure that can point to a Data Table that has the variables as rows and levels as columns. The Scalable Floats will automatically read the value of the specified table row at the ability's current level (or different level if overriden on the `GameplayEffectSpec` (**LINK TO GAMEPLAYEFFECTSPEC**)). This value can further be manipulated by a coefficient. If no Data Table/Row is specified, it treats the value as a 1 so the coefficient can be used to hard code in a single value at all levels.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| `Attribute Based`          | `Attribute Based` `Modifiers` take the `CurrentValue` or `BaseValue` of a backing `Attribute` on the `Source` (who created the `GameplayEffectSpec`) or `Target` (who received the `GameplayEffectSpec`) and further modifies it with a coefficient and pre and post coefficient additions. `Snapshotting` means the backing `Attribute` is captured when the `GameplayEffectSpec` is created whereas no snapshotting means the `Attribute` is captured when the `GameplayEffectSpec` is applied.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
| `Custom Calculation Class` | `Custom Calculation Class` provides the most flexibility for complex `Modifiers`. This `Modifier` takes a `ModifierMagnitudeCalculation` (**LINK TO MODMAGCALC**) class and can further manipulate the resulting float value with a coefficient and pre and post coefficient additions.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| `Set By Caller`            | `SetByCaller` `Modifiers` are values that are set outside of the `GameplayEffect` at runtime by the ability or whoever made the `GameplayEffectSpec` on the `GameplayEffectSpec`. For example, you would use a `SetByCaller` if you want to set the damage to be based on how long the player held down a button to charge the ability. `SetByCallers` are essentially `TMap<FGameplayTag, float>` that live on the `GameplayEffectSpec`. The `Modifier` is just telling the `Aggregator` to look for a `SetByCaller` value associated with the supplied `GameplayTag`. The `SetByCallers` used by `Modifiers` can only use the `GameplayTag` version of the concept. The `FName` version is disabled here. If the `Modifier` is set to `SetByCaller` but a `SetByCaller` with the correct `GameplayTag` does not exist on the `GameplayEffectSpec`, the game will throw a runtime error and return a value of 0. This might cause issues in the case of a `Divide` operation. See `GamelayEffectSpec` (**LINK TO GAMEPLAYEFFECTSPEC/SETBYCALLER**) for more information on how to use `SetByCallers`. |

(**PICTURE OF SCALABLE FLOATS**)

<a name="concepts-ge-stacking"></a>
<a name="3.5.5"></a>
#### 3.5.5 Stacking
`GameplayEffects` by default will apply new instances of the `GameplayEffectSpec` that don't know or care about previously existing instances of the `GameplayEffectSpec` on application. `GameplayEffects` can be set to stack where instead of a new instance of the `GameplayEffectSpec` is added, the currently existing `GameplayEffectSpec's` stack count is changed. Stacking only works for `Duration` and `Infinite` `GameplayEffects`.

There are two types of stacking: Aggregate by Source and Aggregate by Target.

| Stacking Type       | Description                                                                                                                          |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| Aggregate by Source | There is a separate instance of stacks per Source `ASC` on the Target. Each Source can apply X amount of stacks.                     |
| Aggregate by Target | There is only one instance of stacks on the Target regardless of Source. Each Source can apply a stack up to the shared stack limit. |

Stacks also have policies for expiration, duration refresh, and period refresh. They have helpful hover tooltips in the `GameplayEffect` Blueprint.

The Sample Project includes a custom Blueprint node that listens for `GameplayEffect` stack changes. The HUD uses it to update the amount of passive armor stacks that the player has. See `AsyncTaskEffectStackChanged.h/cpp`.

(**PICTURE OF STACK CHANGE NODE**)

<a name="concepts-ge-ga"></a>
<a name="3.5.6"></a>
#### 3.5.6 Granted Abilities
`GameplayEffects` can grant new abilities (**LINK TO GAMEPLAYABILITIES**) to `ASCs`. Only `Duration` and `Infinite` `GameplayEffects` can grant abilities.

A common usecase for this is when you want to force another player to do something like moving them from a knockback or pull. You would apply a `GameplayEffect` to them that grants them an automatically activating ability (see passive abilities for how to automatically activate an ability when it is granted) (**LINK TO PASSIVE ABILITIES**) that does the desired action to them.

Designers can choose which abilities a `GameplayEffect` grants, what level to grant them at, what input to bind them at (**LINK TO INPUT BINDING IN ABILITIES**) and the removal policy for the granted ability.

| Removal Policy             | Description                                                                                                                                                                     |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Cancel Ability Immediately | The granted ability is canceled and removed immediately when the `GameplayEffect` that granted it is removed from the Target.                                                   |
| Remove Ability on End      | The granted ability is allowed to finish and then is removed from the Target.                                                                                                   |
| Do Nothing                 | The granted ability is not affected by the removal of the granting `GameplayEffect` from the Target. The Target has the ability permanently until it is manually removed later. |

<a name="concepts-ge-tags"></a>
<a name="3.5.7"></a>
#### 3.5.7 Gameplay Effect Tags
`GameplayEffects` carry multiple `GameplayTagContainers` (**LINK TO GAMPLAY TAGS**). Designers will edit the `Added` and `Removed` `GameplayTagContainers` for each category and the result will show up in the `Combined` `GameplayTagContainer` on compilation. `Added` tags are new tags that this `GameplayEffect` adds that its parents did not previously have. `Removed` tags are tags that parent classes have but this subclass does not have.

| Category                          | Description                                                                                                                                                                                                                                                                                                                                                                        |
| --------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Gameplay Effect Asset Tags        | Tags that the `GameplayEffect` has. They do not do any function on their own and serve only the purpose of describing the `GameplayEffect`.                                                                                                                                                                                                                                        |
| Granted Tags                      | Tags that live on the `GameplayEffect` but are also given to the `ASC` that the `GameplayEffect` is applied to. They are removed from the `ASC` when the `GameplayEffect` is removed. This only works for `Duration` and `Infinite` `GameplayEffects`.                                                                                                                             |
| Ongoing Tag Requirements          | Once applied, these tags determine whether the `GameplayEffect` is on or off. A `GameplayEffect` can be off and still be applied. If a `GameplayEffect` is off due to failing the Ongoing Tag Requirements, but the requirements are then met, the `GameplayEffect` will turn on again and reapply its modifiers. This only works for `Duration` and `Infinite` `GameplayEffects`. |
| Application Tag Requirements      | Tags on the Target that determine if a `GameplayEffect` can be applied to the Target. If these requirements are not met, the `GameplayEffect` is not applied.                                                                                                                                                                                                                      |
| Remove Gameplay Effects with Tags | `GameplayEffects` on the Target that have any of these tags in their `Asset Tags` or `Granted Tags` will be removed from the Target when this `GameplayEffect` is successfully applied.                                                                                                                                                                                            |

<a name="concepts-ge-immunity"></a>
<a name="3.5.8"></a>
#### 3.5.8 Immunity
`GameplayEffects` can grant immunity, effectively blocking the application of other `GameplayEffects`, based on `Gameplaytags` (**LINK TO GAMEPLAYTAGS**). While immunity can be effectively achieved through other means like `Application Tag Requirements`, using this system provides a delegate for when `GameplayEffects` are blocked due to immunity `UAbilitySystemComponent::OnImmunityBlockGameplayEffectDelegate`.

`GrantedApplicationImmunityTags` checks if the Source `ASC` (including tags from the Source ability's `AbilityTags` if there was one) has any of the specified tags. This is a way to provide immunity from all `GameplayEffects` from certain characters or sources based on their tags.

`Granted Application Immunity Query` checks the incoming `GameplayEffectSpec` if it matches any of the queries to block or allow its application.

The queries have helpful hover tooltips in the `GameplayEffect` Blueprint.

<a name="concepts-ge-spec"></a>
<a name="3.5.9"></a>
#### 3.5.9 Gameplay Effect Spec
The `GameplayEffectSpec` (`GESpec`) (**LINK TO API**) can be thought of as the instantiations of `GameplayEffects`. They hold a reference to the `GameplayEffect` class that they represent, what level it was created at, and who created it. These can be freely created and modified at runtime before application unlike `GameplayEffects` which should be created by designers prior to runtime. When applying a `GameplayEffect`, a `GameplayEffectSpec` is created from the `GameplayEffect` and that is actually what is applied to the Target.

`GameplayEffectSpecs` are created from `GameplayEffects` using `UAbilitySystemComponent::MakeOutgoingSpec()` which is `BlueprintCallable`. `GameplayEffectSpecs` do not have to be immediately applied. It is common to pass a `GameplayEffectSpec` to a projectile created from an ability that the projectile can apply to the target it hits later. When `GameplayEffectSpecs` are successfully applied, they return a new struct called `FActiveGameplayEffect`.

Notable `GameplayEffectSpec` Contents:
* The `GameplayEffect` class that this `GameplayEffect` was created from.
* The level of this `GameplayEffectSpec`. Usually the same as the level of the ability that created the `GameplayEffectSpec` but can be different.
* The duration of the `GameplayEffectSpec`. Defaults to the duration of the `GameplayEffect` but can be different.
* The period of the `GameplayEffectSpec` for periodic effects. Defaults to the period of the `GameplayEffect` but can be different.
* The current stack count of this `GameplayEffectSpec`. The stack limit is on the `GameplayEffect`.
* The `GameplayEffectContextHandle` (**LINK TO EFFECT CONTEXT**) tells us who created this `GameplayEffectSpec`.
* `Attributes` that were captured at the time of the `GameplayEffectSpec`'s creation due to snapshotting.
* `DynamicGrantedTags` that the `GameplayEffectSpec` grants to the Target in addition to the `GameplayTags` that the `GameplayEffect` grants.
* `DynamicAssetTags` that the `GameplayEffectSpect` has in addition to the `AssetTags` that the `GameplayEffect` has.
* `SetByCaller` `TMaps`.

<a name="concepts-ge-spec-setbycaller"></a>
##### SetByCallers
`SetByCallers` allow the `GameplayEffectSpec` to carry float values associated with a `GameplayTag` or `FName` around. They are stored in their respective `TMaps`: `TMap<FGameplayTag, float>` and `TMap<FName, float>` on the `GameplayEffectSpec`. These can be used to as `Modifiers` on the `GameplayEffect` or as generic means of ferrying floats around. It is common to pass numerical data generated inside of an ability to `GameplayEffectExecutionCalculations` (**LINK TO EXEC CALC**) or `ModifierMagnitudeCalculations` (**LINK TO MODMAGCALC**) via `SetByCallers`.

| `SetByCaller` Use | Notes                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Modifiers`       | Must be defined ahead of time in the `GameplayEffect` class. Can only use the `GameplayTag` version. If one is defined on the `GameplayEffect` class but the `GameplayEffectSpec` does not have the corresponding tag and float value pair, the game will have a runtime error on application of the `GameplayEffectSpec` and return 0. This is a potential problem for a `Divide` operation. See `Modifiers` (**LINK TO MODIFIERS**) |
| Elsewhere         | Does not need to be defined ahead of time anywhere. Reading a `SetByCaller` that does not exist on a `GameplayEffectSpec` can return a developer defined default value with optional warnings.                                                                                                                                                                                                                                        |

To assign `SetByCaller` values in Blueprint, use the Blueprint node for the version that you need (`GameplayTag` or `FName`):

(**PIC OF ASSIGNING SETBYCALLER**)

To read a `SetByCaller` value in Blueprint, you will need to make custom nodes in your Blueprint Library.

To assign `SetByCaller` values in C++, use the version of the function that you need (`GameplayTag` or `FName`):

```c++
void FGameplayEffectSpec::SetSetByCallerMagnitude(FName DataName, float Magnitude);
```
```c++
void FGameplayEffectSpec::SetSetByCallerMagnitude(FGameplayTag DataTag, float Magnitude);
```

To read a `SetByCaller` value in C++, use the version of the function that you need (`GameplayTag` or `FName`):

```c++
float GetSetByCallerMagnitude(FName DataName, bool WarnIfNotFound = true, float DefaultIfNotFound = 0.f) const;
```
```c++
float GetSetByCallerMagnitude(FGameplayTag DataTag, bool WarnIfNotFound = true, float DefaultIfNotFound = 0.f) const;
```

I recommend using the `GameplayTag` version over the `FName` version. This can prevent spelling errors in Blueprint and `GameplayTags` are more efficient to send over the network when the `GameplayEffectSpec` replicates than `FNames` since the `TMaps` replicate too.

<a name="concepts-ge-context"></a>
<a name="3.5.10"></a>
#### 3.5.10 Gameplay Effect Context
The `GameplayEffectContext` (**LINK TO API**) structure holds information about a `GameplayEffectSpec's` instigator and target location data. This is also a good structure to subclass to pass arbitrary data around between places like `ModifierMagnitudeCalculations` / `GameplayEffectExecutionCalculations` and the `AttributeSet`.

To subclass the `GameplayEffectContext`:

1. Subclass `FGameplayEffectContext`
1. Override `FGameplayEffectContext::GetScriptStruct()`
1. Override `FGameplayEffectContext::Duplicate()`
1. Override `FGameplayEffectContext::NetSerialize()` if your new data needs to be replicated
1. If you overrode `FGameplayEffectContext::NetSerialize()`, be sure to make the `TStructOpsTypeTraits` like the parent struct `FGameplayEffectContext` has
1. Override `AllocGameplayEffectContext()` in your `AbilitySystemGlobals` class (**LINK TO ABILITYSYSTEMGLOBALS**) to return a new object of your subclass

<a name="concepts-ge-mmc"></a>
<a name="3.5.11"></a>
#### 3.5.11 Modifier Magnitude Calculation
`ModifierMagnitudeCalculations` (`ModMagcCalc` or `MMC`) (**LINK TO API**) are poweful classes used as a `Modifier` (**LINK TO MODIFIERS**) in `GameplayEffects`. They function similarly to `GameplayEffectExecutionCalculations` (**LINK BELOW TO EXEC CALCS**) but are less powerful and most importantly they can be predicted (**LINK TO PREDICTION**). Their sole purpose is to return a float value from `CalculateBaseMagnitude_Implementation()`. You can subclass and override this function in Blueprint and C++.

`MMCs'` strength lies in their capability to capture the value of any number of `Attributes` on the `Source` or the `Target` of `GameplayEffect` with full access to the `GameplayEffectSpec` to read `GameplayTags` and `SetByCallers`. `Attributes` can either be snapshotted or not. Snapshotted `Attributes` are captured when the `GameplayEffectSpec` is created whereas non snapshotted `Attributes` are captured when the `GameplayEffectSpec` is applied. Capturing `Attributes` recalculates their `CurrentValue` from existing mods on the `ASC`. This recalculation will **not** run `PreAttributeChange()` (**LINK TO PREATTRIBUTECHANGE**) in the `AbilitySet` so any clamping must be done here again.

The resultant float from an `MMC` can futher be modified in the `GameplayEffect's` `Modifier` by a coefficient and a pre and post coefficient addition.

An example `MMC` that captures the `Target's` mana `Attribute` reduces it from a poison effect where the ammount reduced changes depending on how much mana the `Target` has and a tag that the `Target` might have:
```c++
float UPAMMC_PoisonMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	FGameplayEffectAttributeCaptureDefinition ManaDef;
	ManaDef.AttributeToCapture = UPAAttributeSetBase::GetManaAttribute();
	ManaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ManaDef.bSnapshot = false;

	FGameplayEffectAttributeCaptureDefinition MaxManaDef;
	MaxManaDef.AttributeToCapture = UPAAttributeSetBase::GetMaxManaAttribute();
	MaxManaDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxManaDef.bSnapshot = false;

	float Mana = 0.f;
	GetCapturedAttributeMagnitude(ManaDef, Spec, EvaluationParameters, Mana);
	Mana = FMath::Max<float>(Mana, 0.0f);

	float MaxMana = 0.f;
	GetCapturedAttributeMagnitude(MaxManaDef, Spec, EvaluationParameters, MaxMana);
	MaxMana = FMath::Max<float>(MaxMana, 1.0f); // Avoid divide by zero

	float Reduction = -20.0f;
	if (Mana / MaxMana > 0.5f)
	{
		// Double the effect if the target has more than half their mana
		Reduction *= 2;
	}
	
	if (TargetTags->HasTagExact(FGameplayTag::RequestGameplayTag(FName("Status.WeakToPoisonMana"))))
	{
		// Double the effect if the target is weak to PoisonMana
		Reduction *= 2;
	}
	
	return Reduction;
}
```

<a name="concepts-ge-ec"></a>
<a name="3.5.12"></a>
#### 3.5.12 Gameplay Effect Execution Calculation
`GameplayEffectExecutionCalculations` (`ExecutionCalculations` or `ExecCalcs`) (**LINK TO API**) are the most powerful way for `GameplayEffects` to make changes to an `ASC`. Like `ModifierMagnitudeCalculations` (**LINK ABOVE TO MMCs**), these can capture `Attributes` and optionally snapshot them. Unlike `MMCs`, these can change more than one `Attribute` and essentially do anything else that the programmer wants. The downside to this power and flexiblity is that they can not be predicted (**LINK TO PREDICTION**) and they must be implemented in C++.

Snapshotting captures the `Attribute` when the `GameplayEffectSpec` is created whereas not snapshotting captures the `Attribute` when the `GameplayEffectSpec` is applied. Capturing `Attributes` recalculates their `CurrentValue` from existing mods on the `ASC`. This recalculation will **not** run `PreAttributeChange()` (**LINK TO PREATTRIBUTECHANGE**) in the `AbilitySet` so any clamping must be done here again.

To set up `Attribute` capture, we follow a pattern set by Epic's ActionRPG Sample Project by defining a struct holding and defining how we capture the `Attributes` and creating one copy of it in the struct's constructor. You will have a struct like this for every `ExecCalc`. **Note:** Each struct needs a unique name as they share the same namespace. Using the same name for the structs will cause incorrect behavior in capturing your `Attributes` (mostly capturing the values of the wrong `Attributes`).

For `Local Predicted`, `Server Only`, and `Server Initiated` (**LINK ABILITIES**) abilities, the `ExecCalc` only calls on the Server.

Calculating damage received based on a complex formula reading from many attributes on the `Source` and the `Target` is the most common example of an `ExecCalc`. The included Sample Project has a simple `ExecCalc` for calculating damage that reads the value of damage from the `GameplayEffectSpec's` `SetByCaller` (**LINK TOSETBYCALLER **) and then mitigates that value based on the armor `Attribute` captured from the `Target`. See `GDDamageExecCalculation.cpp/.h`.

<a name="concepts-ge-car"></a>
<a name="3.5.13"></a>
#### 3.5.13 Custom Application Requirement
`CustomApplicationRequirement` (`CAR`) (**LINK TO API**) classes give the designers advanced control over whether a `GameplayEffect` can be applied versus the simple `GameplayTag` checks on the `GameplayEffect`. These can be implemented in Blueprint by overriding `CanApplyGameplayEffect()` and in C++ by overriding `CanApplyGameplayEffect_Implementation()`.

Examples of when to use `CARs`:
* `Target` needs to have a certain amount of an `Attribute`
* `Target` needs to have a certain number of stacks of a `GameplayEffect`

`CARs` can also do more advanced things like checking if an instance of this `GameplayEffect` is already on the `Target` and changing the duration (**LINK TO CHANGING ACTIVE GAMEPLAY EFFECT DURATION**) of the existing instance instead of applying a new instance (return false for `CanApplyGameplayEffect()`).

<a name="concepts-ge-cost"></a>
<a name="3.5.14"></a>
#### 3.5.14 Cost Gameplay Effect
`GameplayAbilities` (**LINK TO GAs**) have an optional `GameplayEffect` specifically designed to use as the cost of the ability. Costs are how much of an `Attribute` an `ASC` needs to have to be able to activate the `GameplayAbility`. If a `GA` cannot afford the `Cost GE`, then they will not be able to activate. This `Cost GE` should be an `Instant` `GameplayEffect` with one or more `Modifiers` that subtract from `Attributes`. By default, `Cost GEs` are meant to be predicted and it is recommended to maintain that capability meaning do not use `ExecutionCalculations`. `MMCs` are perfectly acceptable and encouraged for complex cost calculations.

When starting out, you will most likely have one unique `Cost GE` per `GA` that has a cost. A more advanced technique is to reuse one `Cost GE` for multiple `GAs` and just modify the `GameplayEffectSpec` created from the `Cost GE` with the `GA`-specific data (the cost value is defined on the `GA`). **This only works for `Instanced` abilities.**

Two techniques for reusing the `Cost GE`:

1. **Use an `MMC`.** This is the easiest method. Create an `MMC` (**LINK ABOVE TO MMC**) that reads the cost value from the `GameplayAbility` instance which you can get from the `GameplayEffectSpec`.

```c++
float UPGMMC_HeroAbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	const UPGGameplayAbility* Ability = Cast<UPGGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	return Ability->Cost.GetValueAtLevel(Ability->GetAbilityLevel());
}
```

In this example the cost value is an `FScalableFloat` on the `GameplayAbility` child class that I added to it.
```c++
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cost")
FScalableFloat Cost;
```

(**PIC OF GE WITH THIS MMC**)

2. **Override `UGameplayAbility::GetCostGameplayEffect()`.** Override this function and create a `GameplayEffect` at runtime (**LINK TO CREATING DYNAMIC GEs AT RUNTIME**) that reads the cost value on the `GameplayAbility`.

<a name="concepts-ge-cooldown"></a>
<a name="3.5.15"></a>
#### 3.5.15 Cooldown Gameplay Effect
`GameplayAbilities` (**LINK TO GAs**) have an optional `GameplayEffect specifically design to use as the cooldown of the abilitiy. Cooldowns determine how long after activation the ability can be activated again. If a `GA` is still on cooldown, it cannot activate. This `Cooldown GE` should be a `Duration` `GameplayEffect` with no `Modifiers` and a unique `GameplayTag` per `GameplayAbility` or per ability slot (if your game has interchangeable abilities assigned to slots that share a cooldown) in the `GameplayEffect's` `GrantedTags` ("`Cooldown Tag`"). The `GA` actually checks for the presence for the `Cooldown Tag` instead of the presence of the `Cooldown GE`. By default, `Cooldown GEs` are meant to be predicted and it is recommended to maintain that capability meaning do not use `ExecutionCalculations`. `MMCs` are perfectly acceptable and encouraged for complex cooldown calculations.

When starting out, you will most likely have one unique `Cooldown GE` per `GA` that has a cooldown. A more advanced technique is to reuse one `Cooldown GE` for multiple `GAs` and just modify the `GameplayEffectSpec` created from the `Cooldown GE` with `GA`-specific data (the cooldown duration and the `Cooldown Tag` are defined on the `GA`). **This only works for `Instanced` abilities.**

Two techniques for reusing the `Cooldown GE`:

1. **Use a `SetByCaller`. (LINK TO SETBYCALLER)** This is the easiest method. Set the duration of your shared `Cooldown GE` to `SetByCaller` with a `GameplayTag`. On your `GameplayAbility` subclass, define a float / `FScalableFloat` for the duration and a `FGameplayTagContainer` for the unique `Cooldown Tag`.
```c++
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
FScalableFloat CooldownDuration;

UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
FGameplayTagContainer CooldownTags;
```

Then override `UGameplayAbility::GetCooldownTags()` to inject our `Cooldown Tags` into the cooldown's `GameplayEffectSpec`.
```c++
const FGameplayTagContainer * UPGGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* Tags = new FGameplayTagContainer();
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		Tags->AppendTags(*ParentTags);
	}
	Tags->AppendTags(CooldownTags);
	return Tags;
}
```

Finally, override `UGameplayAbility::ApplyCooldown()` to inject our `Cooldown Tags` and to add the `SetByCaller` to the cooldown `GameplayEffectSpec`.
```c++
void UPGGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName(  OurSetByCallerTag  )), CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
```

2. **Use an `MMC`. (LINK TO MMC)** This has the same setup as above except for setting the `SetByCaller` as the duration on the `Cooldown GE` and in `ApplyCost`. Instead, set the duration to be a `Custom Calculation Class` and point to the new `MMC` that we will make.
```c++
UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
FScalableFloat CooldownDuration;

UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
FGameplayTagContainer CooldownTags;
```

Override `UGameplayAbility::GetCooldownTags()` to inject our `Cooldown Tags` into the cooldown's `GameplayEffectSpec`.
```c++
const FGameplayTagContainer * UPGGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* Tags = new FGameplayTagContainer();
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		Tags->AppendTags(*ParentTags);
	}
	Tags->AppendTags(CooldownTags);
	return Tags;
}
```

Finally, override `UGameplayAbility::ApplyCooldown()` to inject our `Cooldown Tags` into the cooldown `GameplayEffectSpec`.
```c++
void UPGGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
```

```c++
float UPGMMC_HeroAbilityCooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const
{
	const UPGGameplayAbility* Ability = Cast<UPGGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	return Ability->CooldownDuration.GetValueAtLevel(Ability->GetAbilityLevel());
}
```

(**PIC OF THE SHARED COOLDOWN GE**)

<a name="concepts-ge-cooldown-tr"></a>
##### Get the Cooldown Gameplay Effect's Remaining Time
```c++
bool APGPlayerState::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float & TimeRemaining, float & CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}
```

**Note:** Querying the cooldown's time remaining on clients requires that they can receive replicated `GameplayEffects`. This will depend on their `ASC's` replication mode (**LINK TO ASC replication mode**).

<a name="concepts-ge-cooldown-listen"></a>
##### Listening for Cooldown Begin and End
To listen for when a cooldown begins, you can either respond to when the `Cooldown GE` is applied by binding to `AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf` or when the `Cooldown Tag` is added by binding to `AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)`. I recommend listening for when the `Cooldown GE` is added because you also have access to the `GameplayEffectSpec` that applied it. From this you can determine if the `Cooldown GE` is the locally predicted one or the Server's correcting one.

To listen for when a cooldown ends, you can either respond to when the `Cooldown GE` is removed by binding to `AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate()` or when the `Cooldown Tag` is removed by binding to `AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved)`. I recommend listening for when the `Cooldown Tag` is removed because when the Server's corrected `Cooldown GE` comes in, it will remove our locally predicted one causing the `OnAnyGameplayEffectRemovedDelegate()` to fire even though we're still on cooldown. The `Cooldown Tag` will not change during the removal of the predicted `Cooldown GE` and the application of the Server's corrected `Cooldown GE`.

**Note:** Listening for a `GameplayEffect` to be added or removed on clients requires that they can receive replicated `GameplayEffects`. This will depend on their `ASC's` replication mode (**LINK TO ASC replication mode**).

The Sample Project includes a custom Blueprint node that listens for cooldowns begninning and ending. The HUD uses it to update the amount of time remaining on the Meteor's cooldown. See `AsyncTaskEffectCooldownChanged.h/cpp`.

(**PICTURE OF COOLDOWN CHANGE NODE**)

<a name="concepts-ge-cooldown-prediction"></a>
##### Predicting Cooldowns
Cooldowns cannot really be predicted currently. We can start UI cooldown timer's when the locally predicted `Cooldown GE` is applied but the `GameplayAbility's` actual cooldown is tied to the server's cooldown's time remaining. Depending on the player's latency, the locally predicted cooldown could expire but the `GameplayAbility` would still be on cooldown on the server and this would prevent the `GameplayAbility's` immediate re-activation until the server's cooldown expires.

The Sample Project handles this by graying out the Meteor ability's UI icon when the locally predicted cooldown begins and then starting the cooldown timer once the server's corrected `Cooldown GE` comes in.

A gameplay consequence of this is that players with high latencies have a lower rate of fire on short cooldown abilities than players with lower latencies putting them at a disadvantage. Fortnite avoids this by their weapons having custom bookkeeping that do not use cooldown `GameplayEffects`.

Allowing for true predicted cooldowns (player could activate a `GameplayAbility` when the local cooldown expires but the server is still on cooldown) is something that Epic would like to implement someday in a future iteration of GAS (**LINK TO DAVE RATTI'S RESPONSES**).

<a name="concepts-ge-duration"></a>
<a name="3.5.16"></a>
#### 3.5.16 Changing Active Gameplay Effect Duration
To change the time remaining for a `Cooldown GE` or any `Duration` `GameplayEffect`, we need to change the `GameplayEffectSpec's` `Duration`, update its `StartServerWorldTime`, update its `CachedStartServerWorldTime`, update its `StartWorldTime`, and rerun the check on the duration with `CheckDuration()`. Doing this on the server and marking the `FActiveGameplayEffect` dirty will replicate the changes to clients.
**Note:** This does involve a `const_cast` and may not be Epic's intended way of changing durations, but it seems to work well so far.

```c++
bool UPAAbilitySystemComponent::SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect(Handle);
	if (!ActiveGameplayEffect)
	{
		return false;
	}

	FActiveGameplayEffect* AGE = const_cast<FActiveGameplayEffect*>(ActiveGameplayEffect);
	if (NewDuration > 0)
	{
		AGE->Spec.Duration = NewDuration;
	}
	else
	{
		AGE->Spec.Duration = 0.01f;
	}

	AGE->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
	AGE->CachedStartServerWorldTime = AGE->StartServerWorldTime;
	AGE->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
	ActiveGameplayEffects.MarkItemDirty(*AGE);
	ActiveGameplayEffects.CheckDuration(Handle);

	AGE->EventSet.OnTimeChanged.Broadcast(AGE->Handle, AGE->StartWorldTime, AGE->GetDuration());
	OnGameplayEffectDurationChange(*AGE);

	return true;
}
```

<a name="concepts-ge-dynamic"></a>
<a name="3.5.17"></a>
#### 3.5.17 Creating Dynamic Gameplay Effects at Runtime
Creating Dynamic `GameplayEffects` at runtime is an advanced topic. You shouldn't have to do this too often.

Only `Instant` `GameplayEffects` can be created at runtime from scratch in C++. The Sample Project creates one to send the gold and experience points back to the killer of a character when it takes the killing blow in its `AttributeSet`.

```c++
// Create a dynamic instant Gameplay Effect to give the bounties
UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

int32 Idx = GEBounty->Modifiers.Num();
GEBounty->Modifiers.SetNum(Idx + 2);

FGameplayModifierInfo& InfoXP = GEBounty->Modifiers[Idx];
InfoXP.ModifierMagnitude = FScalableFloat(GetXPBounty());
InfoXP.ModifierOp = EGameplayModOp::Additive;
InfoXP.Attribute = UGDAttributeSetBase::GetXPAttribute();

FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
InfoGold.ModifierOp = EGameplayModOp::Additive;
InfoGold.Attribute = UGDAttributeSetBase::GetGoldAttribute();

Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
```

`Duration` and `Infinite` `GameplayEffects` cannot be created dynamically at runtime because when they replicate they look for the `GameplayEffect` class definition that does not exist. To achieve this functionality, you should instead make an archetype `GameplayEffect` class like you would normally do in the Editor. Then customize the `GameplayEffectSpec` instance with what you need at runtime.

<a name="concepts-ge-containers"></a>
<a name="3.5.18"></a>
#### 3.5.18 Gameplay Effect Containers
Epic's Action RPG Sample Project (**LINK TO ARPG SAMPLE**) implements a structure called `FGameplayEffectContainer`. These are not in vanilla GAS but are extremely handy for containing `GameplayEffects` and `TargetData`. It automates a some of the effort like creating `GameplayEffectSpecs` from `GameplayEffects` and setting default values in its `GameplayEffectContext`. Making a `GameplayEffectContainer` in a `GameplayAbility` and passing it to spawned projectiles is very easy and straightforward. I opted not to implement the `GameplayEffectContainers` in the included Sample Project to show how you would work without them in vanilla GAS, but I highly recommend looking into them and considering adding them to your project.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-ga"></a>
<a name="3.6"></a>
### 3.6 Gameplay Abilities

<a name="concepts-ga-definition"></a>
<a name="3.6.1"></a>
#### 3.6.1 Gameplay Abliity Definition
`GameplayAbilities` (`GAs`) (**LINK TO API**) are any actions or skills that an `Actor` can do in the game. More than one `GameplayAbility` can be active at one time for example sprinting and shooting a gun. These can be made in Blueprint or C++.

Examples of `GameplayAbilities`:
* Jumping
* Sprinting
* Shooting a gun
* Passively blocking an attack every X number of seconds
* Using a potion
* Opening a door
* Collecting a resource
* Constructing a building

Things that should not be implemented with `GameplayAbilities`:
* Basic movement input
* Some interactions with UIs - Don't use a `GameplayAbility` to purchase an item from a store.

These are not rules, just my recommendations. Your design and implementations may vary.

`GameplayAbilities` come with default functionality to have a level to modify the amount of change to attributes or to change the `GameplayAbility's` functionality.

`GameplayAbilities` run on the owning client and/or the server but not simulated proxies depending on the `Net Execution Policy` (**LINK TO NET EXECUTION POLICY BELOW**). The `Net Execution Policy` determines if a `GameplayAbility` will be locally predicted (**LINK TO PREDICTION**). They include default behavior for optional cost and cooldown `GameplayEffects` (**LINK DOWN BELOW TO COST AND COOLDOWNS**). `GameplayAbilities` use `AbilityTasks` (**LINK DOWN BELOW TO ABILITYTASKS**) for actions that happen over time like waiting for an event, waiting for an attribute change, waiting for players to choose a target, or moving a `Character` with `Root Motion Source`.

All `GameplayAbilities` will have their `ActivateAbility()` function overriden with your gameplay logic. Additional logic can be added to `EndAbility()` that runs when the `GameplayAbility` completes or is canceled.

**FLOW CHART OF EXAMPLE ABILITY** 
- When prediction key becomes stale

Complex abilities can be implemented using multiple `GameplayAbilities` that interact (activate, cancel, etc) with each other.

<a name="concepts-ga-definition-reppolicy"></a>
##### Replication Policy
Don't use this option. The name is misleading and you don't need it. `GameplayAbilitySpecs` (**LINK TO ABILITY SPECS**) are replicated from the server to the owning client by default. Dave Ratti from Epic has stated his desire to remove it in the future (**LINK TO RESPONSES FROM EPIC**).

<a name="concepts-ga-definition-remotecancel"></a>
##### Server Respects Remote Ability Cancellation
This option causes trouble more often than not. It means if the client's `GameplayAbility` ends either due to cancellation or natural completion, it will force the server's version to end whether it completed or not. The latter issue is the important one, especially for locally predicted `GameplayAbilities` used by players with high latencies. Generally you will want to disable this option.

<a name="concepts-ga-definition-repinputdirectly"></a>
##### Replicate Input Directly
Setting this option will always replicate input press and release events to the server. Epic recommends not using this and instead relying on the `Generic Replicated Events` that are built into the existing input related `AbilityTasks` (**LINK TO ABILITY TASKS**) if you have your input bound to your `ASC` (**LINK BELOW TO BINDING INPUT TO ASC**).

Epic's comment:
```c++
/** Direct Input state replication. These will be called if bReplicateInputDirectly is true on the ability and is generally not a good thing to use. (Instead, prefer to use Generic Replicated Events). */
UAbilitySystemComponent::ServerSetInputPressed()
```

<a name="concepts-ga-input"></a>
<a name="3.6.1"></a>
#### 3.6.1 Binding Input to the ASC
The `ASC` allows you to directly bind input actions to it and assign those inputs to `GameplayAbilities` when you grant them. Input actions assigned to `GameplayAbilities` automatically activate those `GameplayAbilities` when pressed if the `GameplayTag` requirements are met. Assigned input actions are required to use the built-in `AbilityTasks` that respond to input.

In addition to input actions assigned to activate `GameplayAbilities`, the `ASC` also accepts generic `Confirm` and `Cancel` inputs. These special inputs are used by `AbilityTasks` for confirming things like `TargetData` or canceling them.

To bind input to an `ASC`, you must first create an enum that translates the input action name to a byte. The enum name must match exactly to the name used for the input action in the project settings. The `DisplayName` does not matter.

From the Sample Project:
```c++
UENUM(BlueprintType)
enum class EGDAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3 LMB
	Ability1		UMETA(DisplayName = "Ability1"),
	// 4 RMB
	Ability2		UMETA(DisplayName = "Ability2"),
	// 5 Q
	Ability3		UMETA(DisplayName = "Ability3"),
	// 6 E
	Ability4		UMETA(DisplayName = "Ability4"),
	// 7 R
	Ability5		UMETA(DisplayName = "Ability5"),
	// 8 Sprint
	Sprint			UMETA(DisplayName = "Sprint"),
	// 9 Jump
	Jump			UMETA(DisplayName = "Jump")
};
```

In your `SetupPlayerInputComponent()` include the function for binding to the `ASC`:
```c++
// Bind to AbilitySystemComponent
AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("EGDAbilityInputID"), static_cast<int32>(EGDAbilityInputID::Confirm), static_cast<int32>(EGDAbilityInputID::Cancel)));
```

**Note:** In the Sample Project `Confirm` and `Cancel` in the enum don't match the input action names in the project settings (`ConfirmTarget` and `CancelTarget`), but we supply the mapping between them in `BindAbilityActivationToInputComponent()`. These are special since we supply the mapping and they don't have to match, but they can match. All other inputs in the enum must match the input action names in the project settings.

For `GameplayAbilities` that will only ever be activated by one input (they will always exist in the same "slot" like a MOBA), I prefer to add a variable to my `UGameplayAbility` subclass where I can define their input. I can then read this from the `ClassDefaultObject` when granting the ability.

<a name="concepts-ga-granting"></a>
<a name="3.6.1"></a>
#### 3.6.1 Granting Abilities
Granting a `GameplayAbility` to an `ASC` adds it to the `ASC's` list of `ActivatableAbilities` allowing it to activate the `GameplayAbility` at will if it meets the `GameplayTag` requirements (**LINK TO ABILITY TAGS**).

We grant `GameplayAbilities` on the server which then automatically replicates the `GameplayAbilitySpec` (**LINK TO ABILITY SPEC**) to the owning client. Other clients / simulated proxies do not receive the `GameplayAbilitySpec`.

The Sample Project stores a `TArray<TSubclassOf<UGDGameplayAbility>>` on the `Character` class that it reads from and grants when the game starts:
```c++
void AGDCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (Role != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UGDGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}
```

When granting these `GameplayAbilities`, we're creating `GameplayAbilitySpecs` with the `UGameplayAbility` class, the ability level, the input that it is bound to, and the `SourceObject` or who gave this `GameplayAbility` to this `ASC`.

<a name="concepts-ga-activating"></a>
<a name="3.6.1"></a>
#### 3.6.1 Activating Abilities
If a `GameplayAbility` is assigned an input action, it will be automatically activated if the input is pressed and it meets its `GameplayTag` requirements. This may not always be the desirable way to activate a `GameplayAbility`. The `ASC` provides four other methods of activating `GameplayAbilities`: by `GameplayTag`, `GameplayAbility` class, `GameplayAbilitySpec` handle, and by an event. Activating a `GameplayAbility` by event allows you to pass in a payload of data with the event (**LINK TO PASSING DATA TO ABILITIES**).

```c++
UFUNCTION(BlueprintCallable, Category = "Abilities")
bool TryActivateAbilitiesByTag(const FGameplayTagContainer& GameplayTagContainer, bool bAllowRemoteActivation = true);

UFUNCTION(BlueprintCallable, Category = "Abilities")
bool TryActivateAbilityByClass(TSubclassOf<UGameplayAbility> InAbilityToActivate, bool bAllowRemoteActivation = true);

bool TryActivateAbility(FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);

bool TriggerAbilityFromGameplayEvent(FGameplayAbilitySpecHandle AbilityToTrigger, FGameplayAbilityActorInfo* ActorInfo, FGameplayTag Tag, const FGameplayEventData* Payload, UAbilitySystemComponent& Component);

FGameplayAbilitySpecHandle GiveAbilityAndActivateOnce(const FGameplayAbilitySpec& AbilitySpec);
```

Activation sequence for **locally predicted** `GameplayAbilities`:
1. **Owning client** calls `TryActivateAbility()`
1. Calls `InternalTryActivateAbility()`
1. Calls `CanActivateAbility()` and returns whether `GameplayTag` requirements are met, if the `ASC` can afford the cost, if the `GameplayAbility` is not on cooldown, and if no other instances are currently active
1. Calls `CallServerTryActivateAbility()` and passes it the `Prediction Key` that it generates
1. Calls `CallActivateAbility()`
1. Calls `PreActivate()` Epic refers to this as "boilerplate init stuff"
1. Calls `ActivateAbility()` finally activating the ability

**Server** receives `CallServerTryActivateAbility()`
1. Calls `ServerTryActivateAbility()`
1. Calls `InternalServerTryActiveAbility()` 
1. Calls `InternalTryActivateAbility()`
1. Calls `CanActivateAbility()` and returns whether `GameplayTag` requirements are met, if the `ASC` can afford the cost, if the `GameplayAbility` is not on cooldown, and if no other instances are currently active
1. Calls `ClientActivateAbilitySucceed()` if successful telling it to update its `ActivationInfo` that its activation was confirmed by the server and broadcasting the `OnConfirmDelegate` delegate. This is not the same as input confirmation.
1. Calls `CallActivateAbility()`
1. Calls `PreActivate()` Epic refers to this as "boilerplate init stuff"
1. Calls `ActivateAbility()` finally activating the ability

If at any time the server fails to activate, it will call `ClientActivateAbilityFailed()`, immediately terminating the client's `GameplayAbility` and undoing any predicted changes.

To activate a `GameplayAbility` by event, the `GameplayAbility` must have its `Triggers` set up in the `GameplayAbility`. Assign a `GameplayTag` and pick an option for `GameplayEvent`. To send the event, use the function `UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)`. Activating a `GameplayAbility` by event allows you to pass in a payload with data.

`GameplayAbility` `Triggers` also allow you to activate the `GameplayAbility` when a `GameplayTag` is added or removed.

**Note:** When activating a `GameplayAbility` from event in Blueprint, you must use the `ActivateAbilityFromEvent` node and the standard `ActivateAbility` node **cannot exist** in your graph. If the `ActivateAbility` node exists, it will always be called over the `ActivateAbilityFromEvent` node.

**Note:** Don't forget to call `EndAbility()` when the `GameplayAbility` should terminate unless you have a `GameplayAbility` that will always run like a passive ability.

<a name="concepts-ga-activating-passive"></a>
##### Passive Abilities
To implement passive `GameplayAbilities` that automatically activate and run continuously, override `UGameplayAbility::OnAvatarSet()` which is automatically called when a `GameplayAbility` is granted and the `AvatarActor` is set and call `TryActivateAbility()`.

I recommend adding a `bool` to your custom `UGameplayAbility` class specifying if the `GameplayAbility` should be activated when granted. The Sample Project does this for its passive armor stacking ability.

Passive `GameplayAbilitites` will typically have a `Net Execution Policy` of `Server Only` (**LINK TO NET EXECUTION POLICY**).

```c++
void UGDGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilitySpec & Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
```

Epic describes this function as the correct place to initiate passive abilities and to do `BeginPlay` type things.

<a name="concepts-ga-input"></a>
<a name="3.6.1"></a>
#### 3.6.1 Canceling Abilities
To cancel a `GameplayAbility` from within, you call `CancelAbility()`. This will call `EndAbility()` and set its `WasCancelled` parameter to true.

To cancel a `GameplayAbility` externally, the `ASC` provides a few functions:

```c++
/** Cancels the specified ability CDO. */
void CancelAbility(UGameplayAbility* Ability);	

/** Cancels the ability indicated by passed in spec handle. If handle is not found among reactivated abilities nothing happens. */
void CancelAbilityHandle(const FGameplayAbilitySpecHandle& AbilityHandle);

/** Cancel all abilities with the specified tags. Will not cancel the Ignore instance */
void CancelAbilities(const FGameplayTagContainer* WithTags=nullptr, const FGameplayTagContainer* WithoutTags=nullptr, UGameplayAbility* Ignore=nullptr);

/** Cancels all abilities regardless of tags. Will not cancel the ignore instance */
void CancelAllAbilities(UGameplayAbility* Ignore=nullptr);

/** Cancels all abilities and kills any remaining instanced abilities */
virtual void DestroyActiveState();
```

**Note:** I have found that `CancelAllAbilities` doesn't seem to work right if you have a `Non-Instanced` `GameplayAbilities`. It seems to hit the `Non-Instanced` `GameplayAbility` and give up. `CancelAbilities` can handle `Non-Instanced` `GameplayAbilities` better and that is what the Sample Project uses (Jump is a non-instanced `GameplayAbility`). Your mileage may vary.

<a name="concepts-ga-definition-activeability"></a>
<a name="3.6.1"></a>
#### 3.6.1 Getting Active Abilities
Beginners often ask "How can I get the active ability?" perhaps to set variables on it or to cancel it. More than one `GameplayAbility` can be active at a time so there is no one "active ability". Instead, you must search through an `ASC's` list of `ActivatableAbilities` (granted `GameplayAbilities` that the `ASC` owns) and find the one matching the `GameplayTag` (**LINK TO ABILITY TAGS**) that you are looking for.

`UAbilitySystemComponent::GetActivatableAbilities()` returns a `TArray<FGameplayAbilitySpec>` for you to iterate over.

The `ASC` also has another helper function that takes in a `GameplayTagContainer` as a parameter to assist in searching instead of manually iterating over the list of `GameplayAbilitySpecs`. The `bOnlyAbilitiesThatSatisfyTagRequirements` parameter will only return `GameplayAbilitySpecs` that satisfy their `GameplayTag` requirements and could be activated right now. For example, you could have two basic attack `GameplayAbilities`, one with a weapon and one with bare fists, and the correct one activates depending on if a weapon is equipped setting the `GameplayTag` requirement. See Epic's comment on the function for more information.
```c++
UAbilitySystemComponent::GetActivatableGameplayAbilitySpecsByAllMatchingTags(const FGameplayTagContainer& GameplayTagContainer, TArray < struct FGameplayAbilitySpec* >& MatchingGameplayAbilities, bool bOnlyAbilitiesThatSatisfyTagRequirements = true)
```

Once you get the `FGameplayAbilitySpec` that you are looking for, you can call `IsActive()` on it.

<a name="concepts-ga-instancing"></a>
<a name="3.6.1"></a>
#### 3.6.1 Instancing Policy
A `GameplayAbility's` `Instancing Policy` determines if and how the `GameplayAbility` is instanced when activated.

| `Instancing Policy`     | Description                                                                                      | Example of when to use                                                                                                                                                                                                                                                                                                                                                                                             |
| ----------------------- | ------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Instanced Per Actor     | Each `ASC` only has one instance of the `GameplayAbility` that is reused between activations.    | This will probably be the `Instancing Policy` that you use the most. You can use it for any ability and provides persistence between activations. The designer is responsible for manually resetting any variables between activations that need it.                                                                                                                                                               |
| Instanced Per Execution | Every time a `GameplayAbility` is activated, a new instance of the `GameplayAbility` is created. | The benefit of these `GameplayAbilitites` is that the variables are reset everytime you activate. These provide worse performance than `Instanced Per Actor` since they will spawn new `GameplayAbilities` every time they activate. The Sample Project does not use any of these.                                                                                                                                 |
| Non-Instanced           | The `GameplayAbility` operates on its `ClassDefaultObject`. No instances are created.            | This has the best performance of the three but is the most restrictive in what can be done with it. `Non-Instanced` `GameplayAbilities` cannot store state, meaning no dynamic variables and no binding to `AbilityTask` delegates. The best place to use them is for frequently used simple abilities like minion basic attacks in a MOBA or RTS. The Sample Project's Jump `GameplayAbility` is `Non-Instanced`. |

<a name="concepts-ga-net"></a>
<a name="3.6.1"></a>
#### 3.6.1 Net Execution Policy
A `GameplayAbility's` `Net Execution Policy` determines who runs the `GameplayAbility` and in what order.

| `Net Execution Policy` | Description                                                                                                                                                                                                                 |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Local Only`           | The `GameplayAbility` is only run on the owning client. This could be useful for abilities that only make local cosmetic changes. Single player games should use `Server Only`.                                             |
| `Local Predicted`      | `Local Predicted` `GameplayAbilities` activate first on the owning client and then on the server. The server's version will correct anything that the client predicted incorrectly. See Prediction (**LINK TO PREDICTION**) |
| `Server Only`          | The `GameplayAbility` is only run on the server. Passive `GameplayAbilities` will typically be `Server Only`. Single player games should use this.                                                                          |
| `Server Initiated`     | `Server Initiated` `GameplayAbilities` activate first on the server and then on the owning client. I personally haven't used these much if any.                                                                             |

<a name="concepts-ga-tags"></a>
<a name="3.6.2"></a>
#### 3.6.2 Ability Tags
`GameplayAbilities` come with `GameplayTagContainers` with built-in logic. None of these `GameplayTags` are replicated.

| `GameplayTag Container`     | Description                                                                                                                                                                                   |
| --------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Ability Tags`              | `GameplayTags` that the `GameplayAbility` owns. These are just `GameplayTags` to describe the `GameplayAbility`.                                                                              |
| `Cancel Abilities with Tag` | Other `GameplayAbilities` that have these `GameplayTags` in their `Ability Tags` will be canceled when this `GameplayAbility` is activated.                                                   |
| `Block Abilities with Tag`  | Other `GameplayAbilities` that have these `GameplayTags` in their `Ability Tags` are blocked from activating while this `GameplayAbility` is active.                                          |
| `Activation Owned Tags`     | These `GameplayTags` are given to the `GameplayAbility's` owner while this `GameplayAbility` is active. Remember these are not replicated.                                                    |
| `Activation Required Tags`  | This `GameplayAbility` can only be activated if the owner has **all** of these `GameplayTags`.                                                                                                |
| `Activation Blocked Tags`   | This `GameplayAbility` cannot be activated if the owner has **any** of these `GameplayTags`.                                                                                                  |
| `Source Required Tags`      | This `GameplayAbility` can only be activated if the `Source` has **all** of these `GameplayTags`. The `Source` `GameplayTags` are only set if the `GameplayAbility` is triggered by an event. |
| `Source Blocked Tags`       | This `GameplayAbility` cannot be activated if the `Source` has **any** of these `GameplayTags`. The `Source` `GameplayTags` are only set if the `GameplayAbility` is triggered by an event.   |
| `Target Required Tags`      | This `GameplayAbility` can only be activated if the `Target` has **all** of these `GameplayTags`. The `Target` `GameplayTags` are only set if the `GameplayAbility` is triggered by an event. |
| `Target Blocked Tags`       | This `GameplayAbility` cannot be activated if the `Target has **any** of these `GameplayTags`. The `Target` `GameplayTags` are only set if the `GameplayAbility` is triggered by an event.    |

<a name="concepts-ga-spec"></a>
<a name="3.6.1"></a>
#### 3.6.1 Gameplay Ability Spec
A `GameplayAbilitySpec` exists on the `ASC` after a `GameplayAbility` is granted and defines the activatable `GameplayAbility` - `GameplayAbility` class, level, input bindings, and runtime state that must be kept separate from the `GameplayAbility` class.

When a `GameplayAbility` is granted on the server, the server replicates the `GameplayAbilitySpec` to the owning client so that she may activate it.

Activating a `GameplayAbilitySpec` will create an instance (or not for `Non-Instanced` `GameplayAbilities`) of the `GameplayAbility` depending on its `Instancing Policy`.

<a name="concepts-ga-data"></a>
<a name="3.6.1"></a>
#### 3.6.1 Passing Data to Abilities
The general paradigm for `GameplayAbilities` is `Activate->Generate Data->Apply->End`. Sometimes you need to act on existing data. GAS provides a few options for getting external data into your `GameplayAbilities`:

| Method                                          | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| ----------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Activate `GameplayAbility` by Event             | Activate a `GameplayAbility` with an event containing a payload of data. The event's payload is replicated from client to server for local predicted `GameplayAbilities`. Use the two `Optional Object` variables for arbitrary data that does not fit any of the existing variables. The downside to this is that it prevents you from activating the ability with an input bind. To activate a `GameplayAbility` by event, the `GameplayAbility` must have its `Triggers` set up in the `GameplayAbility`. Assign a `GameplayTag` and pick an option for `GameplayEvent`. To send the event, use the function `UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)`. |
| Use `WaitGameplayEvent` `AbilityTask`           | Use the `WaitGameplayEvent` `AbilityTask` to tell the `GameplayAbility` to listen for an event with payload data after it activates. The event payload and the process to send it is the same as activating `GameplayAbilities` by event. The downside to this is that events are not replicated by the `AbilityTask` and should only be used for `Local Only` and `Server Only` `GameplayAbilities`. You potentially could write your own `AbilityTask` that will replicate the event payload.                                                                                                                                                                                                                                               |
| Store Data on the `OwnerActor` or `AvatarActor` | Use replicated variables stored on the `OwnerActor`, `AvatarActor`, or any other object that you can get a reference to. This method is the most flexible and will work with `GameplayAbilities` activated by input binds. However, it does not guarantee the data will be synchronized from replication at the time of use. You must ensure that ahead of time - meaning if you set a replicated variable and then immediately activate a `GameplayAbility` there is no guarantee the order that will happen on the receiver due to potential packet loss.                                                                                                                                                                                   |

<a name="concepts-ga-commit"></a>
<a name="3.6.1"></a>
#### 3.6.1 Ability Cost and Cooldown
`GameplayAbilities` come with functionality for optional costs and cooldowns. Costs are predefined amounts of `Attributes` that the `ASC` must have in order to activate the `GameplayAbility` implemented with an `Instant` `GameplayEffect` (**LINK TO COST GE**). Cooldowns are timers that prevent the reactivation of a `GameplayAbility` until it expires and is implemented with a `Duration` `GameplayEffect` (**LINK TO COOLDOWN GE**).

Before a `GameplayAbility` calls `UGameplayAbility::Activate()`, it calls `UGameplayAbility::CanActivateAbility()`. This function checks if the owning `ASC` can afford the cost (`UGameplayAbility::CheckCost()`) and ensures that the `GameplayAbility` is not on cooldown (`UGameplayAbility::CheckCooldown()`).

After a `GameplayAbility` calls `Activate()`, it can optionally commit the cost and cooldown at any time using `UGameplayAbility::CommitAbility()` which calls `UGameplayAbility::CommitCost()` and `UGameplayAbility::CommitCooldown()`. The designer may choose to call `CommitCost()` or `CommitCooldown()` separately if they shouldn't be committed at the same time. Commiting cost and cooldown calls `CheckCost()` and `CheckCooldown()` one more time and is the last chance for the `GameplayAbility` to fail related to them. The owning `ASC's` `Attributes` could potentially change after a `GameplayAbility` is activated, failing to meet the cost at time of commit. Committing the cost and cooldown can be locally predicted if the prediction key is valid at the time of commit. (**LINK TO PREDICTION**).

See `CostGE` (**LINK TO COSTGE**) and `CooldownGE` (**LINK TO COOLDOWN GE**) for implementation details.

<a name="concepts-ga-leveling"></a>
<a name="3.6.1"></a>
#### 3.6.1 Leveling Up Abilities
There are two common methods for leveling up an ability:

| Level Up Method                            | Description                                                                                                                                                                                                      |
| ------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Ungrant and Regrant at the New Level       | Ungrant (remove) the `GameplayAbility` from the `ASC` and regrant it back at the next level on the server. This terminates the `GameplayAbility` if it was active at the time.                                   |
| Increase the `GameplayAbilitySpec's` Level | On the server, find the `GameplayAbilitySpec`, increase its level, and mark it dirty so that replicates to the owning client. This method does not terminate the `GameplayAbility` if it was active at the time. |

The main difference between the two methods is if you want active `GameplayAbilitites` to be canceled at the time of level up. You will most likely use both methods depending on your `GameplayAbilities`. I recommend adding a `bool` to your `UGameplayAbility` subclass specifying which method to use.

<a name="concepts-ga-sets"></a>
<a name="3.6.1"></a>
#### 3.6.1 Ability Sets
`GameplayAbilitySets` are convenience `UDataAsset` classes for holding input bindings and lists of startup `GameplayAbilities` for Characters with logic to grant the `GameplayAbilities`. Subclasses can also include extra logic or properties. Paragon had a `GameplayAbilitySet` per hero that included all of their given `GameplayAbilities`.

I find this class to be unnecessary at least given what I've seen of it so far. The Sample Project handles all of the functionality of `GameplayAbilitySets` inside of the `GDCharacterBase` and its subclasses.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-at"></a>
<a name="3.7"></a>
### 3.7 Ability Tasks

<a name="concepts-at-definition"></a>
<a name="3.7.1"></a>
### 3.7.1 Ability Task Definition
`GameplayAbilities` only execute in one frame. This does not allow for much flexibility on its own. To do actions that happen over time or require responding to delegates fired at some point later in time we use latent actions called `AbilityTasks`.

GAS comes with many `AbilityTasks` out of the box:
* Tasks for moving Characters with `RootMotionSource`
* A task for playing animation montages
* Tasks for responding to `Attribute` changes
* Tasks for responding to `GameplayEffect` changes
* Tasks for responding to player input
* and more

The `AbilitySystemGlobals` (**LINK TO ABILITYSYSTEMGLOBALS**) enforces a hardcoded game-wide maximum of 1000 concurrent `AbilityTasks` running at the same time. Keep this in mind when designing `GameplayAbilities` for games that can have hundreds of characters in the world at the same time like RTS games.

<a name="concepts-at-definition"></a>
<a name="3.7.2"></a>
### 3.7.2 Custom Ability Tasks
Often you will be creating your own custom `AbilityTasks` (in C++). The Sample Project comes with two custom `AbilityTasks`:
1. `PlayMontageAndWaitForEvent` is a combination of the default `PlayMontageAndWait` and `WaitGameplayEvent` `AbilityTasks`. This allows animation montages to send gameplay events from `AnimNotifies` back to the `GameplayAbility` that started them. Use this to trigger actions at specific times during animation montages.
1. `WaitReceiveDamage` listens for the `OwnerActor` to receive damage. The passive armor stacks `GameplayAbility` removes a stack of armor when the hero receives an instance of damage.

`AbilityTasks` are composed of:
* A static function that creates new instances of the `AbilityTask`
* Delegates that are broadcasted on when the `AbilityTask` completes its purpose
* An `Activate()` function to start its main job, bind to external delegates, etc.
* An `OnDestroy()` function for cleanup, including external delegates that it bound to
* Callback functions for any external delegates that it bound to
* Member variables and any internal helper functions

**Note:** `AbilityTasks` can only declare one type of output delegate. All of your output delegates must be of this type, regardless if they use the parameters or not. Pass default values for unused delegate parameters.

<a name="concepts-at-using"></a>
<a name="3.7.3"></a>
### 3.7.3 Using Ability Tasks
To create and activate an `AbilityTask` in C++ (From `GDGA_FireGun.cpp`):
```c++
UGDAT_PlayMontageAndWaitForEvent* Task = UGDAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
Task->OnBlendOut.AddDynamic(this, &UGDGA_FireGun::OnCompleted);
Task->OnCompleted.AddDynamic(this, &UGDGA_FireGun::OnCompleted);
Task->OnInterrupted.AddDynamic(this, &UGDGA_FireGun::OnCancelled);
Task->OnCancelled.AddDynamic(this, &UGDGA_FireGun::OnCancelled);
Task->EventReceived.AddDynamic(this, &UGDGA_FireGun::EventReceived);
Task->ReadyForActivation();
```

In Blueprint, we just use the Blueprint node that we create for the `AbilityTask`. We don't have to call `ReadyForActivate()`. That is automatically called by `Engine/Source/Editor/GameplayTasksEditor/Private/K2Node_LatentGameplayTaskCall.cpp`. `K2Node_LatentGameplayTaskCall` also automatically calls `BeginSpawningActor()` and `FinishSpawningActor()` if they exist in your `AbilityTask` class (see `AbilityTask_WaitTargetData`). To reiterate, `K2Node_LatentGameplayTaskCall` only does automagic sorcery for Blueprint. In C++, we have to manually call `ReadyForActivation()`, `BeginSpawningActor()`, and `FinishSpawningActor()`.

(**PIC OF BLUEPRINT ABILITY TASK**)

To manually cancel an `AbilityTask`, just call `EndTask()` on the `AbilityTask` object in Blueprint (called `Async Task Proxy`) or C++.

Some `AbilityTasks` don't automatically end when the `GameplayAbility` ends like `WaitTargetData`. These should be manually ended in the `GameplayAbility's` `OnEndAbility` if they're still running (`WaitTargetData` naturally ends when the user presses `Confirm` or `Cancel` inputs).

<a name="concepts-at-rms"></a>
<a name="3.7.4"></a>
### 3.7.4 Root Motion Source Ability Tasks
GAS comes with `AbilityTasks` for moving `Characters` over time for things like knockbacks, complex jumps, pulls, and dashes using `Root Motion Sources` hooked into the `CharacterMovementComponent`.

Prior to UE 4.20, these `RootMotionSource` `AbilityTasks` worked predictively with the `CharacterMovementComponent`. Something changed in UE 4.20 that broke `Root Motion Source` `AbilityTasks's` prediction or changed it in a way that we don't know how to use to use it correctly. We've reached out to Epic but they haven't responded yet. I fear that we will have to wait for the completion of the new Network Prediction (**LINK TO NETWORK PREDICTION PLUGIN IN PREDICTION**) plugin before this functionality is restored to GAS.

For now if you need a truly predicted movement ability, you will need to manually implement it in the `CharacterMovementComponent`.

`RootMotionSource` `AbilityTasks` still work great in single player and are acceptable in low latency multiplayer games.

The Sample Project uses a `AbilityTask_ApplyRootMotionConstantForce` for its Dash `GameplayAbility`.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-gc"></a>
<a name="3.8"></a>
### 3.8 Gameplay Cues

<a name="concepts-gc-definition"></a>
<a name="3.8.1"></a>
#### 3.8.1 Gameplay Cue Definition
`GameplayCues` (`GCs`) (**LINK TO API**) execute non-gameplay related things like sound effects, particle effects, camera shakes, etc. `GameplayCues` are typically replicated (unless explicitly `Executed`, `Added`, or `Removed` locally) and predicted.

We trigger `GameplayCues` by sending a corresponding `GameplayTag` with the mandatory parent name of `GameplayCue.` and an event type (`Execute`, `Add`, or `Remove`) to the `GameplayCueManager` via the `ASC`. `GameplayCueNotify` objects and other `Actors` that implement the `IGameplayCueInterface` can subscribe to these events based on the `GameplayCue's` `GameplayTag` (`GameplayCueTag`).

There are two classes of `GameplayCueNotifies`, `Static` and `Actor`. They respond to different events and different types of `GameplayEffects` can trigger them. Override the corresponding event with your logic.

| `GameplayCue` Class        | Event             | `GameplayEffect` Type    | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| -------------------------- | ----------------- | ------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `GameplayCueNotify_Static` | `Execute`         | `Instant` or `Periodic`  | Static `GameplayCueNotifies` operate on the `ClassDefaultObject` (meaning no instances) and are perfect for one-off effects like hit impacts.                                                                                                                                                                                                                                                                                                                                                                        |
| `GameplayCueNotify_Actor`  | `Add` or `Remove` | `Duration` or `Infinite` | Actor `GameplayCueNotifies` spawn a new instance when `Added`. Because these are instanced, they can do actions over time until they are `Removed`. These are good for looping sounds and particle effects that will be removed when the backing `Duration` or `Infinite` `GameplayEffect` is removed or by manually calling remove. These also come with options to manage how many are allowed to be `Added` at the same so that multiple applications of the same effect only start the sounds or particles once. |

`GameplayCueNotifies` technically can respond to any of the events but this is typically how we use them.

**Note:** When using `GameplayCueNotify_Actor`, check `Auto Destroy on Remove` otherwise subsequent calls to `Add` that `GameplayCueTag` won't work.

The Sample Project does not include any `GameplayCues` soley because I don't have access to sounds and particles that I can distribute with the project.

<a name="concepts-gc-local"></a>
<a name="3.8.2"></a>
#### 3.8.2 Local Gameplay Cues
The exposed functions for firing `GameplayCues` from `GameplayAbilities` and the `ASC` are replicated by default. Each `GameplayCue` event is a multicast RPC. This can cause a lot of RPCs. GAS also enforces a maximum of two of the same `GameplayCue` RPCs per net update. We avoid this by using local `GameplayCues` where we can. Local `GameplayCues` only `Execute`, `Add`, or `Remove` on the invidiual client.

Scenarios where we can use local `GameplayCues`:
* Projectile impacts
* Melee collision impacts
* `GameplayCues` fired from animation montages

Local `GameplayCue` functions that you should add to your `ASC` subclass:
```c++
void UPAAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UPAAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
}

void UPAAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}
```

If a `GameplayCue` was `Added` locally, it should be `Removed` locally. If it was `Added` via replication, it should be `Removed` via replication.

<a name="concepts-gc-manager"></a>
<a name="3.8.3"></a>
#### 3.8.3 Gameplay Cue Manager
By default, the `GameplayCueManager` will scan the entire game directory for `GameplayCueNotifies` and load them into memory on play. We can change the path where the `GameplayCueManager` scans by setting it in the `DefaultGame.ini`.

```
[/Script/GameplayAbilities.AbilitySystemGlobals]
GameplayCueNotifyPaths="/Game/GASDocumentation/Characters"
```

We do want the `GameplayCueManager` to scan and find all of the `GameplayCueNotifies`; however, we don't want it to async load every single one on play. This will put every `GameplayCueNotify` and all of their referenced sounds and particles into memory regardless if they're even used in a level. In a large game like Paragon, this can be hundreds of megabytes of unneeded assets in memory. It is possible to only load the `GameplayCueNotifies` that we need by subclassing the `GameplayCueManager`, but I haven't put the time into figuring it out yet and no one else has offered a solution. My ideal scenario would be to load `GameplayCuesNotifies` based on common parent `GameplayCueTags` like `GameplayCue.Hero.HeroX.*`.

****This needs to be figured out at some point.**

<a name="concepts-gc-prevention"></a>
<a name="3.8.4"></a>
#### 3.8.4 Prevent Gameplay Cues from Firing
Sometimes we don't want `GameplayCues` to fire. For example if we block an attack, we may not want to play the hit impact attached to the damage `GameplayEffect` or play a custom one instead. We can do this inside of `GameplayEffectExecutionCalculations` (**LINK TO EXEC CALCS**) by calling `OutExecutionOutput.MarkGameplayCuesHandledManually()` and then manually sending our `GameplayCue` event to the `Target` or `Source's` `ASC`.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-asg"></a>
<a name="3.9"></a>
### 3.9 Ability System Globals
The `AbilitySystemGlobals` class holds global information about GAS. Most of the variables can be set from the `DefaultGame.ini`. Generally you won't have to interact with this class, but you should be aware of its existence. If you need to subclass things like the `GameplayCueManager` (**LINK TO GAMEPLAYCUEMANAGER**) or the `GameplayEffectContext` (**LINK TO GAMEPLAYEFFECTCONTEXT**), you have to do that through the `AbilitySystemGlobals`. The `AbilitySystemGlobals` is also responsible for other things like enforcing a maximum of 1000 concurrently running `AbilityTasks` (**LINK TO ABILITYTASKS**).

To subclass `AbilitySystemGlobals`, set the class name in the `DefaultGame.ini`:
```
[/Script/GameplayAbilities.AbilitySystemGlobals]
AbilitySystemGlobalsClassName="/Script/ParagonAssets.PAAbilitySystemGlobals"
```

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-p"></a>
<a name="3.10"></a>
### 3.10 Prediction


**[⬆ Back to Top](#table-of-contents)**

<a name="cae"></a>
<a name="4"></a>
## 4. Commonly Implemented Abilties and Effects

<a name="cae-stun"></a>
<a name="4.1"></a>
### 4.1 Stun



<a name="cae-sprint"></a>
<a name="4.2"></a>
### 4.2 Sprint



<a name="cae-ads"></a>
<a name="4.3"></a>
### 4.3 Aim Down Sights



<a name="cae-ls"></a>
<a name="4.4"></a>
### 4.4 Lifesteal



<a name="cae-random"></a>
<a name="4.5"></a>
### 4.5 Generating a Random Number on Client and Server



<a name="cae-crit"></a>
<a name="4.6"></a>
### 4.6 Critical Hits



<a name="cae-paused"></a>
<a name="4.7"></a>
### 4.7 Generate Target Data While Game is Paused




**[⬆ Back to Top](#table-of-contents)**

<a name="setup"></a>
<a name="5"></a>
## 5. Setting Up a Project Using GAS
Basic steps to set up a project using GAS:
1. Enable GameplayAbilitySystem plugin in the Editor
1. Edit `YourProjectName.Build.cs` to add `"GameplayAbilities", "GameplayTags", "GameplayTasks"` to your `PrivateDependencyModuleNames`
1. Refresh/Regenerate your Visual Studio project files

That's all that you have to do to enable GAS. From here, add an `ASC` and `AttributeSet` to your `Character` or `PlayerState` and start making `GameplayAbilities` and `GameplayEffects`!

**[⬆ Back to Top](#table-of-contents)**

<a name="debugging"></a>
<a name="6"></a>
## 6. Debugging GAS
Often when debugging GAS related issues, you want to know things like "What are the values of my attributes?", "What gameplay tags do I have?", "What gameplay effects do I currently have?", and "What abilities do I have granted, which ones are running, and which ones are blocked from activating?". GAS comes with two techniques for answering these questions at runtime.

<a name="debugging-sd"></a>
<a name="6.1"></a>
### 6.1 showdebug abilitysystem
Type `showdebug abilitysystem` in the in-game console. This feature is split into three "pages". All three pages will show the `GameplayTags` that you currently have. Type `AbilitySystem.Debug.NextCategory` into the console to cycle between the pages.

The first page shows the `CurrentValue` of all of your `Attributes`:
(**PIC OF FIRST PAGE**)

The second page shows all of the `Duration` and `Infinite` `GameplayEffects` on you, their number of stacks, what `GameplayTags` they give, and what `Modifiers` they give.
(**PIC OF SECOND PAGE**)

The third page shows all of the `GameplayAbilities` that have been granted to you, whether they are currently running, whether they are blocked from activating, and the status of currently running `AbilityTasks`.
(** PIC OF THE THIRD PAGE**)

While you can cycle between targets with `PageUp` and `PageDown`, the pages will only show data for the `ASC` on your locally controlled `Character`.

<a name="debugging-gd"></a>
<a name="6.2"></a>
### 6.2 Gameplay Debugger
GAS adds functionality to the Gameplay Debugger. Access the Gameplay Debugger with the Apostrophe (') key. Enable the Abilities category by pressing 3 on your numpad. The category may be different depending on what plugins you have. If your keyboard doesn't have a numpad like a laptop, then you can change the keybindings in the project settings.

Use the Gameplay Debugger when you want to see the `GameplayTags`, `GameplayEffects`, and `GameplayAbilities` on **other** `Characters`. Unfortunately it does not show the `CurrentValue` of the target's `Attributes`. It will target whatever `Character` is in the center of your screen. Look at a different `Character` and press Apostrophe (') again to switch to inspecting its `ASC`. The currently inspected `Character` has the largest red circle above it.

(**PIC OF GAMEPLAY DEBUGGER**)

**[⬆ Back to Top](#table-of-contents)**

<a name="acronyms"></a>
<a name="7"></a>
## 7. Common GAS Acronymns

| Name                               | Acronyms            |
|----------------------------------- | ------------------- |
| AbilitySystemComponent             | ASC                 |
| AbilityTask                        | AT                  |
| Action RPG Sample Project by Epic  | ARPG, ARPG Sample   |
| GameplayAbility                    | GA                  |
| GameplayAbilitySystem              | GAS                 |
| GameplayCue                        | GC                  |
| GameplayEffect                     | GE                  |
| GameplayEffectExecutionCalculation | ExecCalc, Execution |
| GameplayTag                        | Tag, GT             |
| ModiferMagnitudeCalculation        | ModMagCalc, MMC     |

**[⬆ Back to Top](#table-of-contents)**

<a name="resources"></a>
<a name="8"></a>
## 8. Other Resources
* [Official Documentation](https://docs.unrealengine.com/en-US/Gameplay/GameplayAbilitySystem/index.html)
* Source Code!
   * Especially `GameplayPrediction.h`
* [Action RPG Sample Project by Epic](https://www.unrealengine.com/marketplace/en-US/slug/action-rpg)
* [Dave Ratti from Epic's responses to community questions about GAS](https://epicgames.ent.box.com/s/m1egifkxv3he3u3xezb9hzbgroxyhx89)
* [Unreal Slackers Discord](https://unrealslackers.org/) has a text channel dedicated to GAS `#gameplay-abilities-plugin`
   * Check pinned messages
* [GitHub repository of resources by Dan 'Pan'](https://github.com/Pantong51/GASContent)

**[⬆ Back to Top](#table-of-contents)**
