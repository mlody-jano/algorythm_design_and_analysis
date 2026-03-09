#include "declarations.hpp"

int main(int argc, char *argv[]) {

    validDataGuard(argc, argv);

    Solution solution;

    solution.setTable(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));

    while(true) {
        solution.showMenu();
        solution.catchOptions();
    }

}