
#include <iostream>

#include "CmdLineInterface.h"

int main(int argc, const char* argv[])
{
  CmdLineInterface cli;

  // TODO: probably good to embed xml into executable.
  // At that point we can pass it around as a string variable.
  cli.LoadXML("cli_sample.xml");

  if (cli.GetXMLArgument(argc,argv))
  {
    std::cout << cli.GetXMLModuleDescription() << std::endl;
    return EXIT_SUCCESS;
  }

  double doubleVar = cli.GetArgument<double>("doubleVariable",argc,argv);

  std::cout << "PARSED " << doubleVar << std::endl;

  // TODO: the querying of a parameter should be:
  // 1. Forced to query a parameter existing in the XML description
  // 2. Based on a query type that declares the kind of "widget" to use on the other hand (in terms of capabilities and input data)
  // 3. Based on XML for the query message
  // 4. Taking back a value in CLI format (or an XML fragment, but that's a further step)

  double doubleVarQ = cli.QueryArgument<double>("doubleVariable");

  std::cout << "QUERIED " << doubleVarQ << std::endl;

}

