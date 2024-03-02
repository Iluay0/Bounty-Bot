#include "BaseModule.h"

// Sub-Modules
#include "BountyModule.h"

BaseModule::BaseModule()
{
}

BaseModule::~BaseModule()
{
}

void BaseModule::addCommand(EventType eventType, std::string name, std::function<void(dpp::cluster& bot, const dpp::event_dispatch_t&)> func, const std::vector<Command> options, std::string description)
{
	std::string fullName = getFullName();
	if (!fullName.empty())
		name = fullName + "-" + name;
	m_commands[name] = Command(eventType, name, description, options, func);
}

void BaseModule::addModule(std::shared_ptr<BaseModule> module, BaseModule* parentModule)
{
	module->setParentModule(parentModule);
	module->bindCommands();
	module->createChildModules();

	m_subModules.insert({ module->getName(), module });
}

bool BaseModule::onEvent(dpp::cluster& bot, EventType eventType, const dpp::event_dispatch_t& event)
{
	for (const auto& [name, command] : m_commands)
	{
		if (eventType != command.eventType)
			continue;

		switch (eventType)
		{
			case EventType::SlashCommand:
			{
				const dpp::slashcommand_t& slashCommand = dynamic_cast<const dpp::slashcommand_t&>(event);
				if (slashCommand.command.get_command_name() == command.name)
				{
					command.func(bot, slashCommand);
					return true;
				}
			} break;
			case EventType::FormSubmit:
			{
				const dpp::form_submit_t& formSubmit = dynamic_cast<const dpp::form_submit_t&>(event);
				if (formSubmit.custom_id == command.name)
				{
					command.func(bot, formSubmit);
					return true;
				}
			} break;
			case EventType::Ready:
			{
				command.func(bot, event);
			} break;
		}
	}
	return false;
}

bool BaseModule::dispatchEvent(dpp::cluster& bot, EventType eventType, const dpp::event_dispatch_t& event)
{
	// Check own events.
	if (onEvent(bot, eventType, event))
		return true;

	// Dispatch event to submodules.
	for (const auto& [name, subModule] : m_subModules)
	{
		if (subModule->dispatchEvent(bot, eventType, event))
			return true;
	}

	// No event found in this module or its submodules.
	return false;
}

std::vector<Command> BaseModule::getAllSlashCommands() const
{
	std::vector<Command> slashCommands;

	// Concatenate own commands.
	for (const auto& [name, command] : m_commands)
	{
		if (command.eventType == EventType::SlashCommand)
			slashCommands.push_back(command);
	}

	// Concatenate submodule commands.
	for (const auto& [name, subModule] : m_subModules)
	{
		std::vector<Command> subModuleSlashCommands = subModule->getAllSlashCommands();
		slashCommands.insert(slashCommands.end(), subModuleSlashCommands.begin(), subModuleSlashCommands.end());
	}

	return slashCommands;
}

std::string BaseModule::getFullName()
{
	std::string fullName = "";

	if (m_ParentModule != nullptr && m_ParentModule->hasName())
		fullName += m_ParentModule->getFullName() + "-";
	fullName += getName();

	return fullName;
}
