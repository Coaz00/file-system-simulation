#ifndef FILE_SYSTEM_OBJECTS_H
#define FILE_SYSTEM_OBJECTS_H

#include "Exceptions.h"
#include<iostream>
#include<list>
#include<string>
#include<queue>

using namespace std;

class FileSystemObject {
public:
	FileSystemObject(const string& name);

	void setName(const string& name);
	void setParent(FileSystemObject* parent);

	virtual string getName() const;
	virtual char getType() const = 0; // helps us determine wheter object is file (returns f) or folder (returns F)
	FileSystemObject* getParent() const;
	virtual list<FileSystemObject*>& getFolderContent();
	virtual string getContent() const;

	virtual void printName(string& output, string path, FileSystemObject* root);
	virtual void addToTree(FileSystemObject* object, FileSystemObject* parent);
	virtual bool isLeaf() const = 0;
	bool compare(const FileSystemObject* first, const FileSystemObject* second) const; //case insensitive comaparision of 2 objects' names

	virtual void deleteObject() = 0;
protected:
	string name_;
	FileSystemObject* parent_; // Folder which contains this object
};

class Folder : public FileSystemObject {
public:
	Folder(const string& name);
	~Folder();

	void addToTree(FileSystemObject* object, FileSystemObject* parent) override;
	char getType() const override;
	list<FileSystemObject*>& getFolderContent()  override;
	bool isLeaf() const override;
	void printName(string& output, string path, FileSystemObject* root) override;
	void deleteObject() override;
private:
	list<FileSystemObject*> folder_content_; // folder may contain other folders or files
};

class File : public FileSystemObject {
public:
	File(const string& name, const string& file_content, const string& extension);

	string getFileContent() const;
	string getExtension() const;
	string getName() const override; // returns full name(name.extension)
	char getType() const override;


	void setFileContent(const string& file_content);
	void setExtension(const string& extension);

	void printName(string& output, string path, FileSystemObject* root) override;
	bool isLeaf() const override;
	void deleteObject() override;
protected:
	string file_content_;
	string extension_;
};

class TextFile : public File {
public:
	TextFile(const string& name, const string& file_content); // automatically sets extension to "txt"

	void printContent() const;
	string getContent() const override;
};

class ExeFile : public File {
public:
	ExeFile(const string& name, const string& file_content); // automatically sets extension to "exe"
	string getContent() const override;
};

#endif