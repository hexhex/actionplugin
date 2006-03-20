
//
// this include is necessary
//
#include "dlvhex/PluginInterface.h"


#include <string>


//
// each atom has to be derived from PluginAtom
//
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
        a["concat"] = new ConcatAtom;
        a["strstr"] = new strstrAtom;
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


