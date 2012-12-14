#ifndef CmdLineInterface_h
#define CmdLineInterface_h

#include "ModuleDescriptionXMLParser.h"
#include "CmdLineArgsParser.h"

#include <iostream>
#include <vector>
#include <string>

class CmdLineInterface
{
public:

  CmdLineInterface()
  {
  }

  ~CmdLineInterface()
  {
  }

  void LoadXML(const char* filename);

  const char* GetXMLModuleDescription()
  {
    return _cliParser.GetXMLModuleDescription();
  }

  int GetXMLArgument(int argc, const char* argv[])
  {
    return _cliParser.GetXMLArgument(argc,argv);
  }

  template<typename T>
  T GetArgument(const char* name, int argc, const char* argv[])
  {
    return _cliParser.GetArgument<T>(name,argc,argv);
  }

  template<typename T>
  T QueryArgument(const char* name);

protected:

private: 

  ModuleDescriptionXMLParser _xmlParser;
  CmdLineArgsParser _cliParser;
};

void CmdLineInterface::LoadXML(const char* filename)
{
  _xmlParser.LoadXML(filename);
  _cliParser.LoadXML(filename);
}

template<typename T>
T CmdLineInterface::QueryArgument(const char* name)
{
  std::string res;
  std::cin >> res;

  return _cliParser.GetBareArgument<T>(name,res.c_str());
}

#endif
