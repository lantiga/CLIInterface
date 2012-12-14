#CLI Interface

CLI Interface is a prototype for a next generation command line module interface for 3DSlicer, CTK and CTK-based projects.

It tries to overcome a few limitations of the current status quo in a couple of ways:

* it provides a replacement for GenerateCLP, allowing easy addition of new tags and dynamic querying of parameters (as opposed to the current, monolythic C++ macro-based solution)
* it allows for parameters to be either parsed from the command line arguments as well as queried through standard i/o. 

The main motivation is to allow command line modules to query some of the parameters during their execution by emitting a query XML string in standard output (similarly to what happens now for progress reporting) and have the input get back on standard input (e.g. by a user on the command line, by a user through a 3DSlicer widget or by another process on its standard output), parsed and assigned to a variable during execution.

This would allow CLI modules to be _mildly_ interactive (e.g. specify a threshold on the base of an incremental result, or select a point on an intermediate image), greatly expanding the reach of CLI module use cases and maintaining interoperability of CLI modules among CTK-compliant applications.

The code uses tclap for parsing command line arguments and TinyXML2 for parsing XML.

You can find a sample of what a command line module might look like in the future under Testing.

This project started as experiment with Steve Pieper at the [Bologna 2012 CTK Hackfest](http://www.commontk.org/index.php/CTK-Hackfest-Dec-2012).

## TODO

__This is work in progress.__

There are several parts that still need to be implemented, some of which have to be ported from GenerateCLP:

* Unpacking complex arguments
* Generating query XML fragments
* Embedding the XML within the executable
* Populating ModuleDescriptionParser data structures
* Adding process information, etc needed by callees
