
//
// this include is necessary
//
#include "dlvhex/PluginInterface.h"


#include <string>
#include <sstream>
#include <cstdio>

namespace dlvhex {
  namespace string {

    class ShaAtom : public PluginAtom
    {
    public:
      
      ShaAtom()
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
	if (!query.getInputTuple()[0].isString())
	  {
	    throw PluginError("Wrong input argument type");
	  }
	
	const std::string& in = query.getInputTuple()[0].getUnquotedString();
	
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
	
	out.push_back(Term(res, 1));
	
	answer.addTuple(out);
      }
    };
    
    
    class SplitAtom : public PluginAtom
    {
    public:
      
      SplitAtom()
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
	const Term& t0 = query.getInputTuple()[0];
	const Term& t1 = query.getInputTuple()[1];
	const Term& t2 = query.getInputTuple()[2];
	
	if (!t0.isString())
	  {
	    throw PluginError("Wrong input type for argument 0");
	  }

        if (!t2.isInt())
	  {
	    throw PluginError("Wrong input type for argument 2");
	  }

	
        const std::string& str = t0.getUnquotedString();
	
        std::stringstream ss;

        if (t1.isString())
	  {
	    ss << t1.getUnquotedString();
	  }
        else if (t1.isInt())
	  {
	    ss << t1.getInt();
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
        unsigned pos = t2.getInt();
	
        while ((end = str.find(sep, start)) != std::string::npos)
	  {
	    // the pos'th match is our output tuple
	    if (cnt == pos) 
	      {
		out.push_back(Term(str.substr(start, end - start), true));
		break;
	      }

	    start = end + sep.size();
	    ++cnt;
	  }
	
	// if we couldn't find anything, just return input string
        if (out.empty() && cnt < pos)
	  {
	    out.push_back(t0);
	  }
	else if (out.empty() && cnt == pos) // add the remainder
	  {
	    out.push_back(Term(str.substr(start), true));
	  }
	
        answer.addTuple(out);
      }
    };


    class CmpAtom : public PluginAtom
    {
    public:
      
      CmpAtom()
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
	
        std::stringstream in1, in2;
	
        Term s1 = query.getInputTuple()[0];
        Term s2 = query.getInputTuple()[1];
	
        bool smaller = false;
	
        if (s1.isInt() && s2.isInt())
	  {
	    smaller = (s1.getInt() < s2.getInt());
	  }
        else if (s1.isString() && s2.isString())
	  {
            smaller = (s1.getUnquotedString() < s2.getUnquotedString());
	  }
        else
	  {
            throw PluginError("Wrong input argument type");
	  }

        Tuple out;
	
        if (smaller)
	  {
            answer.addTuple(out);
	  }
      }
    };


    class ConcatAtom : public PluginAtom
    {
    public:
      
      ConcatAtom()
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
	Tuple parms = query.getInputTuple();
	
	Tuple::size_type arity = parms.size();
	
        std::stringstream concatstream;
	
	for (Tuple::size_type t = 0; t < arity; t++)
	  {
	    if (parms[t].isInt())
	      {
		concatstream << parms[t].getInt();
	      }
	    else if (parms[t].isString())
	      {
		concatstream << parms[t].getUnquotedString(); 
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
        out.push_back(Term(std::string(concatstream.str()), 1));
	
        answer.addTuple(out);
      }
    };
    
    
    class strstrAtom : public PluginAtom
    {
    public:
      
      strstrAtom()
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
        std::string in1;
	
        std::stringstream inss;
	
        Term s1 = query.getInputTuple()[0];
        Term s2 = query.getInputTuple()[1];
	
        if (!s1.isString())
	  {
            throw PluginError("Wrong input argument type");
	  }

        in1 = s1.getUnquotedString();
	
        if (s2.isString())
	  {
            inss << s2.getUnquotedString();
	  }
        else if (s2.isInt())
	  {
            inss << s2;
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
            answer.addTuple(out);
	  }
      }
    };

    
    
    //
    // A plugin must derive from PluginInterface
    //
    class StringPlugin : public PluginInterface
    {
    public:
      
      //
      // register all atoms of this plugin:
      //
      virtual void
      getAtoms(AtomFunctionMap& a)
      {
	boost::shared_ptr<PluginAtom> shaatom(new ShaAtom);
	boost::shared_ptr<PluginAtom> splitatom(new SplitAtom);
	boost::shared_ptr<PluginAtom> cmpatom(new CmpAtom);
	boost::shared_ptr<PluginAtom> concatatom(new ConcatAtom);
	boost::shared_ptr<PluginAtom> strstratom(new strstrAtom);
	
        a["sha1sum"] = shaatom;
        a["split"] = splitatom;
        a["cmp"] = cmpatom;
        a["concat"] = concatatom;
        a["strstr"] = strstratom;
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
extern "C"
dlvhex::string::StringPlugin*
PLUGINIMPORTFUNCTION()
{
  dlvhex::string::theStringPlugin.setPluginName(PACKAGE_TARNAME);
  dlvhex::string::theStringPlugin.setVersion(STRINGPLUGIN_MAJOR,
					     STRINGPLUGIN_MINOR,
					     STRINGPLUGIN_MICRO);
  return &dlvhex::string::theStringPlugin;
}


