#pragma once
class NullFalg {
private:
	char& flag;
public:
	explicit NullFalg(char& f):flag(f){}
};