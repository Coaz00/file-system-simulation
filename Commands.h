#ifndef COMMANDS_H
#define COMMANDS_H

#define BACKSLASH "\\"

#include "Exceptions.h"
#include "FileSystemObjects.h"
#include<fstream>
#include<list>
#include<string>
#include<queue>

using namespace std;


class Command {
public:
	Command(const string& name, const string& command_input);
	virtual void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) = 0;

	string getCommandName() const;
	string getCommandInput() const;
protected:
	string name_;
	string command_input_;

	bool isSameStringLower(const string& first, const string& second); // case insensitive string equality comparision
};

class CommandLS : public Command {
public:
	CommandLS();
	void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) override;
private:
	void write(FileSystemObject* current, string& output);
};

class CommandCD : public Command {
public:
	CommandCD(const string& command_input);
	void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) override;
private:
	void setToParentDirectory(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath);
	void setToGivenDirectory(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath);
};

class CommandNEW : public Command {
public:
	CommandNEW(const string& command_input);
	void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) override;
private:
	void createFile(FileSystemObject* current);
	void createFolder(FileSystemObject* current);
	void parseString(string& name, string& extension, string& content);
};

class CommandDEL : public Command {
public:
	CommandDEL(const string& command_input);
	void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) override;
private:
	void deleteFile(FileSystemObject* current);
	void deleteFolder(FileSystemObject* current);
};

class CommandEXE : public Command {
public:
	CommandEXE(const string& command_input);
	void work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted) override;
private:
	FileSystemObject* findExeFile(FileSystemObject* current);
	queue<Command*> createQueue(const string& exe_content);
};


class CommandBase {
public:
	enum CommandName { LS, CD, NEW, DEL, EXE, DEF };

	queue<Command*>& getQueue();
	void createQueue(const string& filepath);
	void setQueue(queue<Command*> commands_queue);
private:
	queue<Command*> commands_queue_;
};


#endif