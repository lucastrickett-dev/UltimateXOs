#include <iostream>
#include <vector>
#include <cassert> 
#include <sstream>
#include <cstdint>

class Board {
protected:
    std::vector<std::vector<int>> board_layout;
public:
    Board(int n) : board_layout(n, std::vector<int>(n, 0)) {}

    virtual int get_action_size(int player) = 0; // Pure virtual functions
    virtual int get_legal_moves(int player) = 0; // Pure virtual functions
    virtual bool has_legal_moves(int player) = 0; // Pure virtual functions
    virtual bool get_win_status(int player) = 0;  // Pure virtual functions
    virtual std::string display_board(void) const = 0;   // Pure virtual functions
    virtual void execute_move(int position, int player_id) = 0;    // Pure virtual functions

    // virtual int get_board_size() const { return n; }        // Getter for type_id

    virtual ~Board() {} // Virtual destructor
};

