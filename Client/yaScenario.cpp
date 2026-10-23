#include "yaScenario.h"
#include "yaApplication.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
#include "yaPlayeScene.h"
#include "yaTransform.h"
#include "yaRigidbody.h"
#include "yaTime.h"
#include <algorithm>
#include <fstream>
#include <sstream>

extern ya::Application application;

namespace ya
{
	struct ScenarioSceneName
	{
		const char* name;
		eSceneType type;
	};

	const ScenarioSceneName ScenarioSceneNames[] =
	{
		{ "Title", eSceneType::Title },
		{ "Stage1", eSceneType::Stage1 },
		{ "Stage2", eSceneType::Stage2 },
		{ "Boss", eSceneType::Boss },
		{ "Tool", eSceneType::Tool },
	};

	// 상태 변수 count.<이름> 에 쓰는 레이어 이름 (eLayerType 순서)
	const char* ScenarioLayerNames[(UINT)eLayerType::End] =
	{
		"bg", "struct", "boss", "tile", "gate", "monster", "player",
		"bullet", "enemybullet", "ground", "test", "effect", "ui",
	};

	static bool ScenarioIsOperator(const std::string& op)
	{
		return op == "<" || op == "<=" || op == ">" || op == ">=" || op == "==" || op == "!=";
	}

	static bool ScenarioCompare(double actual, const std::string& op, double value)
	{
		const double epsilon = 0.0001;
		if (op == "<")	return actual < value;
		if (op == "<=")	return actual <= value + epsilon;
		if (op == ">")	return actual > value;
		if (op == ">=")	return actual >= value - epsilon;
		if (op == "==")	return fabs(actual - value) <= epsilon;
		if (op == "!=")	return fabs(actual - value) > epsilon;
		return false;
	}

