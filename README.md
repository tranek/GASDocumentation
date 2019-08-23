# GASDocumentation
My understanding of Unreal Engine 4's GameplayAbilitySystem plugin (GAS) with a simple multiplayer sample project. This is not official documentation and neither this project nor myself are affiliated with Epic Games. I make no guarantee for the accuracy of this information.

<a name="table-of-contents"></a>
## Table of Contents

> 1. [Intro to the GameplayAbilitySystem Plugin](#intro)
> 1. [Sample Project](#sp)
> 1. [Concepts](#concepts)
> 1. [Setting Up a Project Using GAS](#setup)
> 1. [Debugging](#debugging)
> 1. [Commonly Implemented Effects](#ce)
> 1. [Common GAS Acronymns](#acronyms)
> 1. [Other Resources](#resources)

1. Item 1
1. Item 2
1. Item 3
   1. Item 3a
   1. Item 3b
      1. Item 3?
         1. Item 3??
         
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

> 3.2 [Attributes](#concepts-a)

> 3.3 [Attribute Set](#concepts-as)

> 3.4 [Gameplay Effects](#concepts-ge)

> 3.5 [Gameplay Tags](#concepts-gt)

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

<a name="concepts-a"></a>
<a name="3.2"></a>
### 3.2 Attributes

<a name="concepts-a-definition"></a>
<a name="3.2.1"></a>
#### 3.2.1 Attribute Definition
`Attributes` are float values defined by the struct `FGameplayAttributeData` (**LINK TO OFFICIAL API**). These can represent anything from the amount of health a character has to the character's level to the number of charges that a potion has. If it is a gameplay-related numerical value belonging to an `Actor`, you should consider using an `Attribute` for it. `Attributes` should generally only be modified by `GameplayEffects` (**LINK TO GAMEPLAYEFFECT**) so that the ASC can predict (**LINK TO PREDICTION**) the changes.

`Attributes` are defined by and live in an `AttributeSet` (**LINK TO ATTRIBUTESET**). The `AttributeSet` is reponsible for replicating `Attributes` that are marked for replication. See the section on `AttributeSets` (**LINK TO ATTRIBUTESETS**) for how to define `Attributes`.

<a name="concepts-a-value"></a>
<a name="3.2.2"></a>
#### 3.2.2 BaseValue vs CurrentValue
An `Attribute` is composed of two values - a `BaseValue` and a `CurrentValue`. The `BaseValue` is the permanent value of the `Attribute` whereas the `CurrentValue` is the `BaseValue` plus temporary modifications from `GameplayEffects`. For example, your `Character` may have a movespeed `Attribute` with a `BaseValue` of 600 units/second. Since there are no `GameplayEffects` modifying the movespeed yet, the `CurrentValue` is also 600 u/s. If she gets a temporary 50 u/s movespeed buff, the `BaseValue` stays the same at 600 u/s while the `CurrentValue` is now 600 + 50 for a total of 650 u/s. When the movespeed buff expires, the `CurrentValue` reverts back to the `BaseValue` of 600 u/s.

Often beginners to GAS will confuse `BaseValue` with a maximum value for an `Attribute` and try to treat it as such. This is an incorrect approach. Maximum values for `Attributes` that can change or are referenced in abilities or UI should be treated as separate `Attributes`. For hardcoded maximum and minimum values, there is a way to define a `DataTable` with `FAttributeMetaData` that can set maximum and minimum values, but Epic's comment above the struct calls it a "work in progress". See `AttributeSet.h` for more information. To prevent confusion, I recommend that maximum values that can be referenced in abilities or UI be made as separate `Attributes` and hardcoded maximum and minimum values that are only used for clamping `Attributes` be defined as hardcoded floats in the `AttributeSet`. Clamping of `Attributes` is discussed in (**LINK TO ATTRIBUTE SET PREATTRIBUTECHANGE/POSTGAMEPLAYEFFECT**).

Permanent changes to the `BaseValue` come from instant `GameplayEffects` whereas duration/infinite `GameplayEffects` change the `CurrentValue`. Periodic `GameplayEffects` are treated like instant `GameplayEffects` and change the `BaseValue`.

<a name="concepts-a-meta"></a>
<a name="3.2.3"></a>
#### 3.2.3 Meta Attributes
Some `Attributes` are treated as placeholders for temporary values that are intended to interact with `Attributes`. These are called `Meta Attributes`. For example, we commonly define damage as a `Meta Attribute`. Instead of a `GameplayEffect` directly changing our health `Attribute`, we use a `Meta Attribute` called damage as a placeholder. This way the damage value can be modified with buffs and debuffs in an `GameplayEffectExecutionCalculation` (**LINK TO EXECUTIONCALCULATIONS**) and can be further manipulated in the `AttributeSet` for example subtracting the damage from a current shield `Attribute` before finally subtracting the remainder from the health `Attribute`. The damage `Meta Attribute` has no persistence between `GameplayEffects` and is overriden by every one.

`Meta Attributes` are not typically replicated.

<a name="concepts-a-changes"></a>
<a name="3.2.4"></a>
#### 3.2.4 Responding to Attribute Changes
To listen for when an attribute changes to update the UI or other gameplay, use `UAbilitySystemComponent::GetGameplayAttributeValueChangeDelegate(FGameplayAttribute Attribute)`. This function returns a delegate that you can bind to that will be automatically called whenever an `Attribute` changes. The delegate provides a `FOnAttributeChangeData` parameter with the `NewValue`, `OldValue`, and `FGameplayEffectModCallbackData`. **Note:** The `FGameplayEffectModCallbackData` will only be set on the server.

The Sample Project binds to the attribute value changed delegates on the `GDPlayerState` to update the HUD and to respond to player death when health reaches zero. A custom Blueprint node that wraps this into an `ASyncTask` is included in the Sample Project. It is used in the `UI_HUD` UMG Widget to update the health, mana, and stamina values. See `AsyncTaskAttributeChanged.h/cpp`.

(**PICTURE OF BP NODE FOR ATTRIBUTE CHANGE**)

<a name="concepts-as"></a>
<a name="3.3"></a>
### 3.3 Attribute Set

<a name="concepts-as-definition"></a>
<a name="3.3.1"></a>
#### 3.3.1 Attribute Set Definition
The `AttributeSet` defines, holds, and manages changes to `Attributes`. Developers should subclass from `UAttributeSet` (**LINK TO API DOCUMENATION**). Creating an `AttributeSet` in an `OwnerActor's` constructor automatically registers it with its `ASC`. **This must be done in C++**.

<a name="concepts-as-design"></a>
<a name="3.3.2"></a>
#### 3.3.2 Attribute Set Design
An `ASC` may have one or many `AttributeSets`. AttributeSets have negligable memory overhead so how many `AttributeSets` to use is an organizational decision left up to the developer.

It is acceptable to have one large monolithic `AttributeSet` shared by every `Actor` in your game and only use attributes if needed while ignoring unused attributes.

Alternatively, you may choose to have more than one `AttributeSet` representing groupings of `Attributes` that you selectively add to your `Actors` as needed. For example, you could have an `AttributeSet` for health related `Attributes`, an `AttributeSet` for mana related `Attributes`, and so on. In a MOBA game, heroes might need mana but minions might not. Therefore the heroes would get the mana `AttributeSet` and minions would not.

Additionally, `AttributeSets` can be subclassed as another means of selectively choosing which `Attributes` an `Actor` has.

<a name="concepts-as-attributes"></a>
<a name="3.3.3"></a>
#### 3.3.3 Defining Attributes
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
<a name="3.3.4"></a>
#### 3.3.4 Initializing Attributes
There are multiple ways to initialize `Attributes` (set their `BaseValue` and consequently their `CurrentValue` to some initial value). Epic recommends using an instant `GameplayEffect`. This is the method used in the Sample Project too.

See `GE_HeroAttributes` Blueprint in the Sample Project for how to make an instant `GameplayEffect` to initialize `Attributes`. Application of this `GameplayEffect` happens in C++.

See `AttributeSet.h` for more ways to initialize `Attributes`.

**Note:** `FAttributeSetInitterDiscreteLevels` does not work with `FGameplayAttributeData`. It was created when `Attributes` were raw floats and will complain about `FGameplayAttributeData` not being `Plain Old Data` (`POD`). This is set to be fixed in 4.24 https://issues.unrealengine.com/issue/UE-76557.

<a name="concepts-as-preattributechange"></a>
<a name="3.3.5"></a>
#### 3.3.5 PreAttributeChange()
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
<a name="3.3.6"></a>
#### 3.3.6 PostGameplayEffectExecute()
`PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)` only triggers after changes to the `BaseValue` of an `Attribute` from an instant `GameplayEffect` (**LINK TO GAMEPLAYEFFECT**). This is a valid place to do more `Attribute` manipulation when they change from a `GameplayEffect`.

For example, in the Sample Project we subtract the final damage `Meta Attribute` from the health `Attribute` here. If there was a shield `Attribute`, we would subtract the damage from it first before subtracting the remainder from health. The Sample Project also uses this location to apply hit react animations, show floating Damage Numbers, and assign experience and gold bounties to the killer. By design, the damage `Meta Attribute` will always come through an instant `GameplayEffect` and never the `Attribute` setter.

Other `Attributes` that will only have their `BaseValue` changed from instant `GameplayEffects` like mana and stamina can also be clamped to their maximum value counterpart `Attributes` here.

**Note:** When `PostGameplayEffectExecute()` is called, changes to the `Attribute` have already happened, but they have not replicated back to clients yet so clamping values here will not cause two network updates to clients. Clients will only receive the update after clamping.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-ge"></a>
<a name="3.4"></a>
### 3.4 Gameplay Effects

<a name="concepts-ge-definition"></a>
<a name="3.4.1"></a>
#### 3.4.1 Gameplay Effect definition
`GameplayEffects` (`GEs`) (**LINK TO API**) are the vessels through which abilities change `Attributes` (**LINK TO ATTRIBUTES**) and `GameplayTags` (**LINK TO GAMEPLAYTAGS**) on themselves and others. They can cause immediate `Attribute` changes like damage or healing or apply long term status buff/debuffs like a movespeed boost or stunning. The `UGameplayEffect` class is a meant to be a **data-only** class that defines a single gameplay effect. No additional logic should be added to `GameplayEffects`. Typically designers will create many Blueprint child classes of `UGameplayEffect`.

`GameplayEffects` change `Attributes` through `Modifiers` (**LINK BELOW TO MODIFIERS**) and `Executions` (`GameplayEffectExecutionCalculation`) (**LINK BELOW TO EXECUTIONCALCS**).

`GameplayEffects` have three types of duration: `Instant`, `Duration`, and `Infinite`.

Additionally, `GameplayEffects` can add/execute `GameplayCues` (**LINK TO GAMEPLAYCUES**). An `Instant` `GameplayEffect` will call `Execute` on the `GameplayCue` `GameplayTags` whereas a `Duration` or `Infinite` `GameplayEffect` will call `Add` and `Remove` on the `GameplayCue` `GameplayTags`.

| Duration Type | GameplayCue Event | When to use                                                                                                                                                                                                                                |
| ------------- | ----------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `Instant`     | Execute           | For immediate permanent changes to `Attribute's` `BaseValue`. `GameplayTags` will not be applied, not even for a frame.                                                                                                                    |
| `Duration`    | Add & Remove      | For temporary changes to `Attribute's` `CurrentValue` and to apply `GameplayTags` that will be removed when the `GameplayEffect` expires or is manually removed. The duration is specified in the `UGameplayEffect` class/Blueprint.       |
| `Infinite`    | Add & Remove      | For temporary changes to `Attribute's` `CurrentValue` and to apply `GameplayTags` that will be removed when the `GameplayEffect` is removed. These will never expire on their own and must be manually removed by an ability or the `ASC`. |

`Duration` and `Infinite` `GameplayEffects` have the option of applying `Periodic Effects` that apply its `Modifiers` and `Executions` every `X` seconds as defined by its `Period`. `Periodic Effects` are treated as `Instant` `GameplayEffects` when it comes to changing the `Attribute's` `BaseValue` and `Executing` `GameplayCues`.

`Duration` and `Infinite` `GameplayEffects` can be temporarily turned off and on after application if their `Ongoing Tag Requirements` are not met/met (**LINK TO GE TAGS**). Turning off a `GameplayEffect` removes the effects of its `Modifiers` and applied `GameplayTags` but does not remove the `GameplayEffect`. Turning the `GameplayEffect` back on reapplies its `Modifiers` and `GameplayTags`.

`GameplayEffects` are not typically instantiated. When an ability or `ASC` wants to apply a `GameplayEffect`, it creates a `GameplayEffectSpec` (**LINK TO GAMEPLAYEFFECTSPEC**) from the `GameplayEffect's` `ClassDefaultObject`. Successfully applied `GameplayEffectSpecs` are then added to a new struct called `FActiveGameplayEffect` which is what the `ASC` keeps track of in a special container struct called `ActiveGameplayEffects`.

<a name="concepts-ge-mods"></a>
<a name="3.4.2"></a>
#### 3.4.2 Gameplay Effect Modifiers
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
<a name="3.4.3"></a>
#### 3.4.3 Stacking
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
<a name="3.4.4"></a>
#### 3.4.4 Granted Abilities
`GameplayEffects` can grant new abilities (**LINK TO GAMEPLAYABILITIES**) to `ASCs`. Only `Duration` and `Infinite` `GameplayEffects` can grant abilities.

A common usecase for this is when you want to force another player to do something like moving them from a knockback or pull. You would apply a `GameplayEffect` to them that grants them an automatically activating ability (see passive abilities for how to automatically activate an ability when it is granted) (**LINK TO PASSIVE ABILITIES**) that does the desired action to them.

Designers can choose which abilities a `GameplayEffect` grants, what level to grant them at, what input to bind them at (**LINK TO INPUT BINDING IN ABILITIES**) and the removal policy for the granted ability.

| Removal Policy             | Description                                                                                                                                                                     |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Cancel Ability Immediately | The granted ability is canceled and removed immediately when the `GameplayEffect` that granted it is removed from the Target.                                                   |
| Remove Ability on End      | The granted ability is allowed to finish and then is removed from the Target.                                                                                                   |
| Do Nothing                 | The granted ability is not affected by the removal of the granting `GameplayEffect` from the Target. The Target has the ability permanently until it is manually removed later. |

<a name="concepts-ge-tags"></a>
<a name="3.4.5"></a>
#### 3.4.5 Gameplay Effect Tags
`GameplayEffects` carry multiple `GameplayTagContainers` (**LINK TO GAMPLAY TAGS**). Designers will edit the `Added` and `Removed` `GameplayTagContainers` for each category and the result will show up in the `Combined` `GameplayTagContainer` on compilation. `Added` tags are new tags that this `GameplayEffect` adds that its parents did not previously have. `Removed` tags are tags that parent classes have but this subclass does not have.

| Category                          | Description                                                                                                                                                                                                                                                                                                                                                                        |
| --------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Gameplay Effect Asset Tags        | Tags that the `GameplayEffect` has. They do not do any function on their own and serve only the purpose of describing the `GameplayEffect`.                                                                                                                                                                                                                                        |
| Granted Tags                      | Tags that live on the `GameplayEffect` but are also given to the `ASC` that the `GameplayEffect` is applied to. They are removed from the `ASC` when the `GameplayEffect` is removed. This only works for `Duration` and `Infinite` `GameplayEffects`.                                                                                                                             |
| Ongoing Tag Requirements          | Once applied, these tags determine whether the `GameplayEffect` is on or off. A `GameplayEffect` can be off and still be applied. If a `GameplayEffect` is off due to failing the Ongoing Tag Requirements, but the requirements are then met, the `GameplayEffect` will turn on again and reapply its modifiers. This only works for `Duration` and `Infinite` `GameplayEffects`. |
| Application Tag Requirements      | Tags on the Target that determine if a `GameplayEffect` can be applied to the Target. If these requirements are not met, the `GameplayEffect` is not applied.                                                                                                                                                                                                                      |
| Remove Gameplay Effects with Tags | `GameplayEffects` on the Target that have any of these tags in their `Asset Tags` or `Granted Tags` will be removed from the Target when this `GameplayEffect` is successfully applied.                                                                                                                                                                                            |

<a name="concepts-ge-immunity"></a>
<a name="3.4.6"></a>
#### 3.4.6 Immunity
`GameplayEffects` can grant immunity, effectively blocking the application of other `GameplayEffects`, based on `Gameplaytags` (**LINK TO GAMEPLAYTAGS**). While immunity can be effectively achieved through other means like `Application Tag Requirements`, using this system provides a delegate for when `GameplayEffects` are blocked due to immunity `UAbilitySystemComponent::OnImmunityBlockGameplayEffectDelegate`.

`GrantedApplicationImmunityTags` checks if the Source `ASC` (including tags from the Source ability's `AbilityTags` if there was one) has any of the specified tags. This is a way to provide immunity from all `GameplayEffects` from certain characters or sources based on their tags.

`Granted Application Immunity Query` checks the incoming `GameplayEffectSpec` if it matches any of the queries to block or allow its application.

The queries have helpful hover tooltips in the `GameplayEffect` Blueprint.

<a name="concepts-ge-spec"></a>
<a name="3.4.7"></a>
#### 3.4.7 Gameplay Effect Spec
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
<a name="3.4.8"></a>
#### 3.4.8 Gameplay Effect Context
The `GameplayEffectContext` (**LINK TO API**) structure holds information about a `GameplayEffectSpec's` instigator and target location data. This is also a good structure to subclass to pass arbitrary data around between places like `ModifierMagnitudeCalculations` / `GameplayEffectExecutionCalculations` and the `AttributeSet`.

To subclass the `GameplayEffectContext`:

1. Subclass `FGameplayEffectContext`
1. Override `FGameplayEffectContext::GetScriptStruct()`
1. Override `FGameplayEffectContext::Duplicate()`
1. Override `FGameplayEffectContext::NetSerialize()` if your new data needs to be replicated
1. If you overrode `FGameplayEffectContext::NetSerialize()`, be sure to make the `TStructOpsTypeTraits` like the parent struct `FGameplayEffectContext` has
1. Override `AllocGameplayEffectContext()` in your `AbilitySystemGlobals` class (**LINK TO ABILITYSYSTEMGLOBALS**) to return a new object of your subclass

<a name="concepts-ge-mmc"></a>
<a name="3.4.9"></a>
#### 3.4.9 Modifier Magnitude Calculation
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
<a name="3.4.10"></a>
#### 3.4.10 Gameplay Effect Execution Calculation
`GameplayEffectExecutionCalculations` (`ExecutionCalculations` or `ExecCalcs`) (**LINK TO API**) are the most powerful way for `GameplayEffects` to make changes to an `ASC`. Like `ModifierMagnitudeCalculations` (**LINK ABOVE TO MMCs**), these can capture `Attributes` and optionally snapshot them. Unlike `MMCs`, these can change more than one `Attribute` and essentially do anything else that the programmer wants. The downside to this power and flexiblity is that they can not be predicted (**LINK TO PREDICTION**) and they must be implemented in C++.

Snapshotting captures the `Attribute` when the `GameplayEffectSpec` is created whereas not snapshotting captures the `Attribute` when the `GameplayEffectSpec` is applied. Capturing `Attributes` recalculates their `CurrentValue` from existing mods on the `ASC`. This recalculation will **not** run `PreAttributeChange()` (**LINK TO PREATTRIBUTECHANGE**) in the `AbilitySet` so any clamping must be done here again.

To set up `Attribute` capture, we follow a pattern set by Epic's ActionRPG Sample Project by defining a struct holding and defining how we capture the `Attributes` and creating one copy of it in the struct's constructor. You will have a struct like this for every `ExecCalc`. **Note:** Each struct needs a unique name as they share the same namespace. Using the same name for the structs will cause incorrect behavior in capturing your `Attributes` (mostly capturing the values of the wrong `Attributes`).

For `Local Predicted`, `Server Only`, and `Server Initiated` (**LINK ABILITIES**) abilities, the `ExecCalc` only calls on the Server.

Calculating damage received based on a complex formula reading from many attributes on the `Source` and the `Target` is the most common example of an `ExecCalc`. The included Sample Project has a simple `ExecCalc` for calculating damage that reads the value of damage from the `GameplayEffectSpec's` `SetByCaller` (**LINK TOSETBYCALLER **) and then mitigates that value based on the armor `Attribute` captured from the `Target`. See `GDDamageExecCalculation.cpp/.h`.

<a name="concepts-ge-car"></a>
<a name="3.4.11"></a>
#### 3.4.11 Custom Application Requirement
`CustomApplicationRequirement` (`CAR`) (**LINK TO API**) classes give the designers advanced control over whether a `GameplayEffect` can be applied versus the simple `GameplayTag` checks on the `GameplayEffect`. These can be implemented in Blueprint by overriding `CanApplyGameplayEffect()` and in C++ by overriding `CanApplyGameplayEffect_Implementation()`.

Examples of when to use `CARs`:
* `Target` needs to have a certain amount of an `Attribute`
* `Target` needs to have a certain number of stacks of a `GameplayEffect`

`CARs` can also do more advanced things like checking if an instance of this `GameplayEffect` is already on the `Target` and changing the duration (**LINK TO CHANGING ACTIVE GAMEPLAY EFFECT DURATION**) of the existing instance instead of applying a new instance (return false for `CanApplyGameplayEffect()`).

<a name="concepts-ge-cost"></a>
<a name="3.4.12"></a>
#### 3.4.12 Cost Gameplay Effect
`GameplayAbilities` (**LINK TO GAs**) have an optional `GameplayEffect` specifically designed to use as the cost of the ability. Costs are how much of an `Attribute` an `ASC` needs to have to be able to activate the `GameplayAbility`. If a `GA` cannot afford the `Cost GE`, then they will not be able to activate. This `Cost GE` should be an `Instant` `GameplayEffect` with one or more `Modifiers` that subtract from `Attributes`. By default, `Cost GEs` are meant to be predicted and it is recommended to maintain that capability meaning do not use `ExecutionCalculations`. `MMCs` are perfectly acceptable and encouraged for complex cost calculations.

When starting out, you will most likely have one unique `Cost GE` per `GA` that has a cost. A more advanced technique is to reuse one `Cost GE` for multiple `GAs` and just modify the `GameplayEffectSpec` created from the `Cost GE` with the `GA`-specific data (the cost value is defined on the `GA`). This only works for `Instanced` abilities.

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
<a name="3.4.13"></a>
#### 3.4.13 Cooldown Gameplay Effect
`GameplayAbilities` (**LINK TO GAs**) have an optional `GameplayEffect specifically design to use as the cooldown of the abilitiy. Cooldowns determine how long after activation the ability can be activated again. If a `GA` is still on cooldown, it cannot activate. This `Cooldown GE` should be a `Duration` `GameplayEffect` with no `Modifiers` and a unique `GameplayTag` per `GameplayAbility` or per ability slot (if your game has interchangeable abilities assigned to slots that share a cooldown) in the `GameplayEffect's` `GrantedTags` ("`Cooldown Tag`"). The `GA` actually checks for the presence for the `Cooldown Tag` instead of the presence of the `Cooldown GE`. By default, `Cooldown GEs` are meant to be predicted and it is recommended to maintain that capability meaning do not use `ExecutionCalculations`. `MMCs` are perfectly acceptable and encouraged for complex cooldown calculations.

When starting out, you will most likely have one unique `Cooldown GE` per `GA` that has a cooldown. A more advanced technique is to reuse one `Cooldown GE` for multiple `GAs` and just modify the `GameplayEffectSpec` created from the `Cooldown GE` with `GA`-specific data (the cooldown duration and the `Cooldown Tag` are defined on the `GA`). This only works for `Instanced` abilities.

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
<a name="3.4.14"></a>
#### 3.4.14 Changing Active Gameplay Effect Duration
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
<a name="3.4.15"></a>
#### 3.4.15 Creating Dynamic Gameplay Effects at Runtime
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
<a name="3.4.16"></a>
#### 3.4.16 Gameplay Effect Containers
Epic's Action RPG Sample Project (**LINK TO ARPG SAMPLE**) implements a structure called `FGameplayEffectContainer`. These are not in vanilla GAS but are extremely handy for containing `GameplayEffects` and `TargetData`. It automates a some of the effort like creating `GameplayEffectSpecs` and setting default values in its `GameplayEffectContext`. Making a `GameplayEffectContainer` in a `GameplayAbility` and passing it to spawned projectiles is very easy and straightforward. I opted not to implement the `GameplayEffectContainers` in the included Sample Project to show how you would work without them in vanilla GAS, but I highly recommend looking into them and considering adding them to your project.

**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-gt"></a>
<a name="3.5"></a>
### 3.5 Gameplay Tags


**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-ga"></a>
<a name="3.6"></a>
### 3.6 Gameplay Abilities


**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-at"></a>
<a name="3.7"></a>
### 3.7 Ability Tasks


**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-gc"></a>
<a name="3.8"></a>
### 3.8 Gameplay Cues


**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-asg"></a>
<a name="3.9"></a>
### 3.9 Ability System Globals


**[⬆ Back to Top](#table-of-contents)**

<a name="concepts-p"></a>
<a name="3.10"></a>
### 3.10 Prediction


**[⬆ Back to Top](#table-of-contents)**

<a name="setup"></a>
<a name="4"></a>
## 4. Setting Up a Project Using GAS

**[⬆ Back to Top](#table-of-contents)**

<a name="debugging"></a>
<a name="5"></a>
## 5. Debugging GAS

**[⬆ Back to Top](#table-of-contents)**

<a name="ce"></a>
<a name="6"></a>
## 6. Commonly Implemented Effects

**[⬆ Back to Top](#table-of-contents)**

<a name="acronyms"></a>
<a name="7"></a>
## 7. Common GAS Acronymns

**[⬆ Back to Top](#table-of-contents)**

<a name="resources"></a>
<a name="8"></a>
## 8. Other Resources

**[⬆ Back to Top](#table-of-contents)**
