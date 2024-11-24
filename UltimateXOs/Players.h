#include <iostream>
#include <vector>
#include <cassert> 
#include "board.h"
#include <random>

class Player {
protected:
    int player_id; // Stores the type identifier for the player

public:
    Player(int id) : player_id(id) {} // Constructor to initialize type_id

    virtual int move(const Board& board) = 0; // Pure virtual function
    virtual int get_id() const { return player_id; }        // Getter for type_id

    virtual ~Player() {} // Virtual destructor
};


class RandomPlayer : public Player {
public:
    // Constructor initializes an empty board
    RandomPlayer(int id) : Player(id) {}

    int move(Board& board) {
        int positions = board.get_legal_moves();

        // Return false if no legal moves exist
        if (positions == 0) {
            return false;
        }

        // Count the number of legal moves (set bits)
        int num_legal_moves = __builtin_popcount(positions); // GCC/Clang intrinsic to count set bits

        // Randomly select one of the set bits
        std::random_device rd;       // Seed
        std::mt19937 gen(rd());      // Random number generator
        std::uniform_int_distribution<> dist(1, num_legal_moves);

        int target_bit = dist(gen); // 1-based index of the selected legal move

        // Find the position of the target bit
        for (int i = 0; i < 9; ++i) { // Loop through all 9 possible moves
            if (positions & (1 << i)) { // Check if bit `i` is set
                --target_bit; // Decrement target_bit
                if (target_bit == 0) {
                    return i;
                }
            }
        }
        // Should never reach here
        throw std::logic_error("Error in move function for Random_player");
    }
};


class HumanPlayer : public Player {
public:
    // Constructor initializes an empty board
    HumanPlayer(int id) : Player(id) {}

    int move(Board& board) {
        int positions = board.get_legal_moves();

        // Check if there are no legal moves
        if (positions == 0) {
            std::cout << "No legal moves available.\n";
            return -1; // Indicating no moves possible
        }

        int user_move;
        while (true) {
            // Display legal moves to the user
            std::cout << "Legal moves: ";
            for (int i = 0; i < 9; ++i) {
                if (positions & (1 << i)) {
                    std::cout << i << " ";
                }
            }
            std::cout << "\nEnter your move: ";

            // Read user input
            std::cin >> user_move;

            // Validate input
            if (std::cin.fail() || user_move < 0 || user_move >= board.get_action_size()) {
                std::cin.clear();                  // Clear error state
                std::cin.ignore(INT_MAX, '\n');    // Discard invalid input
                std::cout << "Invalid input. Please enter a number within the correct action size.\n";
                continue;
            }

            // Check if the move is legal
            if (positions & (1 << user_move)) {
                return user_move; // Valid move
            } else {
                std::cout << "Illegal move. That position is not available. Try again.\n";
            }
        }
    }
};


class GreedyPlayer : public Player {
public:
    // Constructor initializes an empty board
    GreedyPlayer(int id) : Player(id) {}

    int move(Board& board) {
        return 1;
    }
};
