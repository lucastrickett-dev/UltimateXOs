#include <iostream>
#include <vector>
#include <cassert> 
#include <sstream>
#include <cstdint>
#include "board.h"
#include <string>

#include "UltimateXOs.h"

using namespace std;


Ultimate_XO_Board::Ultimate_XO_Board(): 
    Board(GRID_SIZE),  // Initialize the base class constructor (assuming Board constructor takes GRID_SIZE)
    board(GRID_SIZE * GRID_SIZE, std::vector<std::vector<std::int8_t>>(GRID_SIZE, std::vector<std::int8_t>(GRID_SIZE, 0))),
    possible_moves(GRID_SIZE * GRID_SIZE, 0b111111111),  // Bitmask for legal moves (all possible initially)
    grid_pointer(4) {} // Set grid_pointer to some default valid value (assuming 4 is valid here)


Ultimate_XO_Board::Ultimate_XO_Board(const Ultimate_XO_Board &other): 
    Board(other),  // Copy the base class (assuming Board has a copy constructor)
    board(other.board),  // Copy the board structure
    possible_moves(other.possible_moves),  // Copy the legal moves array
    grid_pointer(other.grid_pointer) {}  // Copy the current active subgrid pointer



int Ultimate_XO_Board::get_action_size(int player) const {
    return GRID_SIZE * GRID_SIZE;
};

// Returns legal moves for the current active subgrid as a bitmask
int Ultimate_XO_Board::get_legal_moves(int player) const {
    return possible_moves[grid_pointer];
};

// Checks if there are any legal moves in the current active subgrid
bool Ultimate_XO_Board::has_legal_moves(int player) const {
    return (possible_moves[grid_pointer] != 0);
};

// Executes a move in the current active subgrid
void Ultimate_XO_Board::execute_move(int position, int player_id) {
    assert(position < GRID_SIZE * GRID_SIZE);
    assert(position >= 0);

    int row = position / GRID_SIZE;
    int col = position % GRID_SIZE;

    assert(board[grid_pointer][row][col] == 0); // Ensure the cell is empty

    board[grid_pointer][row][col] = player_id;
    possible_moves[grid_pointer] &= ~(1 << position);
    grid_pointer = position; // Update the active subgrid
};

// Determines the winner of a given subsector (3x3 grid)
bool Ultimate_XO_Board::get_minor_condition(int subsector, int player) const {
    assert(subsector < GRID_SIZE * GRID_SIZE);
    assert(subsector >= 0);

    const auto& subgrid = board[subsector];

    // Check rows and columns
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (subgrid[i][0] == player && subgrid[i][1] == player && subgrid[i][2] == player) return true;
        if (subgrid[0][i] == player && subgrid[1][i] == player && subgrid[2][i] == player) return true;
    };

    // Check diagonals
    return (subgrid[0][0] == player && subgrid[1][1] == player && subgrid[2][2] == player) ||
            (subgrid[0][2] == player && subgrid[1][1] == player && subgrid[2][0] == player);
};


bool Ultimate_XO_Board::get_win_status(int player) const {
    // Check rows for a win
    // Check rows and columns
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (board_layout[i][0] == player && board_layout[i][1] == player && board_layout[i][2] == player) return true;
        if (board_layout[0][i] == player && board_layout[1][i] == player && board_layout[2][i] == player) return true;
    }

    // Check diagonals
    return (board_layout[0][0] == player && board_layout[1][1] == player && board_layout[2][2] == player) ||
        (board_layout[0][2] == player && board_layout[1][1] == player && board_layout[2][0] == player);
};

std::string Ultimate_XO_Board::display_board(void) const {
    std::ostringstream output; // Use string stream to build the board as a string
    int line_break = GRID_SIZE; // Number of minor boards per row in the major board

    for (int row = 0; row < line_break; ++row) {
        for (int subrow = 0; subrow < GRID_SIZE; ++subrow) {
            for (int col = 0; col < line_break; ++col) {
                int minor_index = row * line_break + col;

                // Add the subrow of the minor board
                for (int i = 0; i < GRID_SIZE; ++i) {
                    char cell = board[minor_index][subrow][i] == 0 ? '-' :
                                (board[minor_index][subrow][i] == 1 ? 'X' : 'O');
                    output << cell;
                }

                if (col != line_break - 1) output << " | "; // Vertical separator
            }
            output << "\n";
        }

        if (row != line_break - 1) {
            output << std::string((GRID_SIZE + 3) * line_break - 3, '-') << "\n"; // Horizontal separator
        }
    }

    return output.str(); // Return the constructed string
};
