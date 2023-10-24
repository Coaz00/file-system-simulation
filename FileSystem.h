#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Commands.h"
#include "Exceptions.h"
#include "FileSystemObjects.h"
#include <fstream>
#include <string>

#define BACKSLASH '\\'

using namespace std;

class FileSystem {
public:
	FileSystem();
	~FileSystem();

	void loadHierarchy(const string& fs_filepath, const string& log_filepath); 
	void execute(const string& filepath);

	FileSystemObject* getRoot() const;
	FileSystemObject* getCurrent() const;

private:
	string log_filepath_; // path to output file
	FileSystemObject* root_object_; // root of FileSystem hierarchy tree
	FileSystemObject* current_object_; // current directory
	bool alreadyPrinted_; // whether or not output file has been created

	void deleteHierarchy(FileSystemObject*& root); // delete tree (can also delete subtree)
	void extractFromString(string& name, string& extension, const string& tmp);
	void prepareForFile(string& name, string& extension, string& content, const string& tmp, const string& line, int& pos);
	void findFolder(const string& name); //sets current to folder with name name
};


#endif