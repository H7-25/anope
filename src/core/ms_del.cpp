/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class MemoDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	MemoInfo *mi;
 public:
	MemoDelCallback(User *_u, ChannelInfo *_ci, MemoInfo *_mi, const std::string &list) : NumberList(list, true), u(_u), ci(_ci), mi(_mi)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > mi->memos.size())
			return;

		if (ci)
		{
			FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, Number - 1));
		}
		else
		{
			FOREACH_MOD(I_OnMemoDel, OnMemoDel(u->Account(), mi, Number - 1));
		}

		delmemo(mi, Number - 1);
	}
};

class CommandMSDel : public Command
{
 public:
	CommandMSDel() : Command("DEL", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		ci::string numstr = params.size() ? params[0] : "", chan;
		unsigned i;
		int last;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				notice_lang(Config.s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (readonly)
			{
				notice_lang(Config.s_MemoServ, u, READ_ONLY_MODE);
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
		{
			mi = &u->Account()->memos;
		}
		if (numstr.empty() || (!isdigit(numstr[0]) && numstr != "ALL" && numstr != "LAST"))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			if (isdigit(numstr[0]))
				(new MemoDelCallback(u, ci, mi, numstr.c_str()))->Process();
			else if (numstr == "LAST")
			{
				/* Delete last memo. */
				for (i = 0; i < mi->memos.size(); ++i)
					last = mi->memos[i]->number;
				if (ci)
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, last));
				}
				else
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(u->Account(), mi, last))
				}
				delmemo(mi, last);
				notice_lang(Config.s_MemoServ, u, MEMO_DELETED_ONE, last);
			}
			else
			{
				if (ci)
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, 0));
				}
				else
				{
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(u->Account(), mi, 0));
				}
				/* Delete all memos. */
				for (i = 0; i < mi->memos.size(); ++i)
				{
					delete [] mi->memos[i]->text;
					delete mi->memos[i];
				}
				mi->memos.clear();
				if (!chan.empty())
					notice_lang(Config.s_MemoServ, u, MEMO_CHAN_DELETED_ALL, chan.c_str());
				else
					notice_lang(Config.s_MemoServ, u, MEMO_DELETED_ALL);
			}

			/* Reset the order */
			for (i = 0; i < mi->memos.size(); ++i)
				mi->memos[i]->number = i + 1;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_DEL);
	}
};

class MSDel : public Module
{
 public:
	MSDel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
	}
};

MODULE_INIT(MSDel)
