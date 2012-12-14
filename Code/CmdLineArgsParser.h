#ifndef CmdLineArgsParser_h
#define CmdLineArgsParser_h

#include "ModuleDescriptionXMLParser.h"
#include "tclap/CmdLine.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>

class CmdLineArgsParser
{
public:

  CmdLineArgsParser()
  {
  }

  ~CmdLineArgsParser()
  {
  }

  void LoadXML(const char* filename);

  template<typename T>
  T GetArgument(const char* name, int argc, const char* argv[]);

  template<typename T>
  T GetBareArgument(const char* name, const char* arg);

  template<typename T>
  T GetBareArgument(const char* name, int argc, const char* argv[]);

  int GetXMLArgument(int argc, const char* argv[]);

  const char *GetXMLModuleDescription(); 

protected:

  // TODO constness
  template<typename TArg>
  void ParseArg(TArg arg, int argc, const char* argv[]);

  void splitString (const std::string &text,
             const std::string &separators,
             std::vector<std::string> &words);

  void splitFilenames (const std::string &text,
                std::vector<std::string> &words);

private: 

  ModuleDescriptionXMLParser _xmlParser;

};

void CmdLineArgsParser::LoadXML(const char* filename)
{
  _xmlParser.LoadXML(filename);
}

template<typename T>
T CmdLineArgsParser::GetArgument(const char* name, int argc, const char* argv[])
{
  _xmlParser.VisitParameterByName(name);
  const char* flag = _xmlParser.GetParameterTag("flag");
  const char* longflag = _xmlParser.GetParameterTag("longflag");
  int index = _xmlParser.GetParameterTagAsInt("index");
  const char* channel = _xmlParser.GetParameterTag("channel");
  bool multiple = _xmlParser.GetParameterAttributeAsBool("multiple");
  // TODO: check what tags and arguments are needed
  const char* desc = _xmlParser.GetParameterTag("description");
  bool required = _xmlParser.GetParameterTagAsBool("required");
  const char* typeDescription = _xmlParser.GetParameterTypeDescription();
  bool boolean = strcmp("bool",typeDescription) == 0 ? true : false;
  T defaultValue = _xmlParser.GetParameterTagT<T>("default");

  // TODO: TCLAP::ValuesConstraint
  TCLAP::UnlabeledValueArg<T>* arg1 = NULL;
  TCLAP::UnlabeledMultiArg<T>* arg2 = NULL;
  TCLAP::SwitchArg* arg3 = NULL;
  TCLAP::ValueArg<T>* arg4 = NULL;
  TCLAP::MultiArg<T>* arg5 = NULL;
  bool switchValue;
  try 
  {
    T value;
    void* void_value;
    if (!flag && !longflag && !multiple)
    {
      arg1 = new TCLAP::UnlabeledValueArg<T>(name, desc, required, defaultValue, typeDescription); 
      ParseArg(arg1,argc,argv);
      void_value = (void*)&(arg1->getValue());
    }
    else if (!flag && !longflag && multiple)
    {
      // TODO: the return value needs to be unpacked
      arg2 = new TCLAP::UnlabeledMultiArg<T>(name, desc, required, typeDescription); 
      ParseArg(arg2,argc,argv);
      void_value = (void*)&(arg2->getValue());
    }
    else if (boolean)
    {
      arg3 = new TCLAP::SwitchArg(flag, name, desc); 
      ParseArg(arg3,argc,argv);
      switchValue = arg3->getValue();
      void_value = (void*)&switchValue;
    }
    else if (!multiple)
    {
      arg4 = new TCLAP::ValueArg<T>(flag, name, desc, required, defaultValue, typeDescription); 
      ParseArg(arg4,argc,argv);
      void_value = (void*)&(arg4->getValue());
    }
    else
    {
      // TODO: the return value needs to be unpacked
      arg5 = new TCLAP::MultiArg<T>(flag, name, desc, required, typeDescription); 
      ParseArg(arg5,argc,argv);
      void_value = (void*)&(arg5->getValue());
    }

    value = *(T*)void_value;

    if (arg1) delete arg1;
    if (arg2) delete arg2;
    if (arg3) delete arg3;
    if (arg4) delete arg4;
    if (arg5) delete arg5;

    return value;
  } 

  catch (TCLAP::ArgException &e)  // catch any exceptions
  { 
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    return 0; 
  }
}

template<typename T>
T CmdLineArgsParser::GetBareArgument(const char* name, int argc, const char* argv[])
{
  _xmlParser.VisitParameterByName(name);
  const char* flag = _xmlParser.GetParameterTag("flag");
  const char** argv2 = new const char* [argc + 2];
  argv2[0] = "";
  std::string full_flag;
  full_flag.append("-");
  full_flag.append(flag);
  argv2[1] = full_flag.c_str();
  for (int i=0; i<argc; i++)
  {
    argv2[2+i] = argv[i];
  }
  // TODO: take a look at how ParseArg should be improved 
  // to avoid having to build fake argc argv.
  return this->GetArgument<T>(name,argc+2,argv2);
}

