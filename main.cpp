#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>

struct GameRules {
    int numDice;
    int numSides;
    int numThrows;
    std::vector<std::pair<std::string, int>> scoringCombinations;
};

struct Player {
    std::string name;
    int score = 0;
};

GameRules loadRules(const std::string& filePath) {
    GameRules rules;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open rules file.");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        if (line.find("numDice") != std::string::npos) {
            iss >> rules.numDice;
        }
        else if (line.find("numSides") != std::string::npos) {
            iss >> rules.numSides;
        }
        else if (line.find("numThrows") != std::string::npos) {
            iss >> rules.numThrows;
        }
        else {
            std::string combination;
            int points;
            iss >> combination >> points;
            rules.scoringCombinations.emplace_back(combination, points);
        }
    }

    file.close();
    return rules;
}

void rollDice(const GameRules& rules, std::vector<int>& dice) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, rules.numSides);

    dice.clear();
    for (int i = 0; i < rules.numDice; ++i) {
        dice.push_back(dist(gen));
    }
}

void playGame(const GameRules& rules, std::vector<Player>& players) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(players.begin(), players.end(), gen);

    std::cout << "Order of play:\n";
    for (const auto& player : players) {
        std::cout << player.name << "\n";
    }

    for (auto& player : players) {
        std::cout << "\n" << player.name << "'s turn:\n";
        for (int throwNum = 1; throwNum <= rules.numThrows; ++throwNum) {
            std::vector<int> dice;
            rollDice(rules, dice);

            std::cout << "Throw " << throwNum << ": ";
            for (int die : dice) {
                std::cout << die << " ";
            }
            std::cout << "\n";

            // Scoring logic can be added here based on rules.scoringCombinations
        }
    }
}

int main() {
    try {
        GameRules rules = loadRules("rules.txt");

        int numPlayers;
        std::cout << "Enter number of players: ";
        std::cin >> numPlayers;

        std::vector<Player> players(numPlayers);
        for (int i = 0; i < numPlayers; ++i) {
            std::cout << "Enter name for player " << i + 1 << ": ";
            std::cin >> players[i].name;
        }

        playGame(rules, players);
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}