#include "../include/knight.h"
#include "../include/bear.h"
#include "../include/orc.h"

Knight::Knight(int x, int y, const std::string& name) : NPC(KnightType, x, y, name) {}
Knight::Knight(std::istream &is) : NPC(KnightType, is) {}

void Knight::print() {
    std::cout << *this;
}

void Knight::print(std::ostream &os) {
    os << *this;
}

bool Knight::accept(std::shared_ptr<NPC> other) {
    return other->fight(std::dynamic_pointer_cast<Knight>(shared_from_this()));
}

void Knight::save(std::ostream &os) {
    os << KnightType << std::endl;
    NPC::save(os);
}

bool Knight::fight(std::shared_ptr<Orc> other) {
    fight_notify(other, true);
    return true;
}

bool Knight::fight(std::shared_ptr<Knight> other) {
    fight_notify(other, false);
    return false;
}

bool Knight::fight(std::shared_ptr<Bear> other) {
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, Knight &knight) {
    os << "Knight " << knight.get_name() << ": " << *static_cast<NPC *>(&knight) << std::endl;
    return os;
}