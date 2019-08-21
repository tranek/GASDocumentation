# GASDocumentation
My understanding of Unreal Engine 4's GameplayAbilitySystem plugin (GAS) with a simple multiplayer sample project. This is not official documentation and neither this project nor myself are affiliated with Epic Games.

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
* Applying and removing `GameplayEffects` inside of and externally from abilities
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
* Abilities created in Blueprint
* Abilities created in C++
* Instanced per `Actor` abilities
* Non-instanced abilities (Jump)

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

It does not matter if abilities are created in C++ or Blueprint. A mixture of the two were used here for example of how to do them in each language.

Minions do not come with any predefined abilities. The Red Minions have more health regen while the Blue Minions have higher starting health.

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
The `AbilitySystemComponent` (`ASC`) is the heart of GAS. It's a `USceneComponent` (`UAbilitySystemComponent`) (**LINK TO OFFICIAL API**) that handles all interactions with the system. Any `AActor` that wishes to use `Abilities` (**LINK TO ABILITIES**), have `Attributes`(**LINK TO ATTRIBUTES**), or receive `GameplayEffects`(**LINK TO GAMEPLAYEFFECTS**) must have one `ASC` attached to them. These objects all live inside of and are managed and replicated by (with the exception of `Attributes` which are replicated by their `AttributeSet`(**LINK TO ATTRIBUTESET**)) the `ASC`. Developers are expected but not required to subclass this.

The `AActor` with the `ASC` attached to it is referred to as the `OwnerActor` of the `ASC`. The physical representation `AActor` of the `ASC` is called the `AvatarActor`. The `OwnerActor` and the `AvatarActor` can be the same `AActor` as in the case of a simple AI minion in a MOBA game. They can also be different `AActors` as in the case of a player controlled hero in a MOBA game where the `OwnerActor` is the `PlayerState` and the `AvatarActor` is the hero's `Character` class. Most `AActors` will have the `ASC` on themselves. If your `AActor` will respawn and need persistence of `Attributes` or `GameplayEffects` between spawns (like a hero in a MOBA), then the ideal location for the `ASC` is on the `PlayerState`.

Both, the `OwnerActor` and the `AvatarActor` if different `AActors`, should implement the `IAbilitySystemInterface`. This interface has one function that must be overriden, `UAbilitySystemComponent* GetAbilitySystemComponent() const`, which returns a pointer to its `ASC`. `ASCs` interact with each other internally to the system by looking for this interface function.

`ASCs` are typically constructed in the `OwnerActor's` constructor and marked replicated. **This must be done in C++**.

The `ASC` defines three different replication modes for replicating `GameplayEffects`, `GameplayTags`, and `GameplayCues` - `Full`, `Mixed`, and `Minimal`. `Attributes` are replicated by their `AttributeSet`.

| Replication Mode   | When to Use                             | Description                                                                                                                                   |
| ------------------ | --------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `Full`             | Single Player                           | Every `GameplayEffect` is replicated to every client.                                                                                         |
| `Mixed`            | Multiplayer, player controlled `Actors` | `GameplayEffects` are only replicated to the owning client. Only `GameplayTags` and `GameplayCues` are replicated to everyone. |
| `Minimal`          | Multiplayer, AI controlled `Actors`     | `GameplayEffects` are never replicated to anyone. Only `GameplayTags` and `GameplayCues` are replicated to everyone.           |

**Note:** `Mixed` replication mode expects the `OwnerActor's` `Owner` to be the `Controller`. `PlayerState's` `Owner` is the `Controller` by default but `Character's` is not. If using `Mixed` replication mode with the `OwnerActor` not the `PlayerState`, then you need to call `SetOwner()` on the `OwnerActor` with a valid `Controller`.

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

Often beginners to GAS will confuse `BaseValue` with a maximum value for an `Attribute` and try to treat it as such. This is an incorrect approach. Maximum values for `Attributes` that can change or are referenced in abilities or UI should be treated as separate `Attributes`. For hardcoded maximum and minimum values, there is a way to define a `DataTable` with `FAttributeMetaData` that can set maximum and minimum values, but Epic's comment above the struct calls it a "work in progress". See AttributeSet.h for more information. To prevent confusion, I recommend that maximum values that can be referenced in abilities or UI be made as separate `Attributes` and hardcoded maximum and minimum values that are only used for clamping `Attributes` be defined as hardcoded floats in the `AttributeSet`. Clamping of `Attributes` is discussed in (**LINK TO ATTRIBUTE SET PREATTRIBUTECHANGE/POSTGAMEPLAYEFFECT**).

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

See `GE_HeroAttributes` Blueprint in the Sample Project for how to make an instant `GameplayEffect` to initialize `Attributes`.

See `AttributeSet.h` for more ways to initialize `Attributes`.

**Note:** `FAttributeSetInitterDiscreteLevels` does not work with `FGameplayAttributeData`. It was created when `Attributes` were raw floats and will complain about `FGameplayAttributeData` not being `Plain Old Data` (`POD`). This is set to be fixed in 4.24 https://issues.unrealengine.com/issue/UE-76557.

<a name="concepts-as-preattributechange"></a>
<a name="3.3.5"></a>
#### 3.3.5 PreAttributeChange()
`PreAttributeChange()` is one of the two main functions in the `AttributeSet` to respond to changes to an `Attribute` with the other being `PostGameplayEffectExecute()` (**LINK BELOW TO POSTGAMEPLAYEFFECTEXECUTE**).




There is also a `PreAttributeBaseChange()` function that handles changes only to the `BaseValue` of an `Attribute`.

<a name="concepts-as-postgameplayeffectexecute"></a>
<a name="3.3.6"></a>
#### 3.3.6 PostGameplayEffectExecute()





<a name="concepts-ge"></a>
<a name="3.4"></a>
### 3.4 Gameplay Effects


<a name="concepts-gt"></a>
<a name="3.5"></a>
### 3.5 Gameplay Tags


<a name="concepts-ga"></a>
<a name="3.6"></a>
### 3.6 Gameplay Abilities


<a name="concepts-at"></a>
<a name="3.7"></a>
### 3.7 Ability Tasks


<a name="concepts-gc"></a>
<a name="3.8"></a>
### 3.8 Gameplay Cues


<a name="concepts-asg"></a>
<a name="3.9"></a>
### 3.9 Ability System Globals


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
