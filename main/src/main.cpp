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

		SCRIPT_FUNCTION* getDestroyedFunction = NULL;
		SCRIPT_FUNCTION::ScriptData getDestroyedFunctionData =
		{
			0x10CB,
			0x0001,
			0x0000
		};

		SCRIPT_FUNCTION* sayFunction = NULL;
		SCRIPT_FUNCTION* sayToFunction = NULL;

		BYTE dummySCRISayToAlloc[sizeof(Script)] = { 0 };
		BYTE dummySCRISayAlloc[sizeof(Script)] = { 0 };
		Script* dummySayToScript = (Script*)&dummySCRISayToAlloc;
		Script* dummySayScript = (Script*)&dummySCRISayAlloc;

		BYTE sayToScriptData[12] = { 0x34, 0x10, 0x8, 0x0, 0x2, 0x0, 0x72, 0x01, 0x0, 0x72, 0x2, 0x0 };
		BYTE sayScriptData[9] = { 0x33, 0x10, 0x5, 0x0, 0x1, 0x0, 0x72, 0x1, 0x0 };

		void initDummySayToScript() {
			dummySayToScript->formFlags = 0x000400a;
			dummySayToScript->formID = 0xff000a17;
			dummySayToScript->formType = (FormType)0x13;
			//dummySayToScript->pad13 = 0x0f;
			dummySayToScript->header.refObjectCount = 0x00000002;
			dummySayToScript->header.dataSize = 0x0000000c;
			dummySayToScript->header.variableCount = 0x00000000;
			dummySayToScript->header.isQuestScript = 1;
			//dummySayToScript->headertype = 0x00010000;
			dummySayToScript->text = "SayTo";
			dummySayToScript->data = (SCRIPT_FUNCTION::ScriptData*)sayToScriptData;
		}

		void initDummySayScript() {
			dummySayScript->formFlags = 0x000400a;
			dummySayScript->formID = 0xff000e05;
			dummySayScript->formType = (FormType)0x13;
			//dummySayScript->pad13 = 0x00;
			dummySayScript->header.refObjectCount = 0x00000001;
			dummySayScript->header.dataSize = 0x00000009;
			dummySayScript->header.variableCount = 0x00000000;
			dummySayScript->header.isQuestScript = 1;
			dummySayScript->text = "Say";
			dummySayScript->data = (SCRIPT_FUNCTION::ScriptData*)sayScriptData;
		}


		Float32 ObScriptSay(TESObjectREFR* thisActor, TESTopic* TopicID, bool value)
		{
			double result = 0.0;

			if (NULL != sayFunction)
			{


				//auto scriptFactory = IFormFactory::GetConcreteFormFactoryByType<Script>();
				//if (!scriptFactory) {
				//	return 0.0;
				//}

				//Script* script = scriptFactory->Create();
				//if (!script) {
				//	return 0.0;
				//}

				UInt32 opcodeOffset = 0x4;

				if (thisActor == NULL || TopicID == NULL)
					return 0.5;

				SCRIPT_REFERENCED_OBJECT arg2;
				//memset(&arg2.form_name, 0, sizeof(BSString));
				arg2.editorID = "";
				arg2.form = TopicID;

				//BSSimpleList<SCRIPT_REFERENCED_OBJECT*> arglist2(&arg2, NULL);
				//BSSimpleList<SCRIPT_REFERENCED_OBJECT*> arglist1(&arg1, &arglist2);
				//RefListEntry arglist2;

				//arglist1.var = &arg1;
				//arglist2.var = &arg2;
				//arglist1.next = &arglist2;
				//arglist2.next = NULL;


				BSSimpleList<SCRIPT_REFERENCED_OBJECT*> reflist;
				reflist.push_front(&arg2);

				initDummySayScript();
				dummySayScript->refObjects = reflist;

				g_is_obscript_say_say_to = true;
				g_silent_voice_duration_seconds = 5.0;

				sayFunction->executeFunction(
					sayFunction->params,
					(SCRIPT_FUNCTION::ScriptData*)sayScriptData,
					thisActor,
					NULL,
					dummySayScript,
					NULL,
					result,
					opcodeOffset
				);

				g_is_obscript_say_say_to = false;
				result = g_silent_voice_duration_seconds;
				return result + 0.5;

				//std::stringstream stream;
				//stream << "Say "
				//	<< std::setfill('0') << std::setw(8)
				//	<< std::hex << TopicID->formID;

				//_DMESSAGE(stream.str().c_str());

				//script->SetCommand(stream.str());

				//g_is_obscript_say_say_to = true;

				//script->Invoke(thisActor, RE::Script::InvokeType::kSysWindowCompileAndRun);

				//g_is_obscript_say_say_to = false;

				//result = g_silent_voice_duration_seconds;

				//delete script;
			}
			return result;
		}

		Float32 ObScriptSayTo(TESObjectREFR* thisActor, Actor* anotherActor, TESTopic* TopicID, bool value)
		{
			double result = 0.0;

			if (NULL != sayToFunction)
			{


				//auto scriptFactory = IFormFactory::GetConcreteFormFactoryByType<Script>();
				//if (!scriptFactory) {
				//	return 0.0;
				//}

				//Script* script = scriptFactory->Create();
				//if (!script) {
				//	return 0.0;
				//}

				UInt32 opcodeOffset = 0x4;

				if (thisActor == NULL || anotherActor == NULL || TopicID == NULL)
					return 0.5;


				SCRIPT_REFERENCED_OBJECT arg1;
				//memset(&arg1.form_name, 0, sizeof(BSString));
				arg1.editorID = "";
				arg1.form = anotherActor;
				SCRIPT_REFERENCED_OBJECT arg2;
				//memset(&arg2.form_name, 0, sizeof(BSString));
				arg2.editorID = "";
				arg2.form = TopicID;

				//BSSimpleList<SCRIPT_REFERENCED_OBJECT*> arglist2(&arg2, NULL);
				//BSSimpleList<SCRIPT_REFERENCED_OBJECT*> arglist1(&arg1, &arglist2);
				//RefListEntry arglist2;

				//arglist1.var = &arg1;
				//arglist2.var = &arg2;
				//arglist1.next = &arglist2;
				//arglist2.next = NULL;


				BSSimpleList<SCRIPT_REFERENCED_OBJECT*> reflist;
				reflist.push_front(&arg2);
				reflist.push_front(&arg1);

				initDummySayToScript();
				dummySayToScript->refObjects = reflist;

				g_is_obscript_say_say_to = true;
				g_silent_voice_duration_seconds = 5.0;

				sayToFunction->executeFunction(
					sayToFunction->params,
					(SCRIPT_FUNCTION::ScriptData*)sayToScriptData,
					thisActor,
					NULL,
					dummySayToScript,
					NULL,
					result,
					opcodeOffset
				);

				g_is_obscript_say_say_to = false;
				result = g_silent_voice_duration_seconds;
				return result + 0.5;

				//std::stringstream stream;
				//stream << "Say "
				//	<< std::setfill('0') << std::setw(8)
				//	<< std::hex << TopicID->formID;

				//_DMESSAGE(stream.str().c_str());

				//script->SetCommand(stream.str());

				//g_is_obscript_say_say_to = true;

				//script->Invoke(thisActor, RE::Script::InvokeType::kSysWindowCompileAndRun);

				//g_is_obscript_say_say_to = false;

				//result = g_silent_voice_duration_seconds;

				//delete script;
			}
			return result;
		}

		bool prepareForReinitializing(TESQuest* a_form)
		{
			a_form->alreadyRun = false;
			//TODO: stop
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

		//TODO: CHECK: should be Double?
		UInt32 getDestroyed(TESObjectREFR* reference) {
			if (getDestroyedFunction)
			{
				double result = 0.0;
				UInt32 opcodeOffset = 0x4;

				getDestroyedFunction->executeFunction(
					getDestroyedFunction->params,
					&getDestroyedFunctionData,
					reference,
					NULL,
					NULL,
					NULL,
					result,
					opcodeOffset
				);
				return (result != 0.0);
			}
			return 0;
		}

		bool RegisterFuncs(BSScript::Internal::VirtualMachine* a_vm) {

			_MESSAGE("Initializing ObScript hooks...");

			a_vm->RegisterFunction("PrepareForReinitializing", "Quest", prepareForReinitializing);
			
			
			a_vm->RegisterFunction("LegacySayTo", "ObjectReference", ObScriptSayTo);
			a_vm->RegisterFunction("LegacySay", "ObjectReference", ObScriptSay);
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

			_MESSAGE("Looking for IsAnimPlaying execute handler");

			getDestroyedFunction = SCRIPT_FUNCTION::LocateScriptCommand("GetDestroyed");			
			if (NULL == getDestroyedFunction)
			{
				_ERROR("Unable to find getDestroyedFunction!");
			}
			a_vm->RegisterFunction("GetDestroyed", "ObjectReference", getDestroyed);

			sayFunction = SCRIPT_FUNCTION::LocateScriptCommand("Say");
			sayToFunction = SCRIPT_FUNCTION::LocateScriptCommand("SayTo");


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
