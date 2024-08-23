#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <cstdlib>

using namespace std;

struct InputData {
    vector<int> known_hand_cards;
    vector<int> known_desk_cards;
    vector<int> cards;
    int cards_to_calculate;
};


const int NUM_VALUES = 13;
const int NUM_SUITS = 4;
const int DECK_SIZE = 52;

vector<int> removeItem(vector<int> list, vector<int> to_remove) {
    int was_removed = 0;
    for (int c = 0; c < to_remove.size(); c++) {
        for (int i = 0; i < list.size(); i++) {
            if (list[i] == to_remove[c]) {
                int var = list[i];
                list[i] = list[list.size() - 1];
                list[list.size() - 1] = var;
                list.pop_back();
                break;
            }
        }
    }
    return list;
}


int in(vector<int> list,int to_check) {
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == to_check) {
            return 1;
        }
    }
    return 0;
}


int has_repeats(vector<int> list) {
    vector<int> elements(0);
    for (int i = 0; i < list.size(); i++) {
        if (in(elements, list[i])) {
            return 1;
        }
        elements.push_back(list[i]);
    }
    return 0;
}

vector<string> splitItem(string item, char separate_string) {
    vector<string> new_arr;
    string current_string = "";
    for (int i = 0; i < item.size(); i++) {
        if (item[i] == separate_string) {
            if (!current_string.empty()) {
                new_arr.push_back(current_string);
                current_string = "";
            }
        } else {
            current_string += item[i];
        }
    }
    if (!current_string.empty()) {
        new_arr.push_back(current_string);
    }
    return new_arr;
}

int calculate_min(int cards_to_calculate) {
    int cards_num = DECK_SIZE - cards_to_calculate;
    int total = 1;
    for (int i = 0; i < cards_to_calculate - 2; i++) {
        total = total * (cards_num - i);
    }
    return total + 1;
}


enum HandRank {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH
};

struct Hand {
    HandRank rank;
    int value;
};

int cardValue(int card) {
    return (card - 1) % NUM_VALUES + 1;
}

int cardSuit(int card) {
    return (card - 1) / NUM_VALUES;
}

int isStraight(const vector<int>& values) {
    for (int i = 0; i < 4; ++i) {
        if (values[i] + 1 != values[i + 1]) {
            return false;
        }
    }
    return true;
}

bool isFlush(const vector<int>& suits) {
    for (int i = 1; i < 5; ++i) {
        if (suits[i] != suits[0]) {
            return false;
        }
    }
    return true;
}

Hand evaluateHand(const vector<int>& cards) {
    vector<int> values(cards.size());
    vector<int> suits(cards.size());
    vector<int> hand_values(2);
    for (int i = 0; i < cards.size(); ++i) {
        values[i] = cardValue(cards[i]);
        suits[i] = cardSuit(cards[i]);
    }

    for (int i = 0; i < 2; i++) {
        hand_values[i] = cardValue(cards[i]);
    }


    sort(values.begin(), values.end());

    map<int, int> valueCounts;
    map<int, int> suitCounts;
    for (int v : values) {
        valueCounts[v]++;
    }
    for (int s : suits) {
        suitCounts[s]++;
    }

    bool flush = false;
    for (auto& suitCount : suitCounts) {
        if (suitCount.second >= 5) {
            flush = true;
            break;
        }
    }

    int straightHighestValue = -1;
    for (int i = values.size() - 5; i >= 0; --i) {
        if (isStraight(vector<int>(values.begin() + i, values.begin() + i + 5))) {
            straightHighestValue = i;
        }
    }

    if (straightHighestValue != -1 && flush) {
        return { STRAIGHT_FLUSH, straightHighestValue };
    }

    if (flush) {
        return { FLUSH, 0 };
    }

    if (straightHighestValue != -1) {
        return { STRAIGHT, straightHighestValue };
    }

    int countPairs = 0;
    for (int i = 0; i < values.size(); ++i) {
        int counts = valueCounts[i];
        if (counts >= 2) countPairs++;
    }

    for (int i = values.size() - 1; i >= 0; --i) {
        int counts = valueCounts[i];

        if (counts == 4) {
            return { FOUR_OF_A_KIND, i };
        }

        if (counts == 3 && countPairs > 1) {
            return { FULL_HOUSE, i };
        }

        if (counts == 3) {
            return { THREE_OF_A_KIND,i };
        }

        if (counts == 2 && countPairs > 1) {
            return { TWO_PAIR, i };
        }

        if (counts == 2) {
            return { ONE_PAIR, i };
        }
    }
    return { HIGH_CARD, max(hand_values[0], hand_values[0]) };
}

int compareHands(const Hand& hand1, const Hand& hand2) {

    if (hand1.rank != hand2.rank) {
        return hand1.rank > hand2.rank ? 1 : -1;
    }

    if (hand1.value != hand2.value) {
        return hand1.value > hand2.value ? 1 : -1;
    }

    return 0;
}

vector<int> who_won(vector<int> your_cards, vector<int> cards_on_table, vector<int> enemies_hand) {
    vector<int> your_full_hand = your_cards;
    your_full_hand.insert(your_full_hand.end(), cards_on_table.begin(), cards_on_table.end());

    vector<int> enemies_full_hand = enemies_hand;
    enemies_full_hand.insert(enemies_full_hand.end(), cards_on_table.begin(), cards_on_table.end());

    Hand your_best_hand = evaluateHand(your_full_hand);
    Hand enemies_best_hand = evaluateHand(enemies_full_hand);

    int result = compareHands(your_best_hand, enemies_best_hand);

    vector<int> out(2);

    if (result > 0) {
        out = {1, 0};
    } else if (result < 0) {
        out = {0, 1};
    } else {
        out = {0, 0};
    }
    return out;
}

