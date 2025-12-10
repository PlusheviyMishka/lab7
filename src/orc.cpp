#include "../include/orc.h"
#include "../include/knight.h"
#include "../include/bear.h"

Orc::Orc(int x, int y, const std::string& name) : NPC(OrcType, x, y, name) {}
Orc::Orc(std::istream &is) : NPC(OrcType, is) {}

void Orc::print() {
    std::cout << *this;
}

void Orc::print(std::ostream &os) {
    os << *this;
}

bool Orc::accept(std::shared_ptr<NPC> visitor) {
    return visitor->fight(std::dynamic_pointer_cast<Orc>(shared_from_this()));
}

void Orc::save(std::ostream &os) {
    os << OrcType << std::endl;
    NPC::save(os);
}

bool Orc::fight(std::shared_ptr<Bear> other) {
    fight_notify(other, true);
    return true;
}

bool Orc::fight(std::shared_ptr<Knight> other) {
    fight_notify(other, false);
    return false;
}

bool Orc::fight(std::shared_ptr<Orc> other) {
    fight_notify(other, false);
    return false;
}

std::ostream &operator<<(std::ostream &os, Orc &orc) {
    os << "Orc " << orc.get_name() << ": " << *static_cast<NPC*>(&orc) << std::endl;
    return os;
}