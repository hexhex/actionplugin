/**
 * @file ActionPluginInterfaceRobot.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ROBOT_ACTION_PLUGIN_H_
#define ROBOT_ACTION_PLUGIN_H_

#include "ActionPluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class RobotActionPlugin: public ActionPluginInterface {

  public:

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
        }

        virtual void retrieve(const Environment&, const Query&, Answer&) {

        }

    };

    class RobotAction: public PluginAction<RobotActionPlugin> {

      public:

        RobotAction() :
          PluginAction("RobotAction") {
          //addInputConstant();
          currentAlarm = 0;
          world.push_back("                                                                      ");
          world.push_back("                                ALARM: OFF                            ");
          world.push_back("                            +                +                        ");
          world.push_back("                            +                +                        ");
          world.push_back("                            +                +                        ");
          world.push_back("                            +                +                        ");
          world.push_back("                            +                +                        ");
          world.push_back("                            +                +                        ");
          world.push_back("    +++++++++++++++++++++++++                +++++++++++++++++++++++++");
          world.push_back("    +                                                                +");
          world.push_back("    +                                                                +");
          world.push_back("    +                                                                +");
          world.push_back("    +                                                                +");
          world.push_back("    +                                                                +");
          world.push_back("    +              ++++++++++++++++++++++++++++++++++++++            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    +              +                                    +            +");
          world.push_back("    ++++++++++++++++                                    ++++++++++++++");
          currentX = 11;
          currentY = 37;
          putRobotInWorld(currentX, currentY);
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
            world[1] = "                                ALARM: ONF                            ";
            moveRight(1);
            world[1] = "                                ALARM: ON                             ";
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
            world[1] = "                                ALARM: OF                            ";
            moveRight(1);
            world[1] = "                                ALARM: OFF                            ";
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

        virtual void execute(Environment& environment, const Registry& registry, Tuple& parms,
            InterpretationPtr & interpretationPtr) {

          int c = system("clear");

          if (registry.getTermStringByID(parms[0]) == "move") {
            if (registry.getTermStringByID(parms[1]) == "left") {
              leftCorridor();
            } else if (registry.getTermStringByID(parms[1]) == "right") {
              rightCorridor();
            } else if (registry.getTermStringByID(parms[1]) == "all") {
              leftCorridor();
              rightCorridor();
            } else {
              std::cout << "unknown move!\n";
            }
          } else if (registry.getTermStringByID(parms[0]) == "turnAlarm") {
            if (registry.getTermStringByID(parms[1]) == "on") {
              turnAlarmOn();
              currentAlarm = 1;
            } else if (registry.getTermStringByID(parms[1]) == "off") {
              turnAlarmOff();
              currentAlarm = 0;
            } else {
              std::cout << "invalid option!\n";
            }
          }

          refresh();

        }

    };

    virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const {
    }

    virtual std::vector<PluginActionBasePtr> createActions(ProgramCtx& ctx) const {
    }

};

DLVHEX_NAMESPACE_END

#endif /* ROBOT_ACTION_PLUGIN_H_ */
