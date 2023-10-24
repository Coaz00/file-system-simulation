#include "FileSystem.h"

FileSystem::FileSystem() : log_filepath_(""), root_object_(new Folder("")), current_object_(root_object_), alreadyPrinted_(false)
{
}

FileSystem::~FileSystem()
{
	deleteHierarchy(root_object_);
	delete root_object_;
}

FileSystemObject* FileSystem::getRoot() const
{
	return root_object_;
}

void FileSystem::execute(const string& filepath) {
	CommandBase* base = new CommandBase();

	base->createQueue(filepath);//creates queue with commands to execute

	while (!base->getQueue().empty()) {
		try {
			base->getQueue().front()->work(root_object_, current_object_, log_filepath_, alreadyPrinted_);
		}
		catch (...) {
			if (!alreadyPrinted_) {
				fstream output(log_filepath_, ios::out);
				output << "Error: ";
				output << base->getQueue().front()->getCommandName();
				output << " ";
				output << base->getQueue().front()->getCommandInput();
				output << "\n";
				alreadyPrinted_ = true;
				output.close();
			}
			else {
				fstream output(log_filepath_, ios::app);
				output << "Error: ";
				output << base->getQueue().front()->getCommandName();
				output << " ";
				output << base->getQueue().front()->getCommandInput();
				output << "\n";
				alreadyPrinted_ = true;
				output.close();
			}
		}
		delete base->getQueue().front();
		base->getQueue().pop();
	}
	current_object_ = root_object_;
	delete base;
}


FileSystemObject* FileSystem::getCurrent() const
{
	return current_object_;
}

void FileSystem::loadHierarchy(const string& fs_filepath, const string& log_filepath)
{
	log_filepath_ = log_filepath;

	deleteHierarchy(root_object_);

	fstream input(fs_filepath, ios::in); // opening input file in mode for reading

	string line; //used to store a line

	//iterating until we reach end of file
	while (getline(input,line)) {
		line += "\n";
		int pos = 0; // used to go through string line
		current_object_ = root_object_; // reseting current pointer to root
		string tmp, extension, name, content;

		//check if start with '\'
		try {

			if (line[pos] != BACKSLASH) throw MissingBackslash();

			pos++; // skipping /

			//going through whole line
			while (pos < line.size()) {
				tmp.clear();
				//storing a name of 1 object
				while (line[pos] != BACKSLASH && line[pos] != ' ' && line[pos] != '\n') tmp += line[pos++];
				
				name.clear();
				extension.clear();
				//in this case we have a file so we extract its name and extension from tmp
				if (line[pos] == ' ') {
					prepareForFile(name, extension, content, tmp, line, pos); // parsing valuable info from tmp and line

					FileSystemObject* object = nullptr;
					// according to extension we make exe or text file
					if (extension == "txt") object = new TextFile(name, content);
					else if (extension == "exe") object = new ExeFile(name, content);


					current_object_->addToTree(object, current_object_);

				}
				else if (line[pos] == '\n') {
					//create folder
					FileSystemObject* object = new Folder(tmp);

					current_object_->addToTree(object, current_object_);

					pos++; //skipping \n
				}
				else if (line[pos] == BACKSLASH) {
					findFolder(tmp);
					pos++; // skipping /
				}
			}
		}
		catch (MissingBackslash& e) {
			cout << e.what() << endl;
			exit(1);
		}
		catch (InvalidName& e) {
			cout << e.what() << endl;
			exit(1);
		}
		catch (InvalidExtension& e) {
			cout << e.what() << endl;
			exit(1);
		}
		catch (FolderNotFound& e) {
			cout << e.what() << endl;
			exit(1);
		}
		catch (AlreadyExists& e) {
			cout << e.what() << endl;
			exit(1);
		}
	}
	input.close();
	current_object_ = root_object_;
	
}

void FileSystem::extractFromString(string& name, string& extension, const string& tmp)
{
	int i = 0;

	while (tmp[i] != '.') {
		bool viableChar = (tmp[i] >= 'a' && tmp[i] <= 'z') || (tmp[i] >= 'A' && tmp[i] <= 'Z') ||
			(tmp[i] >= '0' && tmp[i] <= '9') || tmp[i] == '_';
		if (!viableChar) throw InvalidName();
		name += tmp[i++]; //everything before '.' storing in name
	}
	if (name.empty()) throw InvalidName();
	i++; //skipping .
	while (i < tmp.size()) extension += tmp[i++];
	if (extension != "txt" && extension != "exe") throw InvalidExtension();

}

void FileSystem::prepareForFile(string& name, string& extension, string& content, const string& tmp, const string& line, int& pos)
{
	extractFromString(name, extension, tmp);

	pos++; //skipping ' ';

	content.clear();
	while (line[pos] != '\n') content += line[pos++]; //storing content

	pos++; //skipping \n
}

void FileSystem::findFolder(const string& name)
{
	//going through list of pointers to objects
	for (auto const& i : current_object_->getFolderContent())
		//checks if object has same name as requested and if object is a folder
		if (i->getName() == name && i->getType() == 'F') {
			current_object_ = i;
			return;
		}
	throw FolderNotFound();
}

void FileSystem::deleteHierarchy(FileSystemObject*& root) {
	if (root->getFolderContent().empty()) return; // if tree is empty we don't do anything
	root->deleteObject();
	root = new Folder("");
}
