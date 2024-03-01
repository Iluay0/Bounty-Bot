#pragma once

#include "util.h"

enum EventType
{
	SlashCommand,
	FormSubmit,
	Ready
};

struct Command
{
	EventType eventType;
	std::string name;
	std::string description;
	std::function<void(dpp::cluster& bot, const dpp::event_dispatch_t&)> func;

	Command(const EventType& eventType, const std::string& name, const std::string& description, const std::function<void(dpp::cluster& bot, const dpp::event_dispatch_t&)>& func)
		: eventType(eventType), name(name), description(description), func(func)
	{
	}

	Command() = default;
};

class BaseModule
{
public:
	BaseModule();
	~BaseModule();

	bool onEvent(dpp::cluster& bot, EventType eventType, const dpp::event_dispatch_t& event);
	bool dispatchEvent(dpp::cluster& bot, EventType eventType, const dpp::event_dispatch_t& event);

	std::vector<Command> getAllSlashCommands() const;

	std::string getFullName();
	bool hasName() { return m_name != ""; };
	std::string getName() { return m_name; }
	std::string getDescription() { return m_description; }

	void setParentModule(BaseModule* parentModule) { m_ParentModule = parentModule; }
protected:
	void setName(std::string name) { m_name = name; }
	void setDescription(std::string description) { m_description = description; }

	void addCommand(EventType eventType, std::string name, std::function<void(dpp::cluster& bot, const dpp::event_dispatch_t&)> func, std::string description = "");
	virtual void bindCommands() {};

	void addModule(std::shared_ptr<BaseModule> module, BaseModule* parentModule = nullptr);
	virtual void createChildModules() {};
private:
	std::string m_name = "";
	std::string m_description = "";
	std::map<std::string, Command> m_commands;
	std::map<std::string, std::shared_ptr<BaseModule>> m_subModules;
	BaseModule* m_ParentModule = nullptr;
};