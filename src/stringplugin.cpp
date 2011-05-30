
//
// this include is necessary
//
#include "dlvhex/PluginInterface.h"
#include "dlvhex/Term.hpp"
#include "dlvhex/Registry.hpp"

#include <string>
#include <sstream>
#include <cstdio>

namespace dlvhex {
  namespace string {

    class ShaAtom : public PluginAtom
    {
		public:
			ShaAtom() : PluginAtom("", 0)
			{
				//
				// input string
				//
				addInputConstant();
				
				setOutputArity(1);
			}
      
			virtual void
			retrieve(const Query& query, Answer& answer) throw (PluginError)
			{
				Registry &registry = *getRegistry();
				std::string termsymbol = registry.getTermStringByID(query.input[0]);

				if ((termsymbol.at(0) != '"') || (termsymbol.at(termsymbol.length()-1) != '"'))
				{
					throw PluginError("Wrong input argument type");
				}
	
				const std::string& in = termsymbol.substr(1, (termsymbol.length()-2));
	
				FILE *pp;
				char VBUFF[1024];
				
				std::string execstr("echo \"" + in + "\" | sha1sum | cut -d\" \" -f1");
	
				std::stringstream result;
	
				if ((pp = popen(execstr.c_str(), "r")) == NULL)
				{
					throw PluginError("sha1sum system call failed");
				}
	
				if (fgets(VBUFF, 1024, pp) == NULL)
				{
					throw PluginError("Cannot read from sha1sum pipe");
				}
	
				result << VBUFF;
	
				pclose(pp);
	
				std::string res(result.str());
	
				res.erase(res.size() - 1);
	
				Tuple out;
				Term newterm(ID::SUBKIND_TERM_CONSTANT, res);
				out.push_back(registry.storeTerm(newterm));
				answer.get().push_back(out); 
			}
	};
    
    
    class SplitAtom : public PluginAtom
    {
		public:
      
			SplitAtom() : PluginAtom("", 0)
			{
				//
				// string to split
				//
				addInputConstant();
	
				//
				// delimiter (string or int)
				//
				addInputConstant();
	
				//
				// which position to return (int)
				//
				addInputConstant();
	
				setOutputArity(1);
			}
      
			virtual void
			retrieve(const Query& query, Answer& answer) throw (PluginError)
			{        
				Registry &registry = *getRegistry();
				const std::string t0symbol = registry.getTermStringByID(query.input[0]);
				const std::string t1symbol = registry.getTermStringByID(query.input[1]);
				const std::string t2symbol = registry.getTermStringByID(query.input[2]);

				if ((t0symbol.at(0) != '"') || (t0symbol.at(t0symbol.length()-1) != '"'))
				{
					throw PluginError("Wrong input type for argument 0");
				}

				int t1intval, t2intval;
				
				if (t2intval = strtol(t2symbol.c_str(), NULL, 10) != 0)
				{
					throw PluginError("Wrong input type for argument 2");
				}

	
				const std::string& str = t0symbol.substr(1, (t0symbol.length() - 2));
	
				std::stringstream ss;

				if ((t1symbol.at(0) != '"') || (t1symbol.at(t1symbol.length() - 1) != '"'))
				{
					ss << t1symbol.substr(1, (t1symbol.length() - 2));
				}
				else if (t1intval = strtol(t1symbol.c_str(), NULL, 10) != 0)
				{
					ss << t1intval;
				}
				else
				{
					throw PluginError("Wrong input type for argument 1");
				}

				std::string sep(ss.str());
        
				Tuple out;
        
				std::string::size_type start = 0;
				std::string::size_type end = 0;

				unsigned cnt = 0;
				unsigned pos = t2intval;
	
				while ((end = str.find(sep, start)) != std::string::npos)
				{
					// the pos'th match is our output tuple
					if (cnt == pos) 
					{
						Term term(ID::SUBKIND_TERM_CONSTANT, str.substr(start, end - start));
						out.push_back(registry.storeTerm(term));
						break;
					}

					start = end + sep.size();
					++cnt;
				}
	
				// if we couldn't find anything, just return input string
				if (out.empty() && cnt < pos)
				{
					out.push_back(query.input[0]);
				}
				else if (out.empty() && cnt == pos) // add the remainder
				{
					Term term(ID::SUBKIND_TERM_CONSTANT, str.substr(start));
					out.push_back(registry.storeTerm(term));
				}
	
				answer.get().push_back(out);
			}
	};


	class CmpAtom : public PluginAtom
	{
	     public:
      
			CmpAtom() : PluginAtom("", 0)
			{
				//
				// first string or int
				//
				addInputConstant();
	
				//
				// second string or int
				//
				addInputConstant();
	
				setOutputArity(0);
			}
      
			virtual void
			retrieve(const Query& query, Answer& answer) throw (PluginError)
			{	
				Registry &registry = *getRegistry();
				std::stringstream in1, in2;
	
				const Term& s1 = registry.terms.getByID(query.input[0]);
				const Term& s2 = registry.terms.getByID(query.input[1]);
	
				bool smaller = false;
	
				int s1intval, s2intval; 
				
				if ( ((s1intval = strtol(s1.symbol.c_str(), NULL, 10)) != 0) 
				  && ((s2intval = strtol(s2.symbol.c_str(), NULL, 10)) != 0) )
				{
					smaller = (s1intval < s2intval);
				}
				else if ((s1.symbol.at(0) == '"') && (s1.symbol.at(s1.symbol.length() - 1) == '"') 
					  && (s2.symbol.at(0) == '"') && (s2.symbol.at(s2.symbol.length() - 1) == '"') )
				{
					smaller = (s1.symbol.substr(1, s1.symbol.length() - 2) < s2.symbol.substr(1, s2.symbol.length() - 2));
				}
				else
				{
					throw PluginError("Wrong input argument type");
				}

				Tuple out;
	
				if (smaller)
				{
					answer.get().push_back(out);
				}
			}
	};


