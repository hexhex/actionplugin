/**
 * @file BoolMatrixActionPlugin.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef BOOL_MATRIX_ACTION_PLUGIN_H_
#define BOOL_MATRIX_ACTION_PLUGIN_H_

#include "acthex/ActionPluginInterface.h"

#include "TGAImageMaker.h"

DLVHEX_NAMESPACE_BEGIN

class BoolMatrixActionPlugin: public ActionPluginInterface {

public:

	class Environment: public PluginEnvironment {
	public:
		Environment() {
			name = "EnvironmentOfBoolMatrixActionPlugin";
			
			currentRows = 4;
			currentColumns = 9;
			createMatrix();

			dimensionAlreadySet = false;
			highestRowWithAdd = 0;
			
		}
		virtual ~Environment();
		void createImage(string);
		void print(string, string);
		void add(unsigned int, unsigned int);
		void setDimension(unsigned int, unsigned int);
		void createMatrix();
		void destroyMatrix();
		bool getValue(unsigned int, unsigned int) const;
		unsigned int getCurrentRows() const;
		unsigned int getCurrentColumns() const;
		bool haveToSetDimension() const;
		unsigned getHighestRowWithAdd() const;
	private:
		std::string name;
		bool ** values;
		unsigned int currentRows;
		unsigned int currentColumns;
		bool dimensionAlreadySet;
		unsigned int highestRowWithAdd;
	};

	BoolMatrixActionPlugin() :
			ActionPluginInterface() {
				setNameVersion("libactionboolmatrix", 2, 0, 0);
// 		setNameVersion(PACKAGE_TARNAME, BOOLMATRIXACTIONPLUGIN_VERSION_MAJOR,
// 							BOOLMATRIXACTIONPLUGIN_VERSION_MINOR,
// 							BOOLMATRIXACTIONPLUGIN_VERSION_MICRO);
	}

	class BoolMatrixActionAtomRule30: public PluginActionAtom<BoolMatrixActionPlugin> {
	public:
		BoolMatrixActionAtomRule30();
	private:
		virtual void retrieve(const Environment& environment,
				const Query& query, Answer& answer);
	private:
		bool getState(const Environment& environment, unsigned int row, unsigned int column);
	};
	
	class BoolMatrixActionAtomHaveToSetDimension: public PluginActionAtom<BoolMatrixActionPlugin> {
	public:
		BoolMatrixActionAtomHaveToSetDimension();
	private:
		virtual void retrieve(const Environment& environment,
									 const Query& query, Answer& answer);
	};
	
	class BoolMatrixActionAtomHighestRowWithAdd: public PluginActionAtom<BoolMatrixActionPlugin> {
	public:
		BoolMatrixActionAtomHighestRowWithAdd();
	private:
		virtual void retrieve(const Environment& environment,
									 const Query& query, Answer& answer);
	};

	class BoolMatrixAction: public PluginAction<BoolMatrixActionPlugin> {

	public:

		BoolMatrixAction();

	private:
		virtual void execute(Environment& environment, RegistryPtr registry,
				const Tuple& parms, InterpretationConstPtr interpretationPtr);

	};

	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const;

	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const;

protected:

	ActionPluginInterfacePtr create(ProgramCtx& ctx) {
		std::cerr << "create in BoolMatrixActionPlugin" << std::endl;
#warning a trick, maybe we should find a way remove it
		ctx.getPluginEnvironment<BoolMatrixActionPlugin>();
		std::cerr << "getPluginEnvironment done" << std::endl;
		return boost::shared_ptr < BoolMatrixActionPlugin > (new BoolMatrixActionPlugin());
	}

};

DLVHEX_NAMESPACE_END

#endif /* BOOL_MATRIX_ACTION_PLUGIN_H_ */