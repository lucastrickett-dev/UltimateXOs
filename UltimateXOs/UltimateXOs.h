#ifndef MCTS_ULTIMATEXOS_H
#define MCTS_ULTIMATEXOS_H

#include <iostream>
#include <vector>
#include <cassert> 
#include <sstream>
#include <cstdint>
#include "../../mcts/include/state.h"


#define GRID_SIZE 3


class Ultimate_XO_Board : public MCTS_state {
private:
    std::vector<std::vector<std::vector<std::int8_t>>> board; // 3D array: [subgrid][row][col]
    std::vector<std::uint16_t> possible_moves; // Signed 8-bit integers (-128 to 127)
    uint8_t grid_pointer; // Tracks the current active subgrid

public:
    // Default class constructor
    Ultimate_XO_Board(); 

    // Copy class constructor
    Ultimate_XO_Board(const Ultimate_XO_Board &other); 

    // Returns the number of actions (always 9 moves per subgrid)
    int get_action_size(int player) const;

    // Returns legal moves for the current active subgrid as a bitmask
    int get_legal_moves(int player) const;

    // Checks if there are any legal moves in the current active subgrid
    bool has_legal_moves(int player) const;

    // Executes a move in the current active subgrid
    void execute_move(int position, int player_id);

    // Determines the winner of a given subsector (3x3 grid)
    bool get_minor_condition(int subsector, int player) const;

    bool get_win_status(int player) const;

    std::string display_board(void) const;
};


#endif
