#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include<exception>

using namespace std;

class MissingBackslash : public exception {
public:
	MissingBackslash() : exception("Missing backslash!") {}
	MissingBackslash(const char* s) : exception(s) {}

};

class InvalidName : public exception {
public:
	InvalidName() : exception("Invalid name!") {}
	InvalidName(const char* s) : exception(s) {}

};

class InvalidExtension : public exception {
public:
	InvalidExtension() : exception("Invalid extension!") {}
	InvalidExtension(const char* s) : exception(s) {}

};

class FolderNotFound : public exception {
public:
	FolderNotFound() : exception("Folder not found!") {}
	FolderNotFound(const char* s) : exception(s) {}

};

class AlreadyExists : public exception {
public:
	AlreadyExists() : exception("Object already exists!") {}
	AlreadyExists(const char* s) : exception(s) {}

};


class InvalidCommand : public exception {
public:
	InvalidCommand() : exception("Invalid command") {}
	InvalidCommand(const char* s) : exception(s) {}

};

class FileNotFound : public exception {
public:
	FileNotFound() : exception("File not found") {}
	FileNotFound(const char* s) : exception(s) {}

};
#endif