	static std::string ScenarioNarrow(const std::wstring& text)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string result(size > 0 ? size - 1 : 0, '\0');
		WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, result.data(), size, nullptr, nullptr);
		return result;
	}

	static std::string ScenarioFormat(double value)
	{
		char buffer[32] = {};
		snprintf(buffer, sizeof(buffer), "%.6g", value);
		return buffer;
	}

	Scenario::Scenario()
		: mStartScene(eSceneType::Title)
		, mSeed(1)
		, mFrames(600)
		, mFrame(0)
		, mStateLog(nullptr)
		, mFinished(false)
	{
	}

	Scenario::~Scenario()
	{
		if (mStateLog != nullptr)
			fclose(mStateLog);
	}

	bool Scenario::Load(const std::wstring& path, const std::wstring& outDir)
	{
		mPath = path;
		mOutDir = outDir;

		std::ifstream file{ std::filesystem::path(path) };
		if (!file)
		{
			fprintf(stderr, "scenario: cannot open %s\n", ScenarioNarrow(path).c_str());
			return false;
		}

		std::string line;
		int lineNumber = 0;
		while (std::getline(file, line))
		{
			lineNumber++;
			if (lineNumber == 1 && line.rfind("\xEF\xBB\xBF", 0) == 0)
				line.erase(0, 3); // UTF-8 BOM

			// '#' 뒤는 주석
			std::string text = line.substr(0, line.find('#'));
			text.erase(text.find_last_not_of(" \t\r") + 1);

			std::istringstream tokens(text);
			std::string command;
			if (!(tokens >> command))
				continue;

			if (command == "scene")
			{
				std::string name;
				tokens >> name;

				bool found = false;
				for (const ScenarioSceneName& scene : ScenarioSceneNames)
				{
					if (name == scene.name)
					{
						mStartScene = scene.type;
						found = true;
					}
				}
				if (!found)
					return parseError(lineNumber, "unknown scene '" + name + "'");
			}
			else if (command == "seed")
			{
				if (!(tokens >> mSeed))
					return parseError(lineNumber, "usage: seed <number>");
			}
			else if (command == "frames")
			{
				if (!(tokens >> mFrames) || mFrames == 0)
					return parseError(lineNumber, "usage: frames <count>");
			}
			else if (command == "dt")
			{
				double deltaTime = 0.0;
				if (!(tokens >> deltaTime) || deltaTime <= 0.0)
					return parseError(lineNumber, "usage: dt <seconds>");
				Time::SetFixedDeltaTime(deltaTime);
			}
			else if (command == "hold" || command == "press")
			{
				std::string keyName;
				KeyHold hold = {};
				tokens >> keyName >> hold.from;
				if (command == "hold")
					tokens >> hold.to;
				else
					hold.to = hold.from;

				if (tokens.fail() || hold.to < hold.from)
					return parseError(lineNumber, command == "hold" ? "usage: hold <KEY> <from> <to>" : "usage: press <KEY> <frame>");
				if (!Input::FindKeyCode(keyName, hold.key))
					return parseError(lineNumber, "unknown key '" + keyName + "'");
				mHolds.push_back(hold);
			}
			else if (command == "shot")
			{
				UINT frame = 0;
				if (!(tokens >> frame))
					return parseError(lineNumber, "usage: shot <frame>");
				mShots.push_back(frame);
			}
			else if (command == "expect" || command == "always")
			{
				Check check = {};
				check.line = lineNumber;
				check.always = (command == "always");
				if (!check.always)
					tokens >> check.frame;
				tokens >> check.var >> check.op >> check.value;

				if (tokens.fail() || !ScenarioIsOperator(check.op))
					return parseError(lineNumber, check.always ? "usage: always <var> <op> <number>" : "usage: expect <frame> <var> <op> <number>");
				check.text = text;
				mChecks.push_back(check);
			}
			else
			{
				return parseError(lineNumber, "unknown command '" + command + "'");
			}
		}

		std::error_code error;
		std::filesystem::create_directories(std::filesystem::path(outDir), error);
		_wfopen_s(&mStateLog, (outDir + L"\\state.jsonl").c_str(), L"w");
		if (mStateLog == nullptr)
		{
			fprintf(stderr, "scenario: cannot write %s\\state.jsonl\n", ScenarioNarrow(outDir).c_str());
			return false;
		}
		return true;
	}

	void Scenario::Start()
	{
		Input::SetScripted(true);
		srand(mSeed);

		if (mStartScene != SceneManager::GetActiveSceneType())
			SceneManager::LoadScene(mStartScene);

		// 대본에 쓴 변수 이름이 실제 상태 변수에 있는지 미리 확인한다.
		std::map<std::string, double> state;
		collectState(state);
		for (const Check& check : mChecks)
		{
			if (state.find(check.var) == state.end())
				mErrors.push_back("line " + std::to_string(check.line) + ": unknown variable '" + check.var + "'");
		}
		if (!mErrors.empty())
			finish();
	}

	void Scenario::BeginFrame()
	{
		if (mFinished)
			return;

		bool down[(UINT)eKeyCode::END] = {};
		for (const KeyHold& hold : mHolds)
		{
			if (hold.from <= mFrame && mFrame <= hold.to)
				down[(UINT)hold.key] = true;
		}

		for (UINT i = 0; i < (UINT)eKeyCode::END; i++)
			Input::SetScriptedKey((eKeyCode)i, down[i]);
	}

	void Scenario::EndFrame()
	{
		if (mFinished)
			return;

		std::map<std::string, double> state;
		collectState(state);
		writeState(state);

		for (Check& check : mChecks)
		{
			if (check.failed || (!check.always && check.frame != mFrame))
				continue;

			double actual = state[check.var];
			if (!ScenarioCompare(actual, check.op, check.value))
			{
				check.failed = true;
				check.failedFrame = mFrame;
				check.actual = actual;
			}
		}

		if (std::find(mShots.begin(), mShots.end(), mFrame) != mShots.end())
			application.SaveScreenshot(mOutDir + L"\\shot_" + std::to_wstring(mFrame) + L".bmp");

		// 위치·속도가 NaN/무한대가 되면 이후 결과는 의미가 없으므로 바로 실패로 끝낸다.
		for (const char* var : { "player.x", "player.y", "player.vx", "player.vy" })
		{
			if (!std::isfinite(state[var]))
			{
				mErrors.push_back("frame " + std::to_string(mFrame) + ": " + var + " is not finite");
				mFrame++;
				finish();
				return;
			}
		}

		mFrame++;
		if (mFrame >= mFrames)
			finish();
	}

	void Scenario::collectState(std::map<std::string, double>& state)
	{
		Scene* scene = SceneManager::GetActiveScene();
		state["frame"] = mFrame;
		state["scene"] = (double)SceneManager::GetActiveSceneType();

		// 플레이어 = 플레이 씬에서 지금 조작 중인 스컬
		PlayeScene* playScene = dynamic_cast<PlayeScene*>(scene);
		Skul* skul = (playScene != nullptr) ? playScene->GetSkul() : nullptr;

		Vector2 pos = Vector2::Zero;
		Vector2 velocity = Vector2::Zero;
		bool ground = false;
		if (skul != nullptr)
		{
			pos = skul->GetComponent<Transform>()->GetPos();
			Rigidbody* rigidbody = skul->GetComponent<Rigidbody>();
			if (rigidbody != nullptr)
			{
				velocity = rigidbody->GetVelocity();
				ground = rigidbody->GetGround();
			}
		}
		state["player.found"] = (skul != nullptr);
		state["player.x"] = pos.x;
		state["player.y"] = pos.y;
		state["player.vx"] = velocity.x;
		state["player.vy"] = velocity.y;
		state["player.ground"] = ground;
		state["player.state"] = (skul != nullptr) ? (double)skul->GetSkulState() : -1.0;

		// 레이어별 활성 오브젝트 수, 전체 오브젝트 수 (누수 감시용)
		double objects = 0.0;
		for (UINT i = 0; i < (UINT)eLayerType::End; i++)
		{
			if (ScenarioLayerNames[i] == nullptr)
				continue;

			double active = 0.0;
			for (GameObject* gameObj : scene->GetGameObjects((eLayerType)i))
			{
				if (gameObj != nullptr && gameObj->GetState() == GameObject::eState::Active)
					active++;
			}
			state[std::string("count.") + ScenarioLayerNames[i]] = active;
			objects += (double)scene->GetGameObjects((eLayerType)i).size();
		}
		state["objects"] = objects;
	}

	void Scenario::writeState(const std::map<std::string, double>& state)
	{
		fprintf(mStateLog, "{");
		bool first = true;
		for (const auto& [name, value] : state)
		{
			fprintf(mStateLog, first ? "\"%s\":" : ",\"%s\":", name.c_str());
			if (std::isfinite(value))
				fprintf(mStateLog, "%s", ScenarioFormat(value).c_str());
			else
				fprintf(mStateLog, "null");
			first = false;
		}
		fprintf(mStateLog, "}\n");
	}

	void Scenario::finish()
	{
		mFinished = true;
		if (mStateLog != nullptr)
		{
			fclose(mStateLog);
			mStateLog = nullptr;
		}

		std::vector<std::string> failures = mErrors;
		for (const Check& check : mChecks)
		{
			std::string where = "line " + std::to_string(check.line) + " '" + check.text + "'";
			if (check.failed)
				failures.push_back(where + ": failed at frame " + std::to_string(check.failedFrame) + " (actual " + ScenarioFormat(check.actual) + ")");
			else if (!check.always && check.frame >= mFrame && mErrors.empty())
				failures.push_back(where + ": frame never reached (ran " + std::to_string(mFrame) + " frames)");
		}

		bool passed = failures.empty();
		std::string summary = std::string(passed ? "PASS " : "FAIL ") + ScenarioNarrow(mPath)
			+ " frames=" + std::to_string(mFrame) + " checks=" + std::to_string(mChecks.size());

		FILE* result = nullptr;
		_wfopen_s(&result, (mOutDir + L"\\result.txt").c_str(), L"w");

		fprintf(stderr, "%s\n", summary.c_str());
		if (result != nullptr)
			fprintf(result, "%s\n", summary.c_str());
		for (const std::string& failure : failures)
		{
			fprintf(stderr, "  %s\n", failure.c_str());
			if (result != nullptr)
				fprintf(result, "  %s\n", failure.c_str());
		}
		if (result != nullptr)
			fclose(result);

		PostQuitMessage(passed ? 0 : 1);
	}

	bool Scenario::parseError(int line, const std::string& message)
	{
		fprintf(stderr, "%s:%d: %s\n", ScenarioNarrow(mPath).c_str(), line, message.c_str());
		return false;
	}
}
