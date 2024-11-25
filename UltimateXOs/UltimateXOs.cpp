#include "UltimateXOs.h"
#include <iostream>
#include <ctime>
#include <cassert>
#include <sstream>
#include <deque>
#include <cstdlib>

using namespace std;

Ultimate_XO_state::Ultimate_XO_state(): 
    MCTS_state(),
    subboard(GRID_SIZE * GRID_SIZE, std::vector<std::vector<std::uint8_t>>(GRID_SIZE, std::vector<std::uint8_t>(GRID_SIZE, 0))),
    board(GRID_SIZE, std::vector<std::uint8_t>(GRID_SIZE, 0)),
    occupied_positions(GRID_SIZE * GRID_SIZE, 0b111111111), // All moves possible at start
    grid_pointer(4), // Default to the middle subgrid
    player_turn(true), // Assume X starts
    end_state(ONGOING) {}

Ultimate_XO_state::Ultimate_XO_state(const Ultimate_XO_state &other): 
    MCTS_state(other),
    subboard(other.subboard),
    board(other.board),
    occupied_positions(other.occupied_positions),
    grid_pointer(other.grid_pointer),
    player_turn(other.player_turn),
    end_state(other.end_state) {}


int Ultimate_XO_state::get_turn() const { return player_turn; } // Alternate turns based on grid_pointer

game_state_t Ultimate_XO_state::get_winner() const { return end_state; }

bool Ultimate_XO_state::is_terminal() const { return end_state != ONGOING; }

game_state_t Ultimate_XO_state::calculate_winner() const {
    if (player_won(PLAYER1)) return PLAYER1;
    if (player_won(PLAYER2)) return PLAYER2;
    if (occupied_positions[grid_pointer] == 0) return DRAW; // draw
    
    return ONGOING;  // no-one yet
}

bool Ultimate_XO_state::player_won(game_state_t player) const {
    for (int i = 0; i < GRID_SIZE; ++i) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) || // Rows
            (board[0][i] == player && board[1][i] == player && board[2][i] == player))  // Columns
            return true;
    }
    return (board[0][0] == player && board[1][1] == player && board[2][2] == player) || // Diagonal
           (board[0][2] == player && board[1][1] == player && board[2][0] == player);   // Anti-diagonal
}

int Ultimate_XO_state::calculate_local_winner() const {
    for (int i = 0; i < GRID_SIZE; ++i) {
        // Check rows and columns for a winner
        if (subboard[grid_pointer][i][0] != 0 && // Ensure the cell isn't empty
            subboard[grid_pointer][i][0] == subboard[grid_pointer][i][1] &&
            subboard[grid_pointer][i][1] == subboard[grid_pointer][i][2]) {
            return subboard[grid_pointer][i][0]; // Return the ID of the winner
        }

        if (subboard[grid_pointer][0][i] != 0 && // Ensure the cell isn't empty
            subboard[grid_pointer][0][i] == subboard[grid_pointer][1][i] &&
            subboard[grid_pointer][1][i] == subboard[grid_pointer][2][i]) {
            return subboard[grid_pointer][0][i]; // Return the ID of the winner
        }
    }

    // Check diagonals for a winner
    if (subboard[grid_pointer][0][0] != 0 && // Ensure the cell isn't empty
        subboard[grid_pointer][0][0] == subboard[grid_pointer][1][1] &&
        subboard[grid_pointer][1][1] == subboard[grid_pointer][2][2]) {
        return subboard[grid_pointer][0][0]; // Return the ID of the winner
    }

    if (subboard[grid_pointer][0][2] != 0 && // Ensure the cell isn't empty
        subboard[grid_pointer][0][2] == subboard[grid_pointer][1][1] &&
        subboard[grid_pointer][1][1] == subboard[grid_pointer][2][0]) {
        return subboard[grid_pointer][0][2]; // Return the ID of the winner
    }

    return 0; // No winner
}

MCTS_state *Ultimate_XO_state::next_state(const MCTS_move *move) const {
    // Validate the move type
    const auto *m = dynamic_cast<const Ultimate_XO_move *>(move);
    if (!m) {
        cerr << "Error: Invalid move type!" << endl;
        return nullptr;
    }

    // Clone the current state to create the new state
    Ultimate_XO_state *new_state = new Ultimate_XO_state(*this);

    int row = m->row;
    int col = m->col;
    
    // Make the move on the active subgrid
    if (new_state->subboard[grid_pointer][row][col] == 0) {
        new_state->subboard[grid_pointer][row][col] = (player_turn) ? 1 : 2; // Mark the cell
        new_state->occupied_positions[grid_pointer] &= ~(1 << (row * GRID_SIZE + col)); // Update move mask
        new_state->board[grid_pointer/GRID_SIZE][grid_pointer%GRID_SIZE] = new_state->calculate_local_winner();
        new_state->grid_pointer = row * GRID_SIZE + col; // Set active subgrid for the next turn
        new_state->player_turn = !player_turn; // Alternate player turn
        new_state->end_state = new_state->calculate_winner();
    } else {
        cerr << "Warning: Illegal move!" << endl;
        delete new_state;
        return nullptr;
    }

    return new_state;
}

