#include "Commands.h"

Command::Command(const string& name, const string& command_input) : name_(name), command_input_(command_input)
{
}

string Command::getCommandName() const
{
    return name_;
}

string Command::getCommandInput() const
{
    return command_input_;
}

bool Command::isSameStringLower(const string& first, const string& second)
{
    int pos = 0;
    while (pos < first.size() && pos < second.size()) {
        if (tolower(first[pos]) != tolower(second[pos])) return false;
        pos++;
    }
    
    return true;
}

CommandLS::CommandLS() : Command("LS", "")
{
}

void CommandLS::work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted)
{
    if (!alreadyPrinted) {
        fstream output(log_filepath, ios::out); // creating output file with path given in loadHierarchy
        alreadyPrinted = true;
        string output_string;
        write(current, output_string);
        output_string.pop_back();
        output << output_string;
        output.close();
    }
    else {
        fstream output(log_filepath, ios::app); //opening output file for appending (if its already created)
        output << endl;
        string output_string;
        write(current, output_string);
        output_string.pop_back();
        output << output_string;
        output.close();
    }

}

void CommandLS::write(FileSystemObject* current, string& output)
{
    string path;
    current->printName(output, path, current);
}


void CommandCD::setToParentDirectory(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath)
{
    if (current->getParent() != nullptr) current = current->getParent();
    else throw FolderNotFound();
}

void CommandCD::setToGivenDirectory(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath)
{
    for (auto const& i : current->getFolderContent()) 
        if (isSameStringLower(i->getName(), command_input_)) {
            current = i;
            return;
        }
    throw FolderNotFound();
}

CommandCD::CommandCD(const string& command_input) : Command("CD", command_input)
{
}

void CommandCD::work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted)
{
    if (command_input_ == "..") {
        setToParentDirectory(root, current, log_filepath);
    }
    else {
        setToGivenDirectory(root, current, log_filepath);
    }
}

CommandNEW::CommandNEW(const string& command_input) : Command("NEW", command_input)
{
}

void CommandNEW::work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted)
{
    int hasSpace = 0; //check if input has spaces
    for (int i = 0; i < command_input_.size(); i++)
        if (command_input_[i] == ' ') hasSpace = 1;

    if (hasSpace) createFile(current);
    else createFolder(current);
}

void CommandNEW::createFile(FileSystemObject* current)
{
    string name, extension, content;
    parseString(name, extension, content);

    FileSystemObject* object = nullptr;
    if (extension == "txt") object = new TextFile(name, content);
    else if (extension == "exe") object = new ExeFile(name, content);

    current->addToTree(object, current);
}

void CommandNEW::createFolder(FileSystemObject* current)
{
    FileSystemObject* object = new Folder(command_input_);
    current->addToTree(object, current);
}

void CommandNEW::parseString(string& name, string& extension, string& content)
{
    int pos = 0;
    while (command_input_[pos] != '.') {
        bool viableChar = (command_input_[pos] >= 'a' && command_input_[pos] <= 'z') || (command_input_[pos] >= 'A' && command_input_[pos] <= 'Z') ||
            (command_input_[pos] >= '0' && command_input_[pos] <= '9') || command_input_[pos] == '_';
        if (!viableChar) throw InvalidName();
        
        name += command_input_[pos++]; // getting name
    }
    pos++; //skipping '.'
    while (command_input_[pos] != ' ') extension += command_input_[pos++]; // getting extension
    if (extension != "txt" && extension != "exe") throw InvalidExtension();
    pos++; // skipping ' '
    while (pos < command_input_.size()) content += command_input_[pos++]; // getting content
}

CommandDEL::CommandDEL(const string& command_input) : Command("DEL", command_input)
{
}

void CommandDEL::work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted)
{
    int hasDot = 0;
    for (int i = 0; i < command_input_.size(); i++)
        if (command_input_[i] == '.') hasDot = 1;
    
    if (hasDot) deleteFile(current);
    else deleteFolder(current);
    
}

void CommandDEL::deleteFile(FileSystemObject* current)
{
    list<FileSystemObject*>::iterator it;
    FileSystemObject* trash = nullptr;
    int found = 0;

    for (it = current->getFolderContent().begin(); it != current->getFolderContent().end(); it++) 
        if (isSameStringLower((*it)->getName(), command_input_)) {
            found = 1;
            break;
        }
    if (!found) throw FileNotFound();
    
    trash = *it;
    current->getFolderContent().erase(it);
    trash->deleteObject();
}

