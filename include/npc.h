#pragma once

#include <iostream>
#include <memory>
#include <cstring>
#include <string>
#include <random>
#include <fstream>
#include <set>
#include <cmath>
#include <shared_mutex>
#include <mutex>
#include <vector>

class NPC;
class IFightObserver;
class Orc;
class Knight;
class Bear;

using set_t = std::set<std::shared_ptr<NPC>>;

enum NpcType
{
    Unknown = 0,
    OrcType = 1,
    KnightType = 2,
    BearType = 3
};

class IFightObserver {
public:
    virtual void on_fight(const std::shared_ptr<NPC> attacker, const std::shared_ptr<NPC> defender, bool win) = 0;
};

class NPC : public std::enable_shared_from_this<NPC> {
private: 
    mutable std::mutex mtx;  // Добавили mutable
    NpcType type;
    int x{0};
    int y{0};
    bool alive{true};
    std::string name;
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int _x, int _y, const std::string& _name = "");
    NPC(NpcType t, std::istream &is);

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    virtual bool is_close(const std::shared_ptr<NPC> &other, size_t distance);

    virtual bool accept(std::shared_ptr<NPC> visitor) = 0;
    virtual bool fight(std::shared_ptr<Orc> other) = 0;
    virtual bool fight(std::shared_ptr<Knight> other) = 0;
    virtual bool fight(std::shared_ptr<Bear> other) = 0;
    
    bool visit(std::shared_ptr<Orc> other);
    bool visit(std::shared_ptr<Knight> other);
    bool visit(std::shared_ptr<Bear> other);

    std::pair<int, int> position();
    NpcType get_type();
    std::string get_name() const;  // const метод
    
    virtual void print() = 0;
    virtual void print(std::ostream &os) = 0;

    virtual void save(std::ostream &os);

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);

    void move(int shift_x, int shift_y, int max_x, int max_y);

    bool is_alive();
    void must_die();
};