queue<MCTS_move *> *Ultimate_XO_state::actions_to_try() const {
    int possible_moves = occupied_positions[grid_pointer]; // eg 0b111011001

    queue<MCTS_move *> *Q = new queue<MCTS_move *>();

    // Iterate over all possible positions in the subgrid (0 to 8)
    for (int i = 0; i < GRID_SIZE*GRID_SIZE; ++i) {
        // Check if the position is available by testing the corresponding bit
        if (possible_moves & (1 << i)) {
            // Add the move to the queue
            Q->push(new Ultimate_XO_move(i / GRID_SIZE, i % GRID_SIZE));
        }
    }

    return Q;
}

double Ultimate_XO_state::rollout() const {
    // Clone the current state
    Ultimate_XO_state* state_copy = new Ultimate_XO_state(*this);

    // Simulate the game until it reaches a terminal state
    while (!state_copy->is_terminal()) {
        // Generate possible actions
        auto moves = state_copy->actions_to_try();
        if (moves->empty()) {
            delete moves;
            break; // No moves left, should not happen if the state is consistent
        }

        // Select a random move
        int choice = rand() % moves->size();
        MCTS_move *random_move = nullptr;

        for (int i = 0; i <= choice; ++i) {
            random_move = moves->front();
            moves->pop();
            if (i < choice) delete random_move; // Delete unused moves
        }
        delete moves;

        // Generate the next state based on the selected move
        MCTS_state *next = state_copy->next_state(random_move);
        delete random_move;
        delete state_copy; // Clean up the previous state
        state_copy = dynamic_cast<Ultimate_XO_state *>(next);
    }

    // Determine the winner and calculate the reward
    game_state_t winner = state_copy->get_winner();
    delete state_copy; // Clean up the final state

    // Assign rewards based on the winner
    return (winner == PLAYER1) ? 1.0 : (winner == DRAW) ? 0.5 : 0.0;
}

// double Ultimate_XO_state::rollout() const {
//     Ultimate_XO_state *state_copy = new Ultimate_XO_state(*this);

//     while (!state_copy->is_terminal()) {
//         auto moves = state_copy->actions_to_try();
//         int choice = rand() % moves->size();
//         for (int i = 0; i < choice; ++i) {
//             moves->pop();
//         }
//         auto *random_move = moves->front();
//         delete moves;
//         MCTS_state *next = state_copy->next_state(random_move);
//         delete random_move;
//         delete state_copy;
//         state_copy = dynamic_cast<Ultimate_XO_state *>(next);
//     }

//     char winner = state_copy->get_winner();
//     delete state_copy;

//     return (winner == 'X') ? 1.0 : (winner == 'D') ? 0.5 : 0.0;
// }

void Ultimate_XO_state::print() const {
    // Loop through the 3x3 grid of subboards
    for (int rowBlock = 0; rowBlock < GRID_SIZE; ++rowBlock) {
        for (int subRow = 0; subRow < GRID_SIZE; ++subRow) {
            // Loop through each subgrid in the current row block
            for (int colBlock = 0; colBlock < GRID_SIZE; ++colBlock) {
                int subgrid = rowBlock * GRID_SIZE + colBlock;
                // Print the specific row of the subgrid
                for (int cell : subboard[subgrid][subRow]) {
                    cout << (cell == 0 ? '-' : (cell == 1 ? 'X' : 'O')) << " ";
                }
                cout << "   "; // Spacer between subgrids
            }
            cout << endl; // End the row in this block of subgrids
        }
        cout << endl; // Spacer between blocks of subgrids
    }

    // Print the larger board's state, representing the winners of subgrids
    cout << "Larger Board State:\n";
    for (const auto &row : board) {
        for (int cell : row) {
            cout << (cell == 0 ? '-' : (cell == 1 ? 'X' : 'O')) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool Ultimate_XO_move::operator==(const MCTS_move &other) const {
    const Ultimate_XO_move &o = (const Ultimate_XO_move &) other;        // Note: Casting necessary
    return row == o.row && col == o.col;
}
