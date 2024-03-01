#include "util.h"

#include "MainModule.h"

std::shared_ptr<MainModule> m_MainModule;
nlohmann::json util::env;

void logFailedDispatch(dpp::cluster& bot, EventType eventType, const dpp::event_dispatch_t& event)
{
	switch (eventType)
	{
		case EventType::SlashCommand:
		{
			const dpp::slashcommand_t& slashCommand = dynamic_cast<const dpp::slashcommand_t&>(event);
			std::cout << std::format("SlashCommand Event {} was dispatched but a corresponding command was not found.", slashCommand.command.get_command_name()) << std::endl;
		} break;
		case EventType::FormSubmit:
		{
			const dpp::form_submit_t& formSubmit = dynamic_cast<const dpp::form_submit_t&>(event);
			std::cout << std::format("FormSubmit Event {} was dispatched but a corresponding command was not found.", formSubmit.custom_id) << std::endl;
		} break;
	}
}

void setupEvents(dpp::cluster& bot)
{
	bot.on_log(dpp::utility::cout_logger());

	bot.on_slashcommand([&bot](const dpp::event_dispatch_t& event)
		{
			if (!m_MainModule->dispatchEvent(bot, EventType::SlashCommand, event))
				logFailedDispatch(bot, EventType::SlashCommand, event);
		});

	bot.on_form_submit([&bot](const dpp::event_dispatch_t& event)
		{
			if (!m_MainModule->dispatchEvent(bot, EventType::FormSubmit, event))
				logFailedDispatch(bot, EventType::FormSubmit, event);
		});

	bot.on_ready([&bot](const dpp::event_dispatch_t& event)
		{
			if (dpp::run_once<struct register_bot_commands>())
			{
				for (const auto& command : m_MainModule->getAllSlashCommands())
				{
					bot.global_command_create(dpp::slashcommand(command.name, command.description, bot.me.id));
				}
			}

			m_MainModule->dispatchEvent(bot, EventType::Ready, event);
		});

	bot.start(dpp::st_wait);
}

int main()
{
	m_MainModule = std::shared_ptr<MainModule>(new MainModule());
	m_MainModule->createChildModules();

	try
	{
		util::env = nlohmann::json::parse(std::ifstream("env.json"));
	}
	catch (nlohmann::json::parse_error e)
	{
		std::cout << "Error while loading env.json: " << e.what() << std::endl;
		return 0;
	}

	dpp::cluster bot(util::env["botToken"]);
	setupEvents(bot);

	return 0;
}
