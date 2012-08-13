/**
 * @file RobotActionPlugin.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ROBOT_ACTION_PLUGIN_H_
#define ROBOT_ACTION_PLUGIN_H_

#include "acthex/ActionPluginInterface.h"

#include <stdlib.h>
#include <time.h>

DLVHEX_NAMESPACE_BEGIN

class RobotActionPlugin: public ActionPluginInterface {

public:

	RobotActionPlugin() :
			ActionPluginInterface() {
		setNameVersion(PACKAGE_TARNAME, ROBOTACTIONPLUGIN_VERSION_MAJOR,
				ROBOTACTIONPLUGIN_VERSION_MINOR,
				ROBOTACTIONPLUGIN_VERSION_MICRO);
	}

	// stored in ProgramCtx, accessed using getPluginEnvironment<ActionAtom>()
	class Environment: public PluginEnvironment {
	public:
		Environment();
		virtual ~Environment() {
		}
		;
	};

	class RobotActionAtom: public PluginActionAtom<RobotActionPlugin> {
	public:
		RobotActionAtom() :
				PluginActionAtom("RobotActionAtom") {
			addInputConstant();
			setOutputArity(1);
		}
	protected:
		virtual void retrieve(const Environment& environment,
				const Query& query, Answer& answer) {

			Registry &registry = *getRegistry();

			int arity = query.input.size();

			if (arity != 1)
				throw PluginError("Wrong input argument type");

			ID id1 = query.input[0];

			const Term& s1 = registry.terms.getByID(id1);

			bool quantityOfFuel;

			if (id1.isConstantTerm() && s1.getUnquotedString() == "fuel") {
				srand(time(0));
				quantityOfFuel = rand() % 2;
			} else
				throw PluginError("Wrong input argument type");

			Tuple out;

			//
			// call Term::Term with second argument true to get a quoted string!
			//
			Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,
					quantityOfFuel ? "low" : "high");
			out.push_back(registry.storeTerm(term));
			answer.get().push_back(out);

		}

	};

	class RobotAction: public PluginAction<RobotActionPlugin> {

	public:

		RobotAction() :
				PluginAction("RobotAction") {
			//addInputConstant();
			currentAlarm = 0;
			world.push_back(
					"                                                                      ");
			world.push_back(
					"                                ALARM: OFF                            ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"                            +                +                        ");
			world.push_back(
					"    +++++++++++++++++++++++++                +++++++++++++++++++++++++");
			world.push_back(
					"    +                                                                +");
			world.push_back(
					"    +                                                                +");
			world.push_back(
					"    +                                                                +");
			world.push_back(
					"    +                                                                +");
			world.push_back(
					"    +                                                                +");
			world.push_back(
					"    +              ++++++++++++++++++++++++++++++++++++++            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    +              +                                    +            +");
			world.push_back(
					"    ++++++++++++++++                                    ++++++++++++++");
			currentX = 11;
			currentY = 37;
			putRobotInWorld(currentX, currentY);
		}

	protected:

		virtual void execute(RobotActionPlugin::Environment& environment,
				const RegistryPtr registryPtr, const Tuple& parms,
				const InterpretationPtr interpretationPtr) {

			int c = system("clear");

			if (registryPtr->getTermStringByID(parms[0]) == "move") {
				if (registryPtr->getTermStringByID(parms[1]) == "left") {
					leftCorridor();
				} else if (registryPtr->getTermStringByID(parms[1])
						== "right") {
					rightCorridor();
				} else if (registryPtr->getTermStringByID(parms[1]) == "all") {
					leftCorridor();
					rightCorridor();
				} else {
					std::cout << "unknown move!\n";
				}
			} else if (registryPtr->getTermStringByID(parms[0])
					== "turnAlarm") {
				if (registryPtr->getTermStringByID(parms[1]) == "on") {
					turnAlarmOn();
					currentAlarm = 1;
				} else if (registryPtr->getTermStringByID(parms[1]) == "off") {
					turnAlarmOff();
					currentAlarm = 0;
				} else {
					std::cout << "invalid option!\n";
				}
			}

			refresh();

		}

	private:

		std::vector<std::string> world;
		std::vector<std::string> tempWorld;
		int currentX;
		int currentY;
		bool currentAlarm;

		void putRobotInWorld(int x, int y) {
			tempWorld = world;
			tempWorld[x][y] = 'O';
		}

		void moveDown(int x) {
			for (int i = 0; i < x; i++) {
				putRobotInWorld(++currentX, currentY);
				refresh();
			}
		}

		void moveUp(int x) {
			for (int i = 0; i < x; i++) {
				putRobotInWorld(--currentX, currentY);
				refresh();
			}
		}

		void moveLeft(int x) {
			for (int i = 0; i < x; i++) {
				putRobotInWorld(currentX, --currentY);
				refresh();
			}
		}

		void moveRight(int x) {
			for (int i = 0; i < x; i++) {
				putRobotInWorld(currentX, ++currentY);
				refresh();
			}
		}

		void refresh() {
			int c = system("clear");
			tempWorld = world;
			tempWorld[currentX][currentY] = 'O';

			for (int j = 0; j < 35; j++) {
				std::cout << tempWorld[j] << std::endl;
			}

			usleep(1000 * 500);

		}

		void turnAlarmOn() {
			moveUp(9);
			if (!currentAlarm) {
				moveRight(1);
				moveRight(1);
				world[1] =
						"                                ALARM: ONF                            ";
				moveRight(1);
				world[1] =
						"                                ALARM: ON                             ";
				moveRight(1);
				refresh();
				moveLeft(4);
			}
			moveDown(9);
		}

		void turnAlarmOff() {
			moveUp(9);
			if (currentAlarm) {
				moveRight(1);
				moveRight(1);
				world[1] =
						"                                ALARM: OF                            ";
				moveRight(1);
				world[1] =
						"                                ALARM: OFF                            ";
				moveRight(1);
				refresh();
				moveLeft(4);
			}
			moveDown(9);
		}

		void leftCorridor() {
			moveLeft(25);
			moveDown(22);
			sleep(1);
			moveUp(22);
			moveRight(25);
		}

		void rightCorridor() {
			moveRight(25);
			moveDown(22);
			sleep(1);
			moveUp(22);
			moveLeft(25);
		}

	};

	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const {
		std::vector < PluginAtomPtr > ret;
		ret.push_back(
				PluginAtomPtr(new RobotActionAtom,
						PluginPtrDeleter<PluginAtom>()));
		return ret;
	}

	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const {
		std::vector < PluginActionBasePtr > ret;
		ret.push_back(
				PluginActionBasePtr(new RobotAction,
						PluginPtrDeleter<PluginActionBase>()));
		return ret;
	}

};

//
// now instantiate the plugin
//
RobotActionPlugin theRobotActionPlugin;

DLVHEX_NAMESPACE_END

//
// let it be loaded by dlvhex!
//
IMPLEMENT_PLUGINABIVERSIONFUNCTION

// return plain C type s.t. all compilers and linkers will like this code
extern "C" void * PLUGINIMPORTFUNCTION() {
return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theRobotActionPlugin);
}

#endif /* ROBOT_ACTION_PLUGIN_H_ */