template<typename T>
T CmdLineArgsParser::GetBareArgument(const char* name, const char* arg)
{
  std::string sarg = arg;
  std::vector<std::string> words;
  this->splitString(arg," ",words);
  int argc = words.size();
  const char** argv = new const char* [argc];
  for (int i=0; i<argc; i++)
  {
    argv[i] = words[i].c_str();
  }
  return this->GetBareArgument<T>(name,argc,argv);
}

void CmdLineArgsParser::splitString (const std::string &text,
             const std::string &separators,
             std::vector<std::string> &words)
{
  const std::string::size_type n = text.length();
  std::string::size_type start = text.find_first_not_of(separators);
  while (start < n)
  {
    std::string::size_type stop = text.find_first_of(separators, start);
    if (stop > n) stop = n;
    words.push_back(text.substr(start, stop - start));
    start = text.find_first_not_of(separators, stop+1);
  }
}

void CmdLineArgsParser::splitFilenames (const std::string &text,
                std::vector<std::string> &words)
{
  const std::string::size_type n = text.length();
  bool quoted;
  std::string comma(",");
  std::string quote("\"");
  std::string::size_type start = text.find_first_not_of(comma);
  while (start < n)
  {
    quoted = false;
    std::string::size_type startq = text.find_first_of(quote, start);
    std::string::size_type stopq = text.find_first_of(quote, startq+1);
    std::string::size_type stop = text.find_first_of(comma, start);
    if (stop > n) stop = n;
    if (startq != std::string::npos && stopq != std::string::npos)
    {
      while (startq < stop && stop < stopq && stop != n)
      {
        quoted = true;
        stop = text.find_first_of(comma, stop+1);
        if (stop > n) stop = n;
      }
    }
    if (!quoted)
    {
      words.push_back(text.substr(start, stop - start));
    }
    else
    {
      words.push_back(text.substr(start+1, stop - start-2));
    }
    start = text.find_first_not_of(comma, stop+1);
  }
}

int CmdLineArgsParser::GetXMLArgument(int argc, const char* argv[])
{
  if (argc >= 2 && (strcmp(argv[1],"--xml") == 0))
  {
    return 1;
  }

  return 0;
}

const char* CmdLineArgsParser::GetXMLModuleDescription()
{
  return _xmlParser.CStr();
}

