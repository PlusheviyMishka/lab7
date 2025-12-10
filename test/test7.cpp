// [file name]: test7.cpp
#include <gtest/gtest.h>
#include "../include/knight.h"
#include "../include/orc.h"
#include "../include/bear.h"
#include "../include/npc.h"

TEST(KnightTests, Test_01_Print) {
    Knight knight(30, 60);
    knight.print();
}

TEST(KnightTests, Test_02_Accept) {
    std::shared_ptr<NPC> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<NPC> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<NPC> orc = std::make_shared<Orc>(10, 50);
    
    bool successb = bear->accept(knight);
    bool successo = orc->accept(knight);
    bool successk = knight->accept(knight);
    
    ASSERT_FALSE(successb);
    ASSERT_FALSE(successk);
    ASSERT_TRUE(successo);
}

TEST(KnightTests, Test_03_Fight) {
    std::shared_ptr<Knight> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<Bear> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<Orc> orc = std::make_shared<Orc>(10, 50);
    
    bool successb = bear->fight(knight);
    bool successo = orc->fight(knight);
    bool successk = knight->fight(knight);
    
    ASSERT_TRUE(successb);
    ASSERT_FALSE(successo);
    ASSERT_FALSE(successk);
}

TEST(KnightTests, Test_04_Output) {
    Knight knight(30, 60); 
    std::cout << knight << std::endl;
}

TEST(OrcTests, Test_01_Print) {
    Orc orc(20, 15); 
    orc.print();
}

TEST(OrcTests, Test_02_Accept) {
    std::shared_ptr<NPC> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<NPC> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<NPC> orc = std::make_shared<Orc>(10, 50);

    bool successb = bear->accept(orc);
    bool successo = orc->accept(orc);
    bool successk = knight->accept(orc);
    
    ASSERT_TRUE(successb);
    ASSERT_FALSE(successo);
    ASSERT_TRUE(successk);
}

TEST(OrcTests, Test_03_Fight) {
    std::shared_ptr<Knight> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<Bear> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<Orc> orc = std::make_shared<Orc>(10, 50);

    bool successo = orc->fight(knight);
    bool successb = orc->fight(bear);
    
    ASSERT_FALSE(successo);
    ASSERT_TRUE(successb);
}

TEST(OrcTests, Test_04_Output) {
    Orc orc(20, 15); 
    std::cout << orc << std::endl;
}

TEST(BearTests, Test_01_Print) {
    Bear bear(10, 50);
    bear.print();
}

TEST(BearTests, Test_02_Accept) {
    std::shared_ptr<NPC> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<NPC> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<NPC> orc = std::make_shared<Orc>(10, 50);
    
    bool successb = bear->accept(bear);
    bool successo = orc->accept(bear);
    bool successk = knight->accept(bear);
    
    ASSERT_FALSE(successb);
    ASSERT_TRUE(successo);
    ASSERT_FALSE(successk);
}

TEST(BearTests, Test_03_Fight) {
    std::shared_ptr<Knight> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<Bear> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<Orc> orc = std::make_shared<Orc>(10, 50);

    bool successb = bear->fight(knight);
    bool successbb = orc->fight(bear);
    
    ASSERT_TRUE(successb);
    ASSERT_TRUE(successbb);
}

TEST(BearTests, Test_04_Output) {
    Bear bear(10, 50); 
    std::cout << bear << std::endl;
}

TEST(NPCTests, Test_01_Combat) {
    std::set<std::shared_ptr<NPC>> array;

    std::shared_ptr<Knight> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<Bear> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<Orc> orc = std::make_shared<Orc>(10, 50);

    array.insert(knight);
    array.insert(bear);
    array.insert(orc);

    size_t distance = 25;
    std::set<std::shared_ptr<NPC>> dead_list;
    for (const auto &attacker : array)
        for (const auto &defender : array)
            if ((attacker != defender) && (attacker->is_close(defender, distance))) {
                bool success = defender->accept(attacker);
                if (success)
                    dead_list.insert(defender);
            }

    std::cout << "Killed: " << dead_list.size() << std::endl;
}

TEST(NPCTests, Test_02_Survivors) {
    std::set<std::shared_ptr<NPC>> array;
    std::shared_ptr<Knight> knight = std::make_shared<Knight>(30, 60);
    std::shared_ptr<Bear> bear = std::make_shared<Bear>(20, 15);
    std::shared_ptr<Orc> orc = std::make_shared<Orc>(10, 50);

    array.insert(knight);
    array.insert(bear);
    array.insert(orc);

    size_t distance = 25;
    std::set<std::shared_ptr<NPC>> dead_list;

    for (const auto &attacker : array)
        for (const auto &defender : array)
            if ((attacker != defender) && (attacker->is_close(defender, distance))) {
                bool success = defender->accept(attacker);
                if (success)
                    dead_list.insert(defender);
            }

    for (auto &d : dead_list)
        array.erase(d);

    std::cout << "Survivors: " << std::endl;
    for (auto &n : array)
        n->print();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}