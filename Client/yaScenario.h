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
		std::vector<Check> mChecks;
		std::vector<std::string> mErrors;
		FILE* mStateLog;
		bool mFinished;
	};
}