/* Calculates number of iter for "make_combinations()" function. */
unsigned int calculate_num_of_iter(int unknown_cards_num, int known_cards_num) {
   /* Math formula to calculate num of iter is:

    X!/(X-n)!
    
    Where: X = DECK_SIZE - unknown_cards_num; n = known_cards_num */

    unsigned int total = 1;
    int cards_num = DECK_SIZE - known_cards_num;
    for (int i = 0; i < unknown_cards_num; i++) {
        total = total * (cards_num - i);
    }
    return total;
}

vector<int> make_combinations(int unknown_cards_num, unsigned int max_num_of_iterations, vector<int> cards, vector<int> players_cards, vector<int> __cards_on_table__) {
   
   /* Declarating all variables here */

    vector<int> out = {0,0};
    vector<int> cards_list(unknown_cards_num);
    vector<int> __who_won__(2);
    vector<int> cards_on_table(__cards_on_table__.size());
    vector<int> enemies_hand(2);
    int index;

    for (unsigned int i = 0; i < max_num_of_iterations; i++) {

       /* We rewrite "cards_on_table" because it's a vector that we later use in who_won() function. */
        cards_on_table = __cards_on_table__;

       /* The same situation as with "cards_on_table". */
        enemies_hand.clear();

       /* The method to convert "i" to index for "cards" list. */

        index = i;
        for (int j = 0; j < unknown_cards_num; j++) {
            cards_list[j] = cards[index % cards.size()];
            index /= cards.size();
        }

       /* "cards_list" mustn't have any repeated cards. */

        if (has_repeats(cards_list)) {
            continue;
        }

       /* Here we insert "cards_list[j]" into cards_on_table. We use __cards_on_table__.size(), not cards_on_table.size() */
       /* because __cards_on_table__ has constant size, and cards_on_table has dinamic size in for ().                    */

        for (int j = 0; j < 5 - __cards_on_table__.size(); j++) {

            cards_on_table.push_back(cards_list[j]);

        }

       /* Here we insert "cards_list[j]" into enemies_hand. We  start this for() with j equal to the end of the last for(). */

        for (int j = 5 - __cards_on_table__.size(); j < cards_list.size(); j++) {

            enemies_hand.push_back(cards_list[j]);

        }

       /* Here we calculate "__who_won__". out[0] is num of wins, and out[1] is num of loses. */

        __who_won__ = who_won(players_cards, cards_on_table, enemies_hand);
        out[0] += __who_won__[0];
        out[1] += __who_won__[1];
    }
    return out;
}

/* This functions converts input to card value. */
int convert(string str) {
    int out;
    string temp_str = str;
    temp_str = temp_str.substr(1, temp_str.size());
    out = stoi(temp_str);
    switch (str[0]) {
        case 'D':
            out += (13 * 0); break;
        case 'H':
            out += (13 * 1); break;
        case 'S':
            out += (13 * 2); break;
        case 'C':
            out += (13 * 3); break;
    }
    return out;
}

vector<int> make_cards(void) {

    vector<int> cards(DECK_SIZE);
    
    /* 1-13 - Diamonds, 14 - 26 - Hearts, 27 - 39 - Spades, 40 - 52 - Clubs */

    for (int i = 0; i < DECK_SIZE; i++) {
        cards[i] = i + 1;
    }
    return cards;
}

InputData make_input(void) {

    InputData data;

    vector<int> cards = make_cards();

    string temp_string;

    int num_of_opponents;

    std::cout << "Number of opponents:   ";

    /* 
        Increacing "num_of_opponents" can also increace the runtime. 
        Example input format: 1 
    */

    getline(cin, temp_string); 

    num_of_opponents = stoi(temp_string);

    string known_desk_cards_inp;

    std::cout << "Cards on the desk:     ";

    /* 
        You can enter any amount of cards, that is 5 or less.
        Example input format: D13 H13 S13 
    */

    getline(cin, known_desk_cards_inp); 

    string known_hand_cards_inp;
    
    std::cout << "Cards in your hand:    ";

    /*
        You must enter 2 cards here, because it's your hand.
        Example input format: S12 C12
    */

    getline(cin, known_hand_cards_inp);

    vector<string> temp_known_hand_cards_inp = splitItem(known_hand_cards_inp, ' ');

    vector<string> known_cards = splitItem(known_desk_cards_inp, ' ');

    known_cards.push_back(temp_known_hand_cards_inp[0]);
    known_cards.push_back(temp_known_hand_cards_inp[1]);

    vector<int> known_cards_list;

    for (int i = 0; i < known_cards.size(); i++) {
        known_cards_list.push_back(convert(known_cards[i]));
    }

    

    vector<int> known_hand_cards;
    vector<int> known_desk_cards;

    for (int i = 0; i < known_cards_list.size() - 2; i++) {
        known_desk_cards.push_back(known_cards_list[i]);
    }
    for (int i = known_cards_list.size() - 2; i < known_cards_list.size(); i++) {
        known_hand_cards.push_back(known_cards_list[i]);
    }

    cards = removeItem(cards, known_cards_list);

    data.known_hand_cards = known_hand_cards;
    data.known_desk_cards = known_desk_cards;
    data.cards = cards;
    data.cards_to_calculate = num_of_opponents * 2 + 5 - known_desk_cards.size();

    return data;
} 

int main() {

    InputData data = make_input();

    int max_iter = calculate_num_of_iter(data.cards_to_calculate, data.known_desk_cards.size() + 2);
    
    vector<int> out = make_combinations(
        data.cards_to_calculate,
        max_iter,
        data.cards,
        data.known_hand_cards,
        data.known_desk_cards
    );

    /* Your chance to win */

    std::cout << out[0] * 100 / (out[1] + out[0]) << '%' << std::endl;
    
    return 1; 
}
