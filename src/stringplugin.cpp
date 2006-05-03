
//
// this include is necessary
//
#include "dlvhex/PluginInterface.h"


#include <string>


class SplitAtom : public PluginAtom
{
public:

    SplitAtom()
    {
        addInputConstant();
        addInputConstant();
        addInputConstant();
        setOutputArity(1);
    }

    virtual void
    retrieve(const Query& query, Answer& answer) throw (PluginError)
    {

        if (!query.getInputTuple()[0].isString())
            throw PluginError("Wrong input argument type");

        std::string str = query.getInputTuple()[0].getUnquotedString();

        if (!query.getInputTuple()[1].isString())
            throw PluginError("Wrong input argument type");

        std::string sep = query.getInputTuple()[1].getUnquotedString();

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
            out.push_back(Term(components.at(pos)));
        }

        answer.addTuple(out);
    }
};


class CmpAtom : public PluginAtom
{
public:

    CmpAtom()
    {
        addInputConstant();
        addInputConstant();
        setOutputArity(0);
    }

    virtual void
    retrieve(const Query& query, Answer& answer) throw (PluginError)
    {

        std::string in1, in2;

        in1 = query.getInputTuple()[0].getUnquotedString();
        in2 = query.getInputTuple()[1].getUnquotedString();

        Tuple out;

        if (in1 < in2)
            answer.addTuple(out);
    }
};


class ConcatAtom : public PluginAtom
{
public:

    ConcatAtom()
    {
        addInputConstant();
        addInputConstant();
        setOutputArity(1);
    }

    virtual void
    retrieve(const Query& query, Answer& answer) throw (PluginError)
    {

        std::string in1, in2;

        in1 = query.getInputTuple()[0].getUnquotedString();
        in2 = query.getInputTuple()[1].getUnquotedString();

        Tuple out;

        //
        // call Term::Term with second argument true to get a quoted string!
        //
        out.push_back(Term(std::string(in1 + in2), 1));

        answer.addTuple(out);
    }
};


class strstrAtom : public PluginAtom
{
public:

    strstrAtom()
    {
        addInputConstant();
        addInputConstant();
        setOutputArity(0);
    }

    virtual void
    retrieve(const Query& query, Answer& answer) throw (PluginError)
    {

        std::string in1, in2;

        in1 = query.getInputTuple()[0].getUnquotedString();
        in2 = query.getInputTuple()[1].getUnquotedString();

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
    return &theStringPlugin;
}


