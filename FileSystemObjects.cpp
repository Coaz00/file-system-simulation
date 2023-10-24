#include "FileSystemObjects.h"

FileSystemObject::FileSystemObject(const string& name) : name_(name), parent_(nullptr)
{
}

string FileSystemObject::getName() const
{
	return name_;
}

void FileSystemObject::setName(const string& name)
{
	name_ = name;
}

void FileSystemObject::setParent(FileSystemObject* parent)
{
	parent_ = parent;
}

void FileSystemObject::printName(string& output, string path, FileSystemObject* root)
{
	output += name_;
}

FileSystemObject* FileSystemObject::getParent() const
{
	return parent_;
}

list<FileSystemObject*>& FileSystemObject::getFolderContent()
{
	list<FileSystemObject*> ls;
	return ls;
}

string FileSystemObject::getContent() const
{
	return string();
}

void FileSystemObject::addToTree(FileSystemObject* object, FileSystemObject* parent)
{
}

Folder::Folder(const string& name) : FileSystemObject(name)
{
}

Folder::~Folder()
{
	list<FileSystemObject*>::iterator it;
	if (!folder_content_.empty()) {
		for (it = folder_content_.begin(); it != folder_content_.end(); it++) (*it)->deleteObject();
	}
	folder_content_.clear();
}

void Folder::addToTree(FileSystemObject* object, FileSystemObject* parent)
{
	list<FileSystemObject*>::iterator it;
	if (folder_content_.empty()) {
		folder_content_.push_front(object); // if list is empty we add object 
		object->setParent(parent);
		return;
	}

	for (it = folder_content_.begin(); it != folder_content_.end(); it++) {
		if (object->getName() == (*it)->getName()) throw AlreadyExists();
		if (compare(object, *it)) break; // we are breaking if we get to element which is alphabetically later than our object
	}

	folder_content_.insert(it, object); // we add object to list in correct spot

	object->setParent(parent);

}


char Folder::getType() const
{
	return 'F';
}

list<FileSystemObject*>& Folder::getFolderContent() 
{
	return folder_content_;
}

bool Folder::isLeaf() const
{
	if (folder_content_.empty()) return true; //folder is a leaf only if it has no children
	return false;
}

void Folder::printName(string& output, string path, FileSystemObject* root)
{
	// we don't print root folder's name
	if (this != root) {
		output += "\\";
		output += name_;
		output += "\n";
	}

	// we don't print root folder in the path
	if (this != root) {
		path += "\\";
		path += name_;
	}

	for (auto i : folder_content_) {
		output += path;
		i->printName(output, path, root);
	}
}

void Folder::deleteObject()
{
	list<FileSystemObject*>::iterator it;

	if (!folder_content_.empty()) {
		for (it = folder_content_.begin(); it != folder_content_.end(); it++) (*it)->deleteObject();
	}
	folder_content_.clear();

	delete this;
}

bool FileSystemObject::compare(const FileSystemObject* first, const FileSystemObject* second) const
{
		int i = 0;
		while ((i < first->getName().length()) && (i < second->getName().length()))
		{
			if (tolower(first->getName()[i]) < tolower(second->getName()[i])) return true;
			else if (tolower(first->getName()[i]) > tolower(second->getName()[i])) return false;
			i++;
		}
		return (first->getName().length() < second->getName().length());
}

File::File(const string& name, const string& file_content, const string& extension) : FileSystemObject(name), file_content_(file_content), extension_(extension)
{
}

string File::getFileContent() const
{
	return file_content_;
}

string File::getExtension() const
{
	return extension_;
}

void File::setFileContent(const string& file_content)
{
	file_content_ = file_content;
}

void File::setExtension(const string& extension)
{
	extension_ = extension;
}

string File::getName() const
{
	//returns name.extension
	string name;
	name += name_;
	name += ".";
	name += extension_;
	return name;
}

void File::printName(string& output, string path, FileSystemObject* root)
{
	//prints name in form name.extension
	output += "\\";
	output += name_;
	output += ".";
	output += extension_;
	output += "\n";
}

char File::getType() const
{
	return 'f';
}

bool File::isLeaf() const
{
	return true; // File must be a leaf
}

void File::deleteObject()
{
	delete this;
}

TextFile::TextFile(const string& name, const string& file_content) : File(name, file_content, "txt")
{
}

void TextFile::printContent() const
{
	cout << file_content_;
}

string TextFile::getContent() const
{
	return file_content_;
}

ExeFile::ExeFile(const string& name, const string& file_content) : File(name, file_content, "exe")
{
}

string ExeFile::getContent() const
{
	return file_content_;
}
