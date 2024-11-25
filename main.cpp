#include <iostream>
#include <memory>
#include "UltimateXOs.h"
#include "mcts/include/mcts.h"

using namespace std;

int main() {
    std::cout << "Initial Game State:\n";
    Ultimate_XO_state initial_state;
    initial_state.print();

    // Perform a few moves to set up a non-initial state
    std::cout << "\nPerforming moves...\n";

    // Move (1, 1)
    const MCTS_state* next_state1 = initial_state.next_state(std::make_unique<Ultimate_XO_move>(1, 1).get());
    initial_state = dynamic_cast<const Ultimate_XO_state&>(*next_state1);
    std::cout << "After Move (1, 1):\n";
    initial_state.print();

    // Move (0, 0)
    const MCTS_state* next_state2 = initial_state.next_state(std::make_unique<Ultimate_XO_move>(0, 0).get());
    initial_state = dynamic_cast<const Ultimate_XO_state&>(*next_state2);
    std::cout << "After Move (0, 0):\n";
    initial_state.print();

    // Test rollout
    std::cout << "\nTesting rollout from current state...\n";
    int winner = initial_state.rollout();

    initial_state.print();

    std::cout << "\nRollout completed. Winner: ";
    if (winner == 1) {
        std::cout << "X\n";
    } else if (winner == 0) {
        std::cout << "O\n";
    } else {
        std::cout << "Draw\n";
    }


    return 0;
}
