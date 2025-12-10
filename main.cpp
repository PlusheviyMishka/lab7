#include <sstream>
#include <atomic>
#include <thread>
#include <optional>
#include <array>
#include <chrono>
#include <queue>
#include <mutex>
#include <fstream>
#include <shared_mutex>
#include <iomanip>
#include "include/npc.h"
#include "include/orc.h"
#include "include/knight.h"
#include "include/bear.h"

using namespace std::chrono_literals;
std::mutex console_mutex;
std::mutex file_mutex;
std::ofstream log_file("game_log.txt");

class ConsoleObserver : public IFightObserver {
public:

    ConsoleObserver() = default;
    
    ConsoleObserver(const ConsoleObserver&) = delete;
    ConsoleObserver& operator=(const ConsoleObserver&) = delete;

    static std::shared_ptr<IFightObserver> get() {
        static std::shared_ptr<ConsoleObserver> instance = 
            std::make_shared<ConsoleObserver>();
        return instance;
    }

    void on_fight(const std::shared_ptr<NPC> attacker, 
                  const std::shared_ptr<NPC> defender, 
                  bool win) override {
        if (win) {
            std::lock_guard<std::mutex> lock(console_mutex);
            std::cout << std::endl << "=== MURDER ===" << std::endl;
            std::cout << "Attacker: ";
            attacker->print();
            std::cout << "Defender: ";
            defender->print();
            std::cout << "=============" << std::endl << std::endl;
        }
    }
};

class FileObserver : public IFightObserver {
private:
    std::ofstream log_file;
    
public:
    FileObserver() {
        log_file.open("battle_log.txt", std::ios::app);
    }
    
    ~FileObserver() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    // Запрещаем копирование
    FileObserver(const FileObserver&) = delete;
    FileObserver& operator=(const FileObserver&) = delete;

    static std::shared_ptr<IFightObserver> get() {
        static std::shared_ptr<FileObserver> instance = 
            std::make_shared<FileObserver>();
        return instance;
    }

    void on_fight(const std::shared_ptr<NPC> attacker, 
                  const std::shared_ptr<NPC> defender, 
                  bool win) override {
        if (win) {
            std::lock_guard<std::mutex> lock(file_mutex);
            log_file << std::endl << "=== MURDER ===" << std::endl;
            log_file << "Attacker: ";
            attacker->print(log_file);
            log_file << "Defender: ";
            defender->print(log_file);
            log_file << "=============" << std::endl << std::endl;
        }
    }
};
//фабрика из файла
std::shared_ptr<NPC> factory(std::istream &is) {
    std::shared_ptr<NPC> result;
    int type{0};
    if (is >> type) {
        switch (type)
        {
        case OrcType:
            result = std::make_shared<Orc>(is);
            break;
        case KnightType:
            result = std::make_shared<Knight>(is);
            break;
        case BearType:
            result = std::make_shared<Bear>(is);
            break;
        }
    }
    else
        std::cerr << "unexpected NPC type:" << type << std::endl;

    if (result) {
        result->subscribe(ConsoleObserver::get());
        result->subscribe(FileObserver::get());
    }
    return result;
}

//фабрика из параметров
std::shared_ptr<NPC> factory(NpcType type, int x, int y) {
    std::shared_ptr<NPC> result;
    switch (type)
    {
    case OrcType:
        result = std::make_shared<Orc>(x, y);
        break;
    case KnightType:
        result = std::make_shared<Knight>(x, y);
        break;
    case BearType:
        result = std::make_shared<Bear>(x, y);
        break;
    default:
        break;
    }
    if (result) {
        result->subscribe(ConsoleObserver::get());
        result->subscribe(FileObserver::get());
    }
    return result;
}

std::ostream &operator<<(std::ostream &os, const set_t &array) {
    for (auto &n : array)
        n->print();
    return os;
}

void print_to_file(std::ofstream &fs, const set_t &array) {
    for (auto &n : array)
        n->print(fs);
}

bool k = true, m = true;
struct FightEvent {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class FightManager {
private:
    std::queue<FightEvent> events;
    std::shared_mutex mtx;
    FightManager() {}

public:
    static FightManager &get() {
        static FightManager instance;
        return instance;
    }

    void add_event(FightEvent &&event) {
        std::lock_guard<std::shared_mutex> lock(mtx);
        events.push(event);
    }