void CommandDEL::deleteFolder(FileSystemObject* current)
{
    list<FileSystemObject*>::iterator it;
    FileSystemObject* trash;
    int found = 0;

    for (it = current->getFolderContent().begin(); it != current->getFolderContent().end(); it++)
        if (isSameStringLower((*it)->getName(), command_input_)) {
            found = 1;
            break;
        }

    if (!found) throw FolderNotFound();

    trash = *it;
    current->getFolderContent().erase(it);
    trash->deleteObject();
}

CommandEXE::CommandEXE(const string& command_input) : Command("EXE", command_input)
{
}

void CommandEXE::work(FileSystemObject* root, FileSystemObject*& current, const string& log_filepath, bool& alreadyPrinted)
{

    FileSystemObject* exe_file = findExeFile(current);
    CommandBase* base = new CommandBase();
    base->setQueue(createQueue(exe_file->getContent()));
    while (!base->getQueue().empty()) {
        base->getQueue().front()->work(root, current, log_filepath, alreadyPrinted);
        base->getQueue().pop();
    }
    delete base;
}
FileSystemObject* CommandEXE::findExeFile(FileSystemObject* current)
{
    for (auto i : current->getFolderContent()) 
        if (isSameStringLower(i->getName(), command_input_)) return i;

    throw FileNotFound();
    return nullptr;
}

queue<Command*> CommandEXE::createQueue(const string& exe_content)
{
    queue<Command*> commands;
    string command_name;
    string command_input;
    int pos = 0;
    while (pos < exe_content.size()) {
        command_name.clear();
        command_input.clear();
        //storing command name
        while (exe_content[pos] != ' ' && !((exe_content[pos] == '\\') && exe_content[pos + 1] == 'n') && (pos < exe_content.size())) {
            command_name += exe_content[pos++];
        }
    
        if (exe_content[pos] != '\\' && pos < exe_content.size()) {
            pos++; // skipping ' '
            //storing command input
            while (!(exe_content[pos] == '\\' && exe_content[pos + 1] == 'n')) {
                command_input += exe_content[pos++];
            }
           
        }
        else pos += 2; //skipping \ and n

        Command* command;

        //creating command according to command name
        if (!command_name.empty()) {
            if (command_name == "LS") command = new CommandLS();
            else if (command_name == "CD") command = new CommandCD(command_input);
            else if (command_name == "NEW") command = new CommandNEW(command_input);
            else if (command_name == "DEL") command = new CommandDEL(command_input);
            else if (command_name == "EXE") command = new CommandEXE(command_input);
            else throw InvalidCommand();

            commands.push(command); // adds to queue
        }
    }
    return commands;
}

queue<Command*>& CommandBase::getQueue()
{
    return commands_queue_;
}

void CommandBase::createQueue(const string& filepath)
{
    fstream input(filepath, ios::in);

    string line;
    string name;
    string command_input;

    CommandName command_name = DEF;

    Command* cmd;

    while (getline(input, line)) {
        int i = 0;

        try {
            name.clear();
            while (line[i] != ' ' && i < line.size()) name += line[i++]; // extracting command name from line
            i++; //skipping ' '
            command_input.clear();
            while (i < line.size()) command_input += line[i++]; // extracting command input from line

            if (name == "LS") command_name = LS;
            else if (name == "CD") command_name = CD;
            else if (name == "NEW") command_name = NEW;
            else if (name == "DEL") command_name = DEL;
            else if (name == "EXE") command_name = EXE;

            //creating command according to name
            switch (command_name) {
            case LS:
                cmd = new CommandLS();
                break;
            case CD:
                cmd = new CommandCD(command_input);
                break;
            case NEW:
                cmd = new CommandNEW(command_input);
                break;
            case DEL:
                cmd = new CommandDEL(command_input);
                break;
            case EXE:
                cmd = new CommandEXE(command_input);
                break;
            default:
                throw InvalidCommand();
                break;
            }

            if (command_name != DEF) commands_queue_.push(cmd); // adding command to queue
        }
        catch (InvalidCommand& e) {
            cout << e.what() << endl;

        }
    }
}

void CommandBase::setQueue(queue<Command*> commands_queue)
{
    commands_queue_ = commands_queue;
}
