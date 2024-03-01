#pragma once

#include "BaseModule.h"

class BountyModule : public BaseModule
{
public:
	BountyModule()
	{
		setName("bounty");
	}
	~BountyModule()
	{

	}

	void bindCommands() override
	{
		addCommand(EventType::SlashCommand, "create",
			[&](dpp::cluster& bot, const dpp::event_dispatch_t& event)
			{
				const dpp::slashcommand_t& slashCommand = dynamic_cast<const dpp::slashcommand_t&>(event);

				dpp::interaction_modal_response modal(getFullName() + "-modal-create", "Create a bounty");

				modal.add_component(
					dpp::component()
					.set_label("Name")
					.set_id("bounty-name")
					.set_type(dpp::cot_text)
					.set_placeholder("Name of the bounty")
					.set_min_length(4)
					.set_max_length(64)
					.set_text_style(dpp::text_short)
				);

				slashCommand.dialog(modal);
			}, "Creates a bounty.");

		addCommand(EventType::SlashCommand, "solve",
			[&](dpp::cluster& bot, const dpp::event_dispatch_t& event)
			{
				const dpp::slashcommand_t& slashCommand = dynamic_cast<const dpp::slashcommand_t&>(event);

				const dpp::channel& thread = bot.channel_get_sync(slashCommand.command.channel_id);
				if (!thread.name.starts_with("[Unsolved]"))
				{
					slashCommand.reply(dpp::message("This channel is not a bounty, or it is already completed.").set_flags(dpp::m_ephemeral));
					return;
				}

				dpp::channel newThread = thread;
				util::replace(newThread.name, "[Unsolved]", "[Solved]");
				bot.channel_edit(newThread);

				std::string solvedMessage = "## " + newThread.name;
				solvedMessage += "\nLink: " + thread.get_url();

				dpp::message message(util::env["channelIdSolvedBounties"].get<uint64_t>(), solvedMessage);
				bot.message_create(message);

				slashCommand.reply(dpp::message("Bounty successfully solved.").set_flags(dpp::m_ephemeral));

			}, "Solves a bounty. Must be used in a bounty thread.");

		addCommand(EventType::FormSubmit, "modal-create",
			[&](dpp::cluster& bot, const dpp::event_dispatch_t& event)
			{
				const dpp::form_submit_t& formSubmit = dynamic_cast<const dpp::form_submit_t&>(event);

				const dpp::component& name = util::getFormComponent(formSubmit, "bounty-name");
				std::string bountyName = "[Unsolved] " + std::get<std::string>(name.value);

				dpp::thread thread;
				bot.thread_create(bountyName,
					util::env["channelIdBounties"].get<uint64_t>(),
					10080,
					dpp::channel_type::CHANNEL_PUBLIC_THREAD,
					false,
					0);

				formSubmit.reply(dpp::message("Bounty successfully created.").set_flags(dpp::m_ephemeral));
			});
	}

	void createChildModules() override
	{

	};
};