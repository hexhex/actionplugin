/**
 * @file BoolMatrixActionPlugin.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "../examples/BoolMatrix_2x/include/BoolMatrixActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

BoolMatrixActionPlugin::Environment::~Environment() {
	destroyMatrix();
}

bool BoolMatrixActionPlugin::Environment::getValue(unsigned int row, unsigned int column) const {
	if(row >= currentRows || column >= currentColumns)
		throw PluginError("Wrong input argument type (in getValue)");
	return values[row][column];
}

unsigned int BoolMatrixActionPlugin::Environment::getCurrentRows() const {
	return currentRows;
}

unsigned int BoolMatrixActionPlugin::Environment::getCurrentColumns() const {
	return currentColumns;
}

void BoolMatrixActionPlugin::Environment::createImage(string path) {
	std::cout << "Creation of Image in " << path << std::endl;
	//declare image
	TGAImageMaker* img = new TGAImageMaker(currentColumns, currentRows);
	
	//declare a temporary color variable
	Colour white;
	Colour black;
	
	white.r = 255;
	white.g = 255;
	white.b = 255;
	white.a = 255;

	black.r = 0;
	black.g = 0;
	black.b = 0;
	black.a = 255;

	for(unsigned int i = 0; i < currentRows; i++)
		for(unsigned int j = 0; j < currentColumns; j++)
			img->setPixel(getValue(i, j) ? black : white, currentRows - i - 1, j);
	
	//write the image to disk
	img->WriteImage(path);
}

void BoolMatrixActionPlugin::Environment::print(string black_char, string white_char) {

	if(black_char.size() != 1 || white_char.size() != 1)
		throw PluginError("Wrong input argument type (in print)");
	
	std::cout << std::endl;
	int c = system("clear");
	std::cout << "Print" << std::endl;
	for(unsigned int i = 0; i < currentRows; i++) {
		for(unsigned int j = 0; j < currentColumns; j++)
			if(values[i][j])
				std::cout << black_char;
			else
				std::cout << white_char;
			std::cout << std::endl;
	}
}

void BoolMatrixActionPlugin::Environment::add(unsigned int row, unsigned int column) {
	values[row][column] = true;
	std::cout << "Added " << row << " " << column << "; ";
	if(row > highestRowWithAdd)
		highestRowWithAdd = row;
}


void BoolMatrixActionPlugin::Environment::setDimension(unsigned int rows, unsigned int columns) {
	if(rows < 3 || columns < 3) {
		std::cout << "Cannot set a dimension under 3 x 3" << std::endl;
		return ;
	}
	if(rows == currentRows || columns == currentColumns) {
		std::cout << "The new dimension is the same as before" << std::endl;
		return ;
	}
	std::cout << "New dimension " << rows << " " << columns << std::endl;
	destroyMatrix();
	currentRows = rows;
	currentColumns = columns;
	createMatrix();
	dimensionAlreadySet = true;
}

void BoolMatrixActionPlugin::Environment::createMatrix() {
	values = new bool * [currentRows];
	for(unsigned int i = 0; i < currentRows; i++)
		values[i] = new bool[currentColumns];
	
	for(unsigned int i = 0; i < currentRows; i++)
		for(unsigned int j = 0; j < currentColumns; j++)
			values[i][j] = false;
}

void BoolMatrixActionPlugin::Environment::destroyMatrix() {
	for(unsigned int i = 0; i < currentRows; i++)
		delete values[i];
	delete values;
}

bool BoolMatrixActionPlugin::Environment::haveToSetDimension() const {
	return !dimensionAlreadySet;
}

unsigned BoolMatrixActionPlugin::Environment::getHighestRowWithAdd() const {
	return highestRowWithAdd;
}

BoolMatrixActionPlugin::BoolMatrixActionAtomRule30::BoolMatrixActionAtomRule30() :
		PluginActionAtom("rule30") {
	addInputConstant();
	addInputConstant();
	setOutputArity(0);
}

void BoolMatrixActionPlugin::BoolMatrixActionAtomRule30::retrieve(
		const Environment& environment, const Query& query, Answer& answer) {

	Registry &registry = *getRegistry();

	int arity = query.input.size();

	if (arity != 2)
		throw PluginError("Wrong input argument type (arity in retrieve)");

	ID id1 = query.input[0];
	ID id2 = query.input[1];

	bool state1 = false;

	if (id1.isIntegerTerm() && id2.isIntegerTerm()) {
		state1 = getState(environment, id1.address, id2.address);
		// state1 = environment.getValue(id1.address, id2.address);
		// std::cout << "\n\n\n *** state1 " << state1 << " *** \n\n\n" << std::endl;
	} else
		throw PluginError("Wrong input argument type (aren't IntegerTerm in retrieve)");

	Tuple out;

	if (state1)
	{
		//std::cerr << "*** state1 is TRUE ***" << std::endl;
		answer.get().push_back(out);
	} //else
		//std::cerr << "*** state1 is FALSE ***" << std::endl;

}

bool BoolMatrixActionPlugin::BoolMatrixActionAtomRule30::getState(const Environment& environment, unsigned int row, unsigned int column) {

	//std::cerr << "row " << row << ", column " << column << "\t\t";

	if(row < 0 || row >= environment.getCurrentRows() || column < 0 || column >= environment.getCurrentColumns()) {
		//std::cout << "\n\n\n *** WRONG VALUE OF row OR column IN haveState1 *** \n\n\n" << std::endl;
		return false;
	}

	if(row == 0) {
		//std::cerr << "getState row 0" << "\t\t";
		return false;
	}

	if(column == 0)
		// 011 or 010 or 001
		if(environment.getValue(row - 1, column) || environment.getValue(row - 1, column + 1)) {
			//std::cerr << "getState 011 or 010 or 001" << "\t\t";
			return true;
		}
		// not 000
		else {
			//std::cerr << "getState not 000" << "\t\t";
			return false;
		}

	if(column == environment.getCurrentColumns() - 1)
		// not 110
		if(environment.getValue(row - 1, column - 1) && environment.getValue(row - 1, column)) {
			//std::cerr << "getState not 110" << "\t\t";
			return false;
		} else
			// 100 or 010
			if(environment.getValue(row - 1, column - 1) || environment.getValue(row - 1, column)) {
				//std::cerr << "getState 100 or 010" << "\t\t";
				return true;
			// not 000
			} else {
				//std::cerr << "getState not 000" << "\t\t";
				return false;
			}

	//100
	if(environment.getValue(row - 1, column - 1) && !environment.getValue(row - 1, column) && !environment.getValue(row - 1, column + 1)) {
		//std::cerr << "getState 100" << "\t\t";
		return true;
	}

	//011
	if(!environment.getValue(row - 1, column - 1) && environment.getValue(row - 1, column) && environment.getValue(row - 1, column + 1)) {
		//std::cerr << "getState 011" << "\t\t";
		return true;
	}

	//010
	if(!environment.getValue(row - 1, column - 1) && environment.getValue(row - 1, column) && !environment.getValue(row - 1, column + 1)) {
		//std::cerr << "getState 010" << "\t\t";
		return true;
	}

	//001
	if(!environment.getValue(row - 1, column - 1) && !environment.getValue(row - 1, column) && environment.getValue(row - 1, column + 1)) {
		//std::cerr << "getState 001" << "\t\t";
		return true;
	}

	//std::cerr << "getState nothing" << "\t\t";

	return false;

}


BoolMatrixActionPlugin::BoolMatrixActionAtomHaveToSetDimension::BoolMatrixActionAtomHaveToSetDimension() :
PluginActionAtom("haveToSetDimension") {
	setOutputArity(0);
}

void BoolMatrixActionPlugin::BoolMatrixActionAtomHaveToSetDimension::retrieve(
		const Environment& environment, const Query& query, Answer& answer) {

	Registry &registry = *getRegistry();

	if (query.input.size() != 0)
		throw PluginError("Wrong input argument type (arity in retrieve)");

	Tuple out;

	if (environment.haveToSetDimension())
	{
		std::cerr << "\n *** dimensionAlreadySet is FALSE *** \n" << std::endl;
		answer.get().push_back(out);
	} else
		std::cerr << "\n *** dimensionAlreadySet is TRUE *** \n" << std::endl;

}


BoolMatrixActionPlugin::BoolMatrixActionAtomHighestRowWithAdd::BoolMatrixActionAtomHighestRowWithAdd() :
PluginActionAtom("highestRowWithAdd") {
	setOutputArity(0);
}

void BoolMatrixActionPlugin::BoolMatrixActionAtomHighestRowWithAdd::retrieve(
		const Environment& environment, const Query& query, Answer& answer) {

	Registry &registry = *getRegistry();

	if (query.input.size() != 0)
		throw PluginError("Wrong input argument type (arity in retrieve)");

	std::stringstream concatstream;
	
	//concatstream << '"';
	concatstream << environment.getHighestRowWithAdd();
	//concatstream << '"';
			
	Tuple out;

	//
	// call Term::Term with second argument true to get a quoted string!
	//
	Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, std::string(concatstream.str()));
	out.push_back(registry.storeTerm(term));
	answer.get().push_back(out);

}


BoolMatrixActionPlugin::BoolMatrixAction::BoolMatrixAction() :
		PluginAction("boolMatrix") {
			
}

void BoolMatrixActionPlugin::BoolMatrixAction::execute(Environment& environment,
		RegistryPtr pregistry, const Tuple& parms,
		const InterpretationConstPtr interpretationPtr) {

  Registry& registry = *pregistry;

  
  if(registry.getTermStringByID(parms[0]) == "print") {
// 	  environment.print(registry.getTermStringByID(parms[1]), registry.getTermStringByID(parms[2]));

		const Term& black_term = registry.terms.getByID(parms[1]);
		const Term& white_term = registry.terms.getByID(parms[2]);

		if (!black_term.isQuotedString() || !white_term.isQuotedString())
			throw PluginError("Wrong input argument type (terms of print aren't QuotedString)");

	  environment.print(black_term.getUnquotedString(), white_term.getUnquotedString());
  } else
	  if(registry.getTermStringByID(parms[0]) == "add") {
		  environment.add(parms[1].address, parms[2].address);
		  // environment.print();
	  } else
		  if(registry.getTermStringByID(parms[0]) == "setDimension") {
			  environment.setDimension(parms[1].address, parms[2].address);
			  //print();
		  } else
			  if(registry.getTermStringByID(parms[0]) == "draw") {
				  environment.createImage(registry.getTermStringByID(parms[1]) + ".tga");
			 }

}

std::vector<PluginAtomPtr> BoolMatrixActionPlugin::createAtoms(
		ProgramCtx& ctx) const {
	std::vector < PluginAtomPtr > ret;
	ret.push_back(
		PluginAtomPtr(new BoolMatrixActionAtomRule30, PluginPtrDeleter<PluginAtom>()));
	ret.push_back(
		PluginAtomPtr(new BoolMatrixActionAtomHaveToSetDimension, PluginPtrDeleter<PluginAtom>()));
	ret.push_back(
		PluginAtomPtr(new BoolMatrixActionAtomHighestRowWithAdd, PluginPtrDeleter<PluginAtom>()));
	return ret;
}

std::vector<PluginActionBasePtr> BoolMatrixActionPlugin::createActions(
		ProgramCtx& ctx) const {
	std::vector < PluginActionBasePtr > ret;
	ret.push_back(
			PluginActionBasePtr(new BoolMatrixAction,
					PluginPtrDeleter<PluginActionBase>()));
	return ret;
}

//
// now instantiate the plugin
//
BoolMatrixActionPlugin theBoolMatrixActionPlugin;

DLVHEX_NAMESPACE_END

//
// let it be loaded by dlvhex!
//
IMPLEMENT_PLUGINABIVERSIONFUNCTION

// return plain C type s.t. all compilers and linkers will like this code
extern "C" void * PLUGINIMPORTFUNCTION() {
return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theBoolMatrixActionPlugin);
}
