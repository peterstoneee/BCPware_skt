#include <exception>

class PicaException :std::exception
{
public :
	PicaException(const char *str) :std::exception(str)	{}

};

class SettingDialog :std::exception
{
public:
	SettingDialog(const char *str) :std::exception(str)	{}

};