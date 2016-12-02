#ifndef GAME_MOVE
#define GAME_MOVE

#include<set>
#include<map>
#include<vector>

#include"token.hpp"
#include"parser_helpers.hpp"
#include"slice_iterator.hpp"
#include"message.hpp"

class atomic_move{
        int x;
        int y;
        bool every_on_legal;
        std::set<token> on;
        std::set<token> off;
    public:
        atomic_move(void);
        atomic_move(int x,int y,const std::set<token>& on,const std::set<token>& off);
        atomic_move(int x,int y,const std::set<token>& off); // true -> on, false -> off

        friend parser_result<atomic_move> parse_atomic_move(
            slice_iterator& it,
            std::set<token>& encountered_pieces,
            messages_container& msg)throw(message);

        bool operator<(const atomic_move& m)const;
        bool operator==(const atomic_move& m)const;
};

parser_result<atomic_move> parse_atomic_move(slice_iterator& it, messages_container& msg)throw(message);

class turn_change_indicator{
        token player;
        turn_change_indicator(const token& name);
    public:
        turn_change_indicator(void);

        friend parser_result<turn_change_indicator> parse_turn_change_indicator(
            slice_iterator& it,
            const std::map<token,slice>& players,
            messages_container& msg)throw(message);

        bool operator<(const turn_change_indicator& m)const;
        bool operator==(const turn_change_indicator& m)const;
};

parser_result<turn_change_indicator> parse_turn_change_indicator(
    slice_iterator& it,
    const std::map<token,slice>& players,
    messages_container& msg)throw(message);

class moves_sum;

class bracketed_move{
        union{
            moves_sum* sum;
            atomic_move* atomic;
            turn_change_indicator* turn_changer;
        };
        uint tag : 2; // 0 -> sum, 1 -> atomic, else -> turn_changer
        bracketed_move(const moves_sum& src);
        bracketed_move(const atomic_move& src);
        bracketed_move(const turn_change_indicator& src);
    public:
        bracketed_move(void);
        bracketed_move(const bracketed_move& src);
        bracketed_move& operator=(const bracketed_move& src);
        bracketed_move(bracketed_move&& src);
        bracketed_move& operator=(bracketed_move&& src);
        ~bracketed_move(void);

        friend parser_result<bracketed_move> parse_bracketed_move(
            slice_iterator& it,
            std::set<token>& encountered_pieces,
            const std::map<token,slice>& players,
            messages_container& msg)throw(message);

        bool operator<(const bracketed_move& m)const;
        bool operator==(const bracketed_move& m)const;
};

parser_result<bracketed_move> parse_bracketed_move(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const std::map<token,slice>& players,
    messages_container& msg)throw(message);

class moves_concatenation{
        std::vector<bracketed_move> content;
        moves_concatenation(std::vector<bracketed_move>&& src);
    public:
        moves_concatenation(void);

        friend parser_result<moves_concatenation> parse_moves_concatenation(
            slice_iterator& it,
            std::set<token>& encountered_pieces,
            const std::map<token,slice>& players,
            messages_container& msg)throw(message);

        bool operator<(const moves_concatenation& m)const;
        bool operator==(const moves_concatenation& m)const;
};

parser_result<moves_concatenation> parse_moves_concatenation(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const std::map<token,slice>& players,
    messages_container& msg)throw(message);

class moves_sum{
        std::set<moves_concatenation> content;
        moves_sum(std::set<moves_concatenation>&& src);
    public:
        moves_sum(void);

        friend parser_result<moves_sum> parse_moves_sum(
            slice_iterator& it,
            std::set<token>& encountered_pieces,
            const std::map<token,slice>& players,
            messages_container& msg)throw(message);

        bool operator<(const moves_sum& m)const;
        bool operator==(const moves_sum& m)const;
};

parser_result<moves_sum> parse_moves_sum(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const std::map<token,slice>& players,
    messages_container& msg)throw(message);

#endif