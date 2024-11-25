#ifndef MCTS_ULTIMATEXOS_H
#define MCTS_ULTIMATEXOS_H

#include <vector>
#include <cstdint>
#include <queue>
#include "mcts/include/state.h"

using namespace std;

#define GRID_SIZE 3

typedef enum {
    PLAYER1=1,
    PLAYER2,
    ONGOING,
    DRAW
} game_state_t;


class Ultimate_XO_state : public MCTS_state {
private:
    std::vector<std::vector<std::vector<std::uint8_t>>> subboard; // [subgrid][row][col]
    std::vector<std::vector<std::uint8_t>> board;                // [row][col]
    std::vector<std::uint16_t> occupied_positions;                   // Bitmask of legal moves for subgrids
    uint8_t grid_pointer;                              // Active subgrid
    bool player_turn;
    game_state_t end_state;

    game_state_t calculate_winner() const;
    int calculate_local_winner(void) const;
    bool player_won(game_state_t player) const;
    
public:
    Ultimate_XO_state();
    Ultimate_XO_state(const Ultimate_XO_state &other);

    int get_turn() const;
    bool is_terminal() const override;
    game_state_t get_winner() const;
    MCTS_state *next_state(const MCTS_move *move) const override;
    std::queue<MCTS_move *> *actions_to_try() const override;
    double rollout() const override;
    void print() const override;
    bool player1_turn() const override { return get_turn() == PLAYER1; }
};


struct Ultimate_XO_move : public MCTS_move {
    int row;
    int col;
    Ultimate_XO_move(int row, int col): row(row),  col(col) {}
    bool operator==(const MCTS_move &other) const override;
};

#endif // MCTS_ULTIMATEXOS_H