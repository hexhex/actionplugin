
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

        out.push_back(std::string(in1 + in2));

        std::cout << "tuple: " << out << std::endl;

        //
        // fill the answer object...
        //

        answer.addTuple(out);
    }


private:

    //
    // some other user-defined member functions...
    //
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
        //
        // these strings will be the external atoms' names in the program
        //
        a["concat"] = new ConcatAtom;

        //
        // repeat this for each atom you defined!
        //
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