    class ConcatAtom : public PluginAtom
    {
		public:
      
			ConcatAtom() : PluginAtom("", 0)
			{
				//
				// arbitrary list of strings or ints
				//
				addInputTuple();
        
				setOutputArity(1);
			}
      
			virtual void
			retrieve(const Query& query, Answer& answer) throw (PluginError)
			{
				Registry &registry = *getRegistry(); 
	
				int arity = query.input.size();
	
				std::stringstream concatstream;
	
				for (int t = 0; t < arity; t++)
				{
					const Term &term = registry.terms.getByID(query.input[t]);
					int intval;
					if ((intval = strtol(term.symbol.c_str(), NULL, 10)) != 0)
					{
						concatstream << intval;
					}
					else if ((term.symbol.at(0) == '"') 
							 && (term.symbol.at(term.symbol.length() - 1) == '"'))
					{
						concatstream << term.symbol.substr(1, term.symbol.length() - 2); 
					}
					else
					{
						throw PluginError("Wrong input argument type");
					}
				}
        
				Tuple out;
				
				//
				// call Term::Term with second argument true to get a quoted string!
				//
				Term term(ID::SUBKIND_TERM_CONSTANT, std::string(concatstream.str()));
				out.push_back(registry.storeTerm(term));
	
				answer.get().push_back(out);
			}
	};
    
    
	class strstrAtom : public PluginAtom
    {
		public:
      
			strstrAtom() : PluginAtom("strstr", 0)
			{
				//
				// haystack
				// 
				addInputConstant();
	
				//
				// needle
				//
				addInputConstant();
	
				setOutputArity(0);
			}
      
			virtual void
			retrieve(const Query& query, Answer& answer) throw (PluginError)
			{
				Registry &registry = *getRegistry();

				std::string in1;	
				std::stringstream inss;
	
				const Term& s1 = registry.terms.getByID(query.input[0]);
				const Term& s2 = registry.terms.getByID(query.input[1]);
	
				if ((s1.symbol.at(0) != '"') || (s1.symbol.at(s1.symbol.length() - 1) != '"'))
				{
					throw PluginError("Wrong input argument type");
				}

				in1 = s1.symbol.substr(1, s1.symbol.length() - 2);
				int s2intval;
				
				if ((s2.symbol.at(0) == '"') && (s2.symbol.at(s2.symbol.length() - 1) == '"'))
				{
					inss << s2.symbol.substr(1, s2.symbol.length() - 2);
				}
				else if ((s2intval = strtol(s2.symbol.c_str(), NULL, 10)) != 0)
				{
					inss << s2intval;
				}
				else
				{
					throw PluginError("Wrong input argument type");
				}

				std::string in2(inss.str());
	
				std::transform(in1.begin(), in1.end(), in1.begin(), (int(*)(int))std::tolower);
				std::transform(in2.begin(), in2.end(), in2.begin(), (int(*)(int))std::tolower);
	
				Tuple out;
	
				std::string::size_type pos = in1.find(in2, 0);
	
				if (pos != std::string::npos)
				{
					answer.get().push_back(out);
				}
			}
	};

    
    
	//
	// A plugin must derive from PluginInterface
	//
	class StringPlugin : public PluginInterface
    {
		public:
      
			StringPlugin() 
			{
				setNameVersion(PACKAGE_TARNAME, STRINGPLUGIN_MAJOR, STRINGPLUGIN_MINOR, STRINGPLUGIN_MICRO);
			}
		
			virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx&) const
			{
				std::vector<PluginAtomPtr> ret;
			
				// return smart pointer with deleter (i.e., delete code compiled into this plugin)
				ret.push_back(PluginAtomPtr(new ShaAtom, PluginPtrDeleter<PluginAtom>()));
				ret.push_back(PluginAtomPtr(new SplitAtom, PluginPtrDeleter<PluginAtom>()));
				ret.push_back(PluginAtomPtr(new CmpAtom, PluginPtrDeleter<PluginAtom>()));
				ret.push_back(PluginAtomPtr(new ConcatAtom, PluginPtrDeleter<PluginAtom>()));
				ret.push_back(PluginAtomPtr(new strstrAtom, PluginPtrDeleter<PluginAtom>()));
			
				return ret;
			}
      
			virtual void
			setOptions(bool doHelp, std::vector<std::string>& argv, std::ostream& out)
			{
			}
      
	};
    
    
    //
    // now instantiate the plugin
    //
    StringPlugin theStringPlugin;
    
  } // namespace string
} // namespace dlvhex

//
// and let it be loaded by dlvhex!
//
//extern "C"
//dlvhex::string::StringPlugin*
//PLUGINIMPORTFUNCTION()
//{
//	setNameVersion(PACKAGE_TARNAME, STRINGPLUGIN_MAJOR, STRINGPLUGIN_MINOR, STRINGPLUGIN_MICRO);
//	return &dlvhex::string::theStringPlugin;
//}

// return plain C type s.t. all compilers and linkers will like this code
extern "C"
void * PLUGINIMPORTFUNCTION()
{
	return reinterpret_cast<void*>(& dlvhex::string::theStringPlugin);
}


