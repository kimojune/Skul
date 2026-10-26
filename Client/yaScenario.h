#pragma once
#include "YamYamEngine.h"
#include "yaInput.h"

namespace ya
{
	// 자동 검증용 입력 대본 실행기.  Client.exe --scenario <대본.txt> <출력 폴더>
	// 대본 문법과 상태 변수 목록은 Scenarios/README.md 참고.
	class Scenario
	{
	public:
		Scenario();
		~Scenario();

		bool Load(const std::wstring& path, const std::wstring& outDir);
		void Start();
		void BeginFrame();
		void EndFrame();

		// 게임 코드가 시나리오 상태 변수를 내보낸다 (예: Scenario::Publish("boss.hp", hp)).
		// 대본의 변수 이름은 시작할 때 검사하므로 Initialize에서 초깃값을 한 번 내보내야 한다.
		static void Publish(const std::string& name, double value) { mPublished[name] = value; }

	private:
		struct KeyHold
		{
			eKeyCode key;
			UINT from;
			UINT to;
		};

		struct Check
		{
			int line;
			bool always;
			UINT frame;
			std::string var;
			std::string op;
			double value;
			bool failed;
			UINT failedFrame;
			double actual;
			std::string text;
		};

		void collectState(std::map<std::string, double>& state);
		void writeState(const std::map<std::string, double>& state);
		void finish();
		bool parseError(int line, const std::string& message);

		std::wstring mPath;
		std::wstring mOutDir;
		eSceneType mStartScene;
		UINT mSeed;
		UINT mFrames;
		UINT mFrame;
		std::vector<KeyHold> mHolds;
		std::vector<UINT> mShots;
		std::vector<std::pair<UINT, int>> mDamages; // (프레임, 피해량)
		struct Teleport
		{
			UINT frame;
			float x;
			float y;
		};
		std::vector<Teleport> mTeleports;
		std::vector<Check> mChecks;
		std::vector<std::string> mErrors;
		FILE* mStateLog;
		bool mFinished;

		static std::map<std::string, double> mPublished;
	};
}