    void operator()() {
        while (k) {
            std::optional<FightEvent> event;
            {
                std::lock_guard<std::shared_mutex> lock(mtx);
                if (!events.empty()) {
                    event = events.front();
                    events.pop();
                }
            }
            if (event) {
                try {
                    if (event->attacker->is_alive() && event->defender->is_alive()) {
                        if (event->defender->accept(event->attacker)) {
                            event->defender->must_die();
                        }
                    }
                }
                catch (...) {
                    std::lock_guard<std::shared_mutex> lock(mtx);
                    events.push(*event);
                }
            }
            else
                std::this_thread::sleep_for(100ms);
        }
    }
};

int main() {
    log_file << "=== Game Start ===" << std::endl;
    std::cout << "=== Game Start ===" << std::endl;
    
    set_t array;
    const int MAX_X{500};
    const int MAX_Y{500};
    const int DISTANCE{10};

    std::cout << "Generating NPCs ... " << std::endl;
    log_file << "Generating NPCs ... " << std::endl;
    
    // Создаем NPC с разными типами
    for (size_t i = 0; i < 5; ++i) {
        array.insert(factory(OrcType, std::rand() % 500, std::rand() % 500));
    }
    for (size_t i = 0; i < 3; ++i) {
        array.insert(factory(KnightType, std::rand() % 500, std::rand() % 500));
    }
    for (size_t i = 0; i < 2; ++i) {
        array.insert(factory(BearType, std::rand() % 500, std::rand() % 500));
    }

    std::cout << "Starting NPCs (" << array.size() << "): " << std::endl;
    log_file << "Starting NPCs (" << array.size() << "): " << std::endl;
    
    std::cout << array;
    print_to_file(log_file, array);

    std::thread fight_thread(std::ref(FightManager::get()));

    int count = 0;
    std::thread move_thread([&array, MAX_X, MAX_Y, DISTANCE, &count]() {
        while (m) {
            for (std::shared_ptr<NPC> npc : array) {
                if(npc->is_alive()){
                    int shift_x = std::rand() % 20 - 10;
                    int shift_y = std::rand() % 20 - 10;
                    npc->move(shift_x, shift_y, MAX_X, MAX_Y);
                }
            }
            for (std::shared_ptr<NPC> npc : array) {
                for (std::shared_ptr<NPC> other : array) {
                    if (other != npc && npc->is_alive() && other->is_alive()) {
                        if (npc->is_close(other, DISTANCE)) {
                            FightManager::get().add_event({npc, other});
                        }
                    }
                }
            }
            std::this_thread::sleep_for(1s);
            ++count;
        }
    });

    int now = 0, end = 30; 
    while (now < end && m) {
        const int grid{20}, step_x{MAX_X / grid}, step_y{MAX_Y / grid};
        std::array<char, grid * grid> fields{0};
        std::array<std::string, grid * grid> names{""};
        
        // Собираем информацию о NPC на карте
        for (std::shared_ptr<NPC> npc : array) {
            auto [x, y] = npc->position();
            int i = std::min(x / step_x, grid - 1);
            int j = std::min(y / step_y, grid - 1);
            
            int index = i + grid * j;
            
            if (npc->is_alive()) {
                switch (npc->get_type()) {
                    case KnightType:
                        fields[index] = 'K';
                        names[index] = npc->get_name();
                        break;
                    case OrcType:
                        fields[index] = 'O';
                        names[index] = npc->get_name();
                        break;
                    case BearType:
                        fields[index] = 'B';
                        names[index] = npc->get_name();
                        break;
                    default:
                        break;
                }
            } else {
                fields[index] = 'X'; // Мертвые NPC
                names[index] = "DEAD";
            }
        }

        if(count > 25) {
            break;
        }
        
        // Вывод в консоль
        {
            std::lock_guard<std::mutex> lck(console_mutex);
            std::cout << "\n=== Turn " << std::setw(2) << now << " ===" << std::endl;
            std::cout << "Legend: K=Knight, O=Orc, B=Bear, X=Dead" << std::endl;
            
            for (int j = 0; j < grid; ++j) {
                for (int i = 0; i < grid; ++i) {
                    int index = i + j * grid;
                    char c = fields[index];
                    std::string name = names[index];
                    
                    if (c != 0) {
                        std::cout << "[" << c;
                        if (!name.empty() && name != "DEAD") {
                            std::cout << name.substr(0, 3); // Показываем первые 3 буквы имени
                        } else if (name == "DEAD") {
                            std::cout << "XXX";
                        } else {
                            std::cout << "   ";
                        }
                        std::cout << "]";
                    } else {
                        std::cout << "[    ]";
                    }
                }
                std::cout << std::endl;
            }
            
            // Вывод статистики
            int knights = 0, orcs = 0, bears = 0, dead = 0;
            for (auto& npc : array) {
                if (npc->is_alive()) {
                    switch(npc->get_type()) {
                        case KnightType: knights++; break;
                        case OrcType: orcs++; break;
                        case BearType: bears++; break;
                    }
                } else {
                    dead++;
                }
            }
            
            std::cout << "\nStatistics: Knights: " << knights 
                      << ", Orcs: " << orcs 
                      << ", Bears: " << bears 
                      << ", Dead: " << dead 
                      << ", Total: " << array.size() 
                      << std::endl;
        }
        
        // Вывод в файл
        {
            std::lock_guard<std::mutex> lck(file_mutex);
            log_file << "\n=== Turn " << std::setw(2) << now << " ===" << std::endl;
            for (int j = 0; j < grid; ++j) {
                for (int i = 0; i < grid; ++i) {
                    int index = i + j * grid;
                    char c = fields[index];
                    if (c != 0)
                        log_file << "[" << c << "]";
                    else
                        log_file << "[ ]";
                }
                log_file << std::endl;
            }
        }
        
        std::this_thread::sleep_for(2s); 
        now += 1;
    }

    k = false; m = false;
    move_thread.join();
    fight_thread.join();

    // Финальный вывод
    std::cout << "\n\n=== FINAL RESULTS ===" << std::endl;
    log_file << "\n\n=== FINAL RESULTS ===" << std::endl;
    
    int alive_count = 0;
    int knights = 0, orcs = 0, bears = 0;
    
    std::cout << "\nSurvivors:" << std::endl;
    log_file << "\nSurvivors:" << std::endl;
    
    for (auto &n : array) {
        if (n->is_alive()) {
            alive_count++;
            switch(n->get_type()) {
                case KnightType: knights++; break;
                case OrcType: orcs++; break;
                case BearType: bears++; break;
            }
            n->print();
            n->print(log_file);
        }
    }
    
    std::cout << "\nDead NPCs:" << std::endl;
    log_file << "\nDead NPCs:" << std::endl;
    
    for (auto &n : array) {
        if (!n->is_alive()) {
            std::cout << "DEAD - ";
            n->print();
            log_file << "DEAD - ";
            n->print(log_file);
        }
    }
    
    std::cout << "\n=== Summary ===" << std::endl;
    std::cout << "Total survivors: " << alive_count << std::endl;
    std::cout << "Knights: " << knights << std::endl;
    std::cout << "Orcs: " << orcs << std::endl;
    std::cout << "Bears: " << bears << std::endl;
    std::cout << "Total dead: " << (array.size() - alive_count) << std::endl;
    
    log_file << "\n=== Summary ===" << std::endl;
    log_file << "Total survivors: " << alive_count << std::endl;
    log_file << "Knights: " << knights << std::endl;
    log_file << "Orcs: " << orcs << std::endl;
    log_file << "Bears: " << bears << std::endl;
    log_file << "Total dead: " << (array.size() - alive_count) << std::endl;
    
    std::cout << "\n=== Game End ===" << std::endl;
    log_file << "\n=== Game End ===" << std::endl;
    
    log_file.close();
    
    // Краткий итог победителя
    std::cout << "\n=== WINNER ===" << std::endl;
    if (knights > orcs && knights > bears) {
        std::cout << "Knights win the battle!" << std::endl;
    } else if (orcs > knights && orcs > bears) {
        std::cout << "Orcs win the battle!" << std::endl;
    } else if (bears > knights && bears > orcs) {
        std::cout << "Bears win the battle!" << std::endl;
    } else if (alive_count == 0) {
        std::cout << "Everyone died! It's a draw!" << std::endl;
    } else {
        std::cout << "It's a tie between multiple factions!" << std::endl;
    }
    
    return 0;
}