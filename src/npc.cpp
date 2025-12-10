#include "../include/npc.h"
#include "../include/knight.h"
#include "../include/bear.h"
#include "../include/orc.h"
#include <random>
#include <sstream>

// Списки имен для каждого типа NPC
namespace Names {
    const std::vector<std::string> KnightNames = {
        "Arthur", "Lancelot", "Galahad", "Percival", "Gawain",
        "Bedivere", "Tristan", "Gareth", "Bors", "Mordred"
    };
    
    const std::vector<std::string> OrcNames = {
        "Grom", "Mog", "Thrak", "Gul'dan", "Kargath",
        "Nazgrim", "Garrosh", "Durotan", "Blackhand", "Kilrogg"
    };
    
    const std::vector<std::string> BearNames = {
        "Baloo", "Winnie", "Yogi", "Smokey", "Paddington",
        "Fozzie", "Boo-Boo", "Gentle", "Grizzly", "Kodiak"
    };
}

// Генератор случайных имен
std::string generate_random_name(NpcType type) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    switch(type) {
        case KnightType: {
            std::uniform_int_distribution<> dis(0, Names::KnightNames.size() - 1);
            return Names::KnightNames[dis(gen)];
        }
        case OrcType: {
            std::uniform_int_distribution<> dis(0, Names::OrcNames.size() - 1);
            return Names::OrcNames[dis(gen)];
        }
        case BearType: {
            std::uniform_int_distribution<> dis(0, Names::BearNames.size() - 1);
            return Names::BearNames[dis(gen)];
        }
        default:
            return "Unknown";
    }
}

NPC::NPC(NpcType t, int _x, int _y, const std::string& _name) 
    : type(t), x(_x), y(_y), name(_name.empty() ? generate_random_name(t) : _name) {}

NPC::NPC(NpcType t, std::istream &is) : type(t) {
    is >> x;
    is >> y;
    std::getline(is >> std::ws, name);
    if (name.empty()) {
        name = generate_random_name(t);
    }
}

std::string NPC::get_name() const {
    std::lock_guard<std::mutex> lck(mtx);  // Теперь работает с mutable мьютексом
    return name;
}

bool NPC::visit(std::shared_ptr<Orc> orc) {
    return this->fight(orc);
}
bool NPC::visit(std::shared_ptr<Knight> knight) {
    return this->fight(knight);
}
bool NPC::visit(std::shared_ptr<Bear> bear) {
    return this->fight(bear);
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    for (auto &o : observers)
        o->on_fight(shared_from_this(), defender, win);
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) {
    auto [other_x, other_y] = other->position();
    std::lock_guard<std::mutex> lck(mtx);
    
    if ((std::pow(x - other_x, 2) + std::pow(y - other_y, 2)) <= std::pow(distance, 2))
        return true;
    else
        return false;
}

NpcType NPC::get_type() {
    std::lock_guard<std::mutex> lck(mtx);
    return type;
}

std::pair<int, int> NPC::position() {
    std::lock_guard<std::mutex> lck(mtx);
    return {x, y};
}

void NPC::save(std::ostream &os) {
    std::lock_guard<std::mutex> lck(mtx);
    os << x << std::endl;
    os << y << std::endl;
    os << name << std::endl;
}

std::ostream &operator<<(std::ostream &os, NPC &npc) {
    std::lock_guard<std::mutex> lck(npc.mtx);
    os << "{ x:" << npc.x << ", y:" << npc.y << ", name:\"" << npc.name << "\"} ";
    return os;
}

void NPC::move(int shift_x, int shift_y, int max_x, int max_y) {
    if (!alive) return; // Мертвые не двигаются
    
    std::lock_guard<std::mutex> lck(mtx);
    int move_distance = 0;

    if (this->type == OrcType) {
        move_distance = 20;
    }
    else if (this->type == BearType) {
        move_distance = 5;
    }        
    else if (this->type == KnightType) {
        move_distance = 30;
    }

    shift_x = (shift_x >= 0) ? move_distance : -move_distance;
    shift_y = (shift_y >= 0) ? move_distance : -move_distance;

    if ((x + shift_x >= 0) && (x + shift_x <= max_x))
        x += shift_x;
    if ((y + shift_y >= 0) && (y + shift_y <= max_y))
        y += shift_y;
}

bool NPC::is_alive() {
    std::lock_guard<std::mutex> lck(mtx);
    return alive;
}

void NPC::must_die() {
    std::lock_guard<std::mutex> lck(mtx);
    alive = false;
}