template<typename TArg>
void CmdLineArgsParser::ParseArg(TArg arg, int argc, const char* argv[])
{
  //TODO: these should not be realized until somebody asks for them.
  // So probably there's a need to split this function into pieces
  // and defer the parsing as much as possible.
  //GEN
  //bool UseTCLAP = false; 
  //std::vector<std::string> logoFilesTemp; 
  //std::vector<std::string> logoFiles; 
  //std::string InputXML; 
  //std::string OutputCxx; 
  //NEG

  //bool echoSwitch = false; 
  //bool xmlSwitch = false; 
  //std::string processInformationAddressString = "0"; 
  //std::string returnParameterFile; 

  std::string fullDescription("Description: "); 
  fullDescription += _xmlParser.GetModuleInfo("description"); 
  fullDescription += "\nAuthor(s): ";
  fullDescription += _xmlParser.GetModuleInfo("contributor"); 
  fullDescription += "\nAcknowledgements: ";
  fullDescription += _xmlParser.GetModuleInfo("acknowledgements"); 

  TCLAP::CmdLine commandLine (fullDescription, ' ', "1.0" ); 
 
  std::ostringstream msg; 

  // TODO: TCLAP has templated parsed args. We have to have template specialization to handle that.
  // This happens at query time.

  //GEN
  //msg.str("");
  //msg << "Generate TCLAP Code (default: " << UseTCLAP << ")"; 
  //TCLAP::SwitchArg UseTCLAPArg("", "TCLAP", msg.str(), commandLine, UseTCLAP); 
  //NEG
 
  //GEN
  //msg.str("");
  //msg << "Logo files";
  //TCLAP::MultiArg<std::string > logoFilesArg("", "logoFiles", msg.str(), 0, "std::vector<std::string>", commandLine); 
  //NEG

  //GEN 
  //msg.str("");
  //msg << "XML description of interface";
  //TCLAP::ValueArg<std::string > InputXMLArg("", "InputXML", msg.str(), 0, InputXML, "std::string", commandLine); 
  //NEG
 
  //GEN
  //msg.str("");
  //msg << "C++ Code to process command line arguments";
  //TCLAP::ValueArg<std::string > OutputCxxArg("", "OutputCxx", msg.str(), 0, OutputCxx, "std::string", commandLine); 
  //NEG
 
  //msg.str("");
  //msg << "Echo the command line arguments (default: " << echoSwitch << ")"; 
  //TCLAP::SwitchArg echoSwitchArg("", "echo", msg.str(), commandLine, echoSwitch); 
 
  //msg.str("");
  //msg << "Produce xml description of command line arguments (default: " << xmlSwitch << ")"; 
  //TCLAP::SwitchArg xmlSwitchArg("", "xml", msg.str(), commandLine, xmlSwitch); 
 
  //msg.str("");
  //msg << "Address of a structure to store process information (progress, abort, etc.). (default: " << processInformationAddressString << ")"; 
  //TCLAP::ValueArg<std::string > processInformationAddressStringArg("", "processinformationaddress", msg.str(), 0, processInformationAddressString, "std::string", commandLine); 
 
  //msg.str("");
  //msg << "Filename in which to write simple return parameters (int, float, int-vector, etc.) as opposed to bulk return parameters (image, geometry, transform, measurement, table).";
  //TCLAP::ValueArg<std::string > returnParameterFileArg("", "returnparameterfile", msg.str(), 0, returnParameterFile, "std::string", commandLine); 

  commandLine.add(arg);
 
  try 
  { 
    /* Build a map of flag aliases to the true flag */ 
    std::map<std::string,std::string> flagAliasMap; 
    std::map<std::string,std::string> longFlagAliasMap; 
    /* Remap flag aliases to the true flag */ 
    std::vector<std::string> targs; 
    std::map<std::string,std::string>::iterator ait; 
    std::map<std::string,std::string>::iterator dait; 
    size_t ac; 
    for (ac=0; ac < static_cast<size_t>(argc); ++ac)  
    {  
      if (strlen(argv[ac]) == 2 && argv[ac][0]=='-') 
      { 
        /* short flag case */ 
        std::string tflag(argv[ac], 1, strlen(argv[ac])-1); 
        ait = flagAliasMap.find(tflag); 
        if (ait != flagAliasMap.end()) 
        { 
          if (ait != flagAliasMap.end()) 
          { 
            /* remap the flag */ 
            targs.push_back("-" + (*ait).second); 
          } 
        } 
        else 
        { 
          targs.push_back(argv[ac]); 
        } 
      } 
      else if (strlen(argv[ac]) > 2 && argv[ac][0]=='-' && argv[ac][1]=='-') 
      { 
        /* long flag case */ 
        std::string tflag(argv[ac], 2, strlen(argv[ac])-2); 
        ait = longFlagAliasMap.find(tflag); 
        if (ait != longFlagAliasMap.end()) 
        { 
          if (ait != longFlagAliasMap.end()) 
          { 
            /* remap the flag */ 
            targs.push_back("--" + (*ait).second); 
          } 
        } 
        else 
        { 
          targs.push_back(argv[ac]); 
        } 
      } 
      else if (strlen(argv[ac]) > 2 && argv[ac][0]=='-' && argv[ac][1]!='-') 
      { 
        /* short flag case where multiple flags are given at once ala */ 
        /* "ls -ltr" */ 
        std::string tflag(argv[ac], 1, strlen(argv[ac])-1); 
        std::string rflag("-"); 
        for (std::string::size_type fi=0; fi < tflag.size(); ++fi) 
        { 
          std::string tf(tflag, fi, 1); 
          ait = flagAliasMap.find(tf); 
          if (ait != flagAliasMap.end()) 
          { 
            if (ait != flagAliasMap.end()) 
            { 
              /* remap the flag */ 
              rflag += (*ait).second; 
            } 
          } 
          else 
          { 
            rflag += tf; 
          } 
        } 
        targs.push_back(rflag); 
      } 
      else 
      { 
        /* skip the argument without remapping (this is the case for any */ 
        /* arguments for flags */ 
        targs.push_back(argv[ac]); 
      } 
    } 
 
    /* Remap args to a structure that CmdLine::parse() can understand*/ 
    std::vector<char*> vargs; 
    for (ac = 0; ac < targs.size(); ++ac) 
    {  
      vargs.push_back(const_cast<char *>(targs[ac].c_str())); 
    } 
    commandLine.parse ( vargs.size(), (char**) &(vargs[0]) ); 

    // TODO: this happens at query time
    //GEN
    //UseTCLAP = UseTCLAPArg.getValue(); 
    //logoFilesTemp = logoFilesArg.getValue(); 
    //InputXML = InputXMLArg.getValue(); 
    //OutputCxx = OutputCxxArg.getValue(); 
    //NEG
    //echoSwitch = echoSwitchArg.getValue(); 
    //xmlSwitch = xmlSwitchArg.getValue(); 
    //processInformationAddressString = processInformationAddressStringArg.getValue(); 
    //returnParameterFile = returnParameterFileArg.getValue(); 
    // TODO: unpack logoFiles
    //{ /* Assignment for logoFiles */ 
    //  for (unsigned int _i = 0; _i < logoFilesTemp.size(); _i++) 
    //    { 
    //    std::vector<std::string> words; 
    //    std::vector<std::string> elements; 
    //    words.clear(); 
    //    splitFilenames(logoFilesTemp[_i], words); 
    //    for (unsigned int _j= 0; _j < words.size(); _j++) 
    //      { 
    //        logoFiles.push_back((words[_j].c_str())); 
    //      } 
    //    } 
    //} 
  } 
  catch ( TCLAP::ArgException e ) 
  { 
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    //return (void*)arg.getValue(); 
  }

  //return (void*)arg.getValue();
}

#endif
