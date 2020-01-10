#include "skse64_common/skse_version.h"  // RUNTIME_VERSION

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR

#include "SKSE/API.h" //GENERAL SKSE API
#include "RE/CommandTable.h" //OBSCRIPT HOOKS
#include "RE/Skyrim.h" //SKYRIM PAPYRUS INTERFACE

#include "common/ITypes.h" //TYPES

#include <sstream>
 
__declspec(dllimport) double g_silent_voice_duration_seconds;
__declspec(dllimport) int g_is_obscript_say_say_to;


//NativeFunction* stopFunction = NULL;

namespace RE {
	namespace ObScriptHooks {

		SCRIPT_FUNCTION* isAnimPlayingFunction = NULL;
		SCRIPT_FUNCTION::ScriptData isAnimPlayingFunctionData =
		{
			0x1128,
			0x0001,
			0x0000
		};

		float ObScriptSay(TESQuest*, TESObjectREFR* thisActor, TESTopic* TopicID)
		{
			double result = 0.0;

			auto scriptFactory = IFormFactory::GetConcreteFormFactoryByType<Script>();
			if (!scriptFactory) {
				return 0.0;
			}

			Script* script = scriptFactory->Create();
			if (!script) {
				return 0.0;
			}

			std::stringstream stream;
			stream << "Say 0x"
				<< std::setfill('0') << std::setw(8)
				<< std::hex << TopicID->formID;

			_DMESSAGE(stream.str().c_str());

			script->SetCommand(stream.str());

			g_is_obscript_say_say_to = true;

			script->Invoke(thisActor, RE::Script::InvokeType::kDefaultCompiler);

			g_is_obscript_say_say_to = false;

			result = g_silent_voice_duration_seconds;

			delete script;
			return result;
		}

		float ObScriptSayTo(TESQuest*, TESObjectREFR* thisActor, Actor* anotherActor, TESTopic* TopicID)
		{
			double result = 0.0;

			auto scriptFactory = IFormFactory::GetConcreteFormFactoryByType<Script>();
			if (!scriptFactory) {
				return 0.0;
			}

			Script* script = scriptFactory->Create();
			if (!script) {
				return 0.0;
			}

			std::stringstream stream;
			stream << "SayTo 0x"
				<< std::setfill('0') << std::setw(8)
				<< std::hex << anotherActor->formID << " 0x" <<
				TopicID->formID;

			_DMESSAGE(stream.str().c_str());

			script->SetCommand(stream.str());

			g_is_obscript_say_say_to = true;

			script->Invoke(thisActor, Script::InvokeType::kDefaultCompiler);

			g_is_obscript_say_say_to = false;

			result = g_silent_voice_duration_seconds;

			delete script;
			return result;
		}

		bool prepareForReinitializing(TESQuest* a_form)
		{
			a_form->alreadyRun = false;
			a_form->ClearData();
			return true;
		}

		UInt32 getAmountSoldStolen(StaticFunctionTag*) {
			return PlayerCharacter::GetSingleton()->amountStolenSold;
		}

		void modAmountSoldStolen(StaticFunctionTag*, unsigned long amount) {
			PlayerCharacter::GetSingleton()->amountStolenSold = amount;
		}

		UInt32 isPCAMurderer(StaticFunctionTag*) {
			return PlayerCharacter::GetSingleton()->murder;
		}

		bool isAnimPlaying(TESObjectREFR* animatedRefr) {
			if (isAnimPlayingFunction)
			{
				double result = 0.0;
				UInt32 opcodeOffset = 0x4;
				
				isAnimPlayingFunction->executeFunction(
					isAnimPlayingFunction->params,
					&isAnimPlayingFunctionData,
					animatedRefr,
					NULL,
					NULL,
					NULL,
					result,
					opcodeOffset
				);
				return (result != 0.0);
			}
			return false;
		}

		UInt32 getDestroyed(TESObjectREFR* reference) {
			return ((reference->loadedData->flags & 0x800000) == 0x800000) ? 1 : 0;
		}

		bool RegisterFuncs(BSScript::Internal::VirtualMachine* a_vm) {

			_MESSAGE("Initializing ObScript hooks...");

			a_vm->RegisterFunction("PrepareForReinitializing", "Quest", prepareForReinitializing);
			
			a_vm->RegisterFunction("GetDestroyed", "ObjectReference", getDestroyed);
			a_vm->RegisterFunction("ObScriptSayTo", "TES4TimerHelper", ObScriptSayTo);
			a_vm->RegisterFunction("ObScriptSay", "TES4TimerHelper", ObScriptSay);
			a_vm->RegisterFunction("GetAmountSoldStolen", "Game", getAmountSoldStolen);
			a_vm->RegisterFunction("ModAmountSoldStolen", "Game", modAmountSoldStolen);
			a_vm->RegisterFunction("IsPCAMurderer", "Game", isPCAMurderer);

			_MESSAGE("Looking for IsAnimPlaying execute handler");
			isAnimPlayingFunction = SCRIPT_FUNCTION::LocateScriptCommand("IsAnimPlaying");
			if (NULL == isAnimPlayingFunction)
			{
				_ERROR("Unable to find isAnimPlayingFunction!");
			}
			a_vm->RegisterFunction("IsAnimPlaying", "ObjectReference", isAnimPlaying);

			_MESSAGE("Initializing ObScript hooks done");

			return true;
		}
	}
}

extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\Skyblivion.log");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::UseLogStamp(true);

		_MESSAGE("Skyblivion v%s", SKYB_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "Skyblivion";
		a_info->version = SKYB_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		switch (a_skse->RuntimeVersion()) {
		case RUNTIME_VERSION_1_5_97:
			break;
		default:
			_FATALERROR("Unsupported runtime version %08X!\n", a_skse->RuntimeVersion());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("Skyblivion loaded");

		if (!SKSE::Init(a_skse)) {
			return false;
		}

		auto papyrus = SKSE::GetPapyrusInterface();
		if (!papyrus->Register(RE::ObScriptHooks::RegisterFuncs)) {
			_FATALERROR("Failed to register papyrus callback!\n");
			return false;
		}

		return true;
	}
};
