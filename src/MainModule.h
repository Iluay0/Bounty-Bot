#pragma once

#include "BaseModule.h"
#include "BountyModule.h"

class MainModule : public BaseModule
{
public:
	MainModule()
	{

	}
	~MainModule()
	{

	}

	void createChildModules() override
	{
		addModule(std::shared_ptr<BountyModule>(new BountyModule()), this);
	};

	void bindCommands() override
	{
		addCommand(EventType::SlashCommand, "set-env",
			[&](dpp::cluster& bot, const dpp::event_dispatch_t& event)
			{
				const dpp::slashcommand_t& slashCommand = dynamic_cast<const dpp::slashcommand_t&>(event);
				dpp::command_interaction cmdData = slashCommand.command.get_command_interaction();
				std::string guildId = std::to_string(slashCommand.command.guild_id);

				bool set = false;
				for (auto& subcommand : cmdData.options)
				{
					if (subcommand.name == "bounty-channel")
					{
						dpp::channel channel = slashCommand.command.get_resolved_channel(std::get<dpp::snowflake>(subcommand.value));
						util::env[guildId]["channelIdBounties"] = std::stoll(channel.id.str());
						set = true;
					}
					else if (subcommand.name == "solved-bounty-channel")
					{
						dpp::channel channel = slashCommand.command.get_resolved_channel(std::get<dpp::snowflake>(subcommand.value));
						util::env[guildId]["channelIdSolvedBounties"] = std::stoll(channel.id.str());
						set = true;
					}
				}

				if (!set)
				{
					slashCommand.reply(dpp::message("No option specified.").set_flags(dpp::m_ephemeral));
					return;
				}

				std::ofstream envFile("env.json");
				if (!envFile.is_open())
				{
					slashCommand.reply(dpp::message("Environment file could not be opened.").set_flags(dpp::m_ephemeral));
					return;
				}

				envFile << std::setw(4) << util::env << std::endl;
				envFile.close();

				slashCommand.reply(dpp::message("Environment set.").set_flags(dpp::m_ephemeral));
			},
			{
				Command(EventType::None, "bounty-channel", "Sets the channel for bounties.", {}, {}, dpp::co_channel),
				Command(EventType::None, "solved-bounty-channel", "Sets the channel for solved bounties.", {}, {}, dpp::co_channel)
			}, "Sets an environment variable for this guild.");
	};
};