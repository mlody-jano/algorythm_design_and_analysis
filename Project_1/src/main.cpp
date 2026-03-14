#include "Load.hpp"
#include "Sort.hpp"

struct Rank {
    string name;
    int ranking;
};

int main(int argc, char *argv[]) {

    validDataGuard(argc, argv);

    Load loader;

    loader.setTable(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));

    while(true) {
        loader.showMenu();
        loader.catchOptions();
    }

}