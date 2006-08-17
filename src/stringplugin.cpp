
//
// this include is necessary
//
#include "dlvhex/PluginInterface.h"


#include <string>
#include <sstream>


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
            throw PluginError("Wrong input argument type");

        std::string in = query.getInputTuple()[0].getUnquotedString();

        FILE *pp;
        char VBUFF[1024];

        std::string execstr("echo \"" + in + "\" | sha1sum | cut -d\" \" -f1");

        std::stringstream result;

        if ((pp = popen(execstr.c_str(), "r")) == NULL)
            throw PluginError("sha1sum system call failed");

        fgets(VBUFF, 1024, pp);

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
        if (!query.getInputTuple()[0].isString())
            throw PluginError("Wrong input argument type");

        std::string str = query.getInputTuple()[0].getUnquotedString();

        std::stringstream ss;
        
        if (query.getInputTuple()[1].isInt())
            ss << query.getInputTuple()[1].getInt();
        else if (query.getInputTuple()[1].isString())
            ss << query.getInputTuple()[1].getUnquotedString();
        else
            throw PluginError("Wrong input argument type");

        std::string sep(ss.str());

        if (!query.getInputTuple()[2].isInt())
            throw PluginError("Wrong input argument type");
        
        unsigned pos = query.getInputTuple()[2].getInt();

        std::vector<std::string> components;
        
        std::string::size_type start = 0;
        std::string::size_type end = 0;

        while ((end = str.find(sep, start)) != std::string::npos)
        {
            components.push_back(str.substr(start, end - start));
            start = end + sep.size();
        }

        components.push_back(str.substr(start));

        Tuple out;

        if (pos < components.size())
        {
            out.push_back(Term(components.at(pos), 1));
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
            smaller = (s1.getInt() < s2.getInt());
        else if (s1.isString() && s2.isString())
            smaller = (s1.getUnquotedString() < s2.getUnquotedString());
        else
            throw PluginError("Wrong input argument type");

        Tuple out;

        if (smaller)
            answer.addTuple(out);
    }
};


class ConcatAtom : public PluginAtom
{
public:

    ConcatAtom()
    {
        //
        // first string or int
        //
        addInputConstant();

        //
        // second string or int
        //
        addInputConstant();
        
        setOutputArity(1);
    }

    virtual void
    retrieve(const Query& query, Answer& answer) throw (PluginError)
    {

        std::stringstream in1, in2;

        Term s1 = query.getInputTuple()[0];
        Term s2 = query.getInputTuple()[1];

        bool smaller = false;

        if (s1.isInt())
            in1 << s1.getInt();
        else if (s1.isString())
            in1 << s1.getUnquotedString();
        else
            throw PluginError("Wrong input argument type");
        
        if (s2.isInt())
            in2 << s2.getInt();
        else if (s2.isString())
            in2 << s2.getUnquotedString();
        else
            throw PluginError("Wrong input argument type");
        
        Tuple out;

        //
        // call Term::Term with second argument true to get a quoted string!
        //
        out.push_back(Term(std::string(in1.str() + in2.str()), 1));

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
            throw PluginError("Wrong input argument type");

        in1 = s1.getUnquotedString();

        if (s2.isString())
            inss << s2.getUnquotedString();
        else if (s2.isInt())
            inss << s2;
        else
            throw PluginError("Wrong input argument type");

        std::string in2(inss.str());

        std::transform(in1.begin(), in1.end(), in1.begin(), (int(*)(int))std::tolower);
        std::transform(in2.begin(), in2.end(), in2.begin(), (int(*)(int))std::tolower);

        Tuple out;

        std::string::size_type pos = in1.find(in2, 0);

        if (pos != std::string::npos)
            answer.addTuple(out);
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
        a["sha1sum"] = new ShaAtom;
        a["split"] = new SplitAtom;
        a["cmp"] = new CmpAtom;
        a["concat"] = new ConcatAtom;
        a["strstr"] = new strstrAtom;
    }

    virtual void
    setOptions(std::vector<std::string>& options)
    {
    }

};


//
// now instantiate the plugin
//
StringPlugin theStringPlugin;

//
// and let it be loaded by dlvhex!
//
extern "C"
StringPlugin*
PLUGINIMPORTFUNCTION()
{
    theStringPlugin.setVersion(1,0,0);
    return &theStringPlugin;
}


