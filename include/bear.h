#pragma once
#include "npc.h"

class Bear : public NPC {
public:
    Bear(int x, int y, const std::string& name = "");
    Bear(std::istream &is);

    void print() override;
    void print(std::ostream &os) override;
    
    bool accept(std::shared_ptr<NPC> visitor) override;
    
    bool fight(std::shared_ptr<Orc> other) override;
    bool fight(std::shared_ptr<Knight> other) override;
    bool fight(std::shared_ptr<Bear> other) override;
    
    void save(std::ostream &os) override;
    
    friend std::ostream &operator<<(std::ostream &os, Bear &bear);
};