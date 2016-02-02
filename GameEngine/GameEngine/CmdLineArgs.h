#pragma once

namespace Lemur
{

enum CmdCommands : int
{
	VERSION = 1,
	ABOUT = 1 << 1,
};


class CmdLineArgs
{
public:
	CmdLineArgs() = default;
	CmdLineArgs(CmdLineArgs&) = default;

	CmdLineArgs(int argc, char* argv[])
	{
		parse(argc, argv);
	}

	void parse(int argc, char* argv[]);

	bool has_command(CmdCommands cmd) const;

private:
	int m_commands;
};

inline void CmdLineArgs::parse (int argc, char* argv[])
{
	for (int x = 0; x < argc; ++x)
	{
		const char* cmd = argv[0];

		if (cmd == "-version") m_commands &= VERSION;
		else if (cmd == "-about") m_commands &= ABOUT;
	}
}

	inline bool CmdLineArgs::has_command(CmdCommands cmd) const
	{
		return (m_commands & cmd);
	}
}// namespace Lemur