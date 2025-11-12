#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <map>

// Structure to hold the game rules
struct GameRules {
    int numDice; // Number of dice
    int numSides; // Number of sides on each die
    int numThrows; // Number of throws allowed per turn
    std::vector<std::pair<std::string, int>> scoringCombinations; // Scoring combinations and their points
};

// Structure to hold player information
struct Player {
    std::string name; // Player's name
    int score = 0; // Player's score
};

// Function to load game rules from a file
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
        } else if (line.find("numSides") != std::string::npos) {
            iss >> rules.numSides;
        } else if (line.find("numThrows") != std::string::npos) {
            iss >> rules.numThrows;
        } else {
            std::string combination;
            int points;
            iss >> combination >> points;
            rules.scoringCombinations.emplace_back(combination, points);
        }
    }

    file.close();
    return rules;
}

// Function to roll dice
void rollDice(const GameRules& rules, std::vector<int>& dice) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, rules.numSides);

    for (int& die : dice) {
        die = dist(gen);
    }
}

// Function to display dice
void displayDice(const std::vector<int>& dice) {
    std::cout << "Current dice: ";
    for (size_t i = 0; i < dice.size(); ++i) {
        std::cout << "[" << i + 1 << ":" << dice[i] << "] ";
    }
    std::cout << "\n";
}

// Function to allow the player to select dice to keep or re-roll
void playerTurn(const GameRules& rules, Player& player) {
    std::vector<int> dice(rules.numDice);
    int throwsLeft = rules.numThrows;

    while (throwsLeft > 0) {
        rollDice(rules, dice);
        displayDice(dice);

        std::cout << "You have " << throwsLeft << " throws left.\n";
        std::cout << "Enter the indices of dice to keep (separated by spaces), or press Enter to re-roll all: ";
        std::string input;
        std::getline(std::cin, input);

        if (!input.empty()) {
            std::istringstream iss(input);
            std::vector<int> keepIndices;
            int index;
            while (iss >> index) {
                if (index > 0 && index <= rules.numDice) {
                    keepIndices.push_back(index - 1);
                }
            }

            std::vector<int> newDice(rules.numDice);
            for (int idx : keepIndices) {
                newDice[idx] = dice[idx];
            }
            dice = newDice;
        }

        --throwsLeft;
    }

    displayDice(dice);

    // Present scoring options
    std::cout << "Available scoring combinations:\n";
    for (size_t i = 0; i < rules.scoringCombinations.size(); ++i) {
        std::cout << i + 1 << ". " << rules.scoringCombinations[i].first
                  << " (" << rules.scoringCombinations[i].second << " points)\n";
    }

    std::cout << "Select a scoring combination by number: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(); // Clear the newline character from the input buffer

    if (choice > 0 && choice <= static_cast<int>(rules.scoringCombinations.size())) {
        player.score += rules.scoringCombinations[choice - 1].second;
        std::cout << player.name << " scored " << rules.scoringCombinations[choice - 1].second << " points!\n";
    } else {
        std::cout << "Invalid choice. No points scored this turn.\n";
    }
}

// Function to play the game
void playGame(const GameRules& rules, std::vector<Player>& players) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(players.begin(), players.end(), gen);

    std::cout << "Order of play:\n";
    for (const auto& player : players) {
        std::cout << player.name << "\n";
    }

    const int numTurns = 5; // Number of turns per player
    for (int turn = 1; turn <= numTurns; ++turn) {
        std::cout << "\nTurn " << turn << ":\n";
        for (auto& player : players) {
            std::cout << "\n" << player.name << "'s turn:\n";
            playerTurn(rules, player);
        }
    }

    // Determine the winner
    auto winner = std::max_element(players.begin(), players.end(),
                                    [](const Player& a, const Player& b) {
                                        return a.score < b.score;
                                    });

    std::cout << "\nGame over! The winner is " << winner->name
              << " with " << winner->score << " points!\n";
}

int main() {
    try {
        GameRules rules = loadRules("rules.txt");

        int numPlayers;
        std::cout << "Enter number of players: ";
        std::cin >> numPlayers;
        std::cin.ignore(); // Clear the newline character from the input buffer

        std::vector<Player> players(numPlayers);
        for (int i = 0; i < numPlayers; ++i) {
            std::cout << "Enter name for player " << i + 1 << ": ";
            std::getline(std::cin, players[i].name);
        }

        playGame(rules, players);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}