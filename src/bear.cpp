#include "../include/knight.h"
#include "../include/bear.h"
#include "../include/orc.h"

Bear::Bear(int x, int y, const std::string& name) : NPC(BearType, x, y, name) {}
Bear::Bear(std::istream &is) : NPC(BearType, is) {}

void Bear::print() {
    std::cout << *this;
}

void Bear::print(std::ostream &os) {
    os << *this;
}

bool Bear::accept(std::shared_ptr<NPC> visitor) {
    return visitor->fight(std::dynamic_pointer_cast<Bear>(shared_from_this()));
}

void Bear::save(std::ostream &os) {
    os << BearType << std::endl;
    NPC::save(os);
}

bool Bear::fight(std::shared_ptr<Bear> other) {
    fight_notify(other, false);
    return false;
}

bool Bear::fight(std::shared_ptr<Orc> other) {
    fight_notify(other, false);
    return false;
}

bool Bear::fight(std::shared_ptr<Knight> other) {
    fight_notify(other, true);
    return true;
}

std::ostream &operator<<(std::ostream &os, Bear &bear) {
    os << "Bear " << bear.get_name() << ": " << *static_cast<NPC *>(&bear) << std::endl;
    return os;
}