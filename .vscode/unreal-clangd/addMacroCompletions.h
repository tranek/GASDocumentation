/*
* addMacroCompletions.h - Adds both macro and non-macro completions but will cause slower code completion file loading with each addition
*
* Only add files for 'macro completions' you use a lot or 'macro completions' that aren't included by default.
* Remember: All completions, including macro completions, work when you `#include "path/to/header.h"` the header directly in your source file...
* Feel free to experiment!
*
* *** NOTE: Edits/Editions won't work until you reload the file or press the bottom left 'UC' button twice (Do it twice to get back to same setting it was on).
*
* *** NOTE: This file will have Intellisense errors/warnings when opened directly. *****
*
*/

// **** These first two are examples of Modules that aren't enabled by default.
// **** If you do use them or any other module you can add them here.
// ***** module CQTest or (CQTestEnhancedInput?) *****
// #include "CQTest.h"  //  AFTER_EACH(ALL), BEFORE_EACH(ALL), TEST_CLASS_WITH_BASE_AND_FLAGS, and more
// ***** plugin/module GameplayAbilities *****
// #include "AttributeSet.h"  // GAMEPLAYATTRIBUTE_PROPERTY_GETTER, GAMEPLAYATTRIBUTE_VALUE_GETTER, GAMEPLAYATTRIBUTE_VALUE_INITTER, and others

// The example macros specified, in the comments, after these #includes aren't all the macros specified in the file
#include "Net/UnrealNetwork.h"  // DOREP* macros
#include "Online/CoreOnline.h"  // MAX_LOCAL_PLAYERS
#include "Stats/Stats.h"    // QUICK_*, SCOPE_*, DECLARE_*, CONDITIONAL_*, RETURN_*, GET_*, DEC_*, SET_*, STAT_* macros
#include "Delegates/DelegateCombinations.h"  // DECLARE_* (delegate macros)
#include "DrawDebugHelpers.h"  // ENABLE_DRAW_DEBUG
#include "GenericPlatform/GenericPlatformCompilerPreSetup.h" // PRAGMA_DISABLE_DEPRECATION_WARNINGS and others
#include "HAL/Platform.h"  // TEXT(), a lot of different ones
#include "Internationalization/Internationalization.h"  // NSLOCTEXT, LOCTEXT, (others)
#include "Logging/LogMacros.h"  // UE_LOG and others
#include "Math/UnrealMathUtility.h"  // Lots of Math Macros
#include "Misc/AssertionMacros.h"  // check(), ensure(), and a lot of others
#include "Misc/Build.h"  // UE_BUILD_SHIPPING and a lot of others
#include "Misc/CoreMiscDefines.h"  // UE_DEPRECATED, CA_* (code analysis), PURE_VIRTUAL, and more
#include "Misc/EnumClassFlags.h"  // ENUM_CLASS_FLAGS
#include "Modules/ModuleManager.h"  // IMPLEMENT_MODULE, IMPLEMENT_PRIMARY_GAME_MODULE
#include "ProfilingDebugging/CpuProfilerTrace.h"  // TRACE_* macros (cpu profiler)
#include "ProfilingDebugging/CsvProfiler.h"  // CSV_ macros (csv profiler)
#include "Trace/Trace.h"  // UE_TRACE_*
#include "UObject/ObjectMacros.h"  // UPROPERTY, UFUNCTION, USTRUCT, UMETA, UPARAM, UENUM, UDELEGATE, RIGVM_METHOD, a lot of others
#include "UObject/Script.h"  // RESULT_PARAM, RESULT_DECL, FUNC_*
#include "UObject/ScriptMacros.h"  // PARAM_* , P_GET_*, other P_* macros
#include "HAL/MemoryMisc.h"  // ENABLE_SHARED_MEMORY_TRACKER
#include "CollisionQueryParams.h"  // SCENE_QUERY_STAT, 
#include "Stats/StatsMisc.h"  // SCOPE_* and CONDITIONAL_SCOPE_* macros
#include "NativeGameplayTags.h" // UE_DEFINE_GAMEPLAY_TAG, UE_DEFINE_GAMEPLAY_TAG_STATIC, UE_DEFINE_GAMEPLAY_TAG_COMMENT, UE_DECLARE_GAMEPLAY_TAG_EXTERN, and others
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"   // UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES and others