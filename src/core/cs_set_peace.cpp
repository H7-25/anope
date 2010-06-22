/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSetPeace : public Command
{
 public:
	CommandCSSetPeace(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1] == "ON")
		{
			ci->SetFlag(CI_PEACE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PEACE_ON, ci->name.c_str());
		}
		else if (params[1] == "OFF")
		{
			ci->UnsetFlag(CI_PEACE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PEACE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "PEACE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_PEACE, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET PEACE", CHAN_SET_PEACE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_PEACE, "SET");
	}
};

class CommandCSSASetPeace : public CommandCSSetPeace
{
 public:
	CommandCSSASetPeace(const ci::string &cname) : CommandCSSetPeace(cname, "chanserv/saset/peace")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_PEACE, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET PEACE", CHAN_SASET_PEACE_SYNTAX);
	}
};

class CSSetPeace : public Module
{
 public:
	CSSetPeace(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetPeace("PEACE"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetPeace("PEACE"));
	}

	~CSSetPeace()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("PEACE");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("PEACE");
	}
};

MODULE_INIT(CSSetPeace)
