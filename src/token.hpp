#ifndef TOKEN
#define TOKEN

#include<string>
#include<vector>
#include<set>

#include"message.hpp"

enum token_type{
    left_round_bracket=0,
    right_round_bracket,
    left_curly_bracket,
    right_curly_bracket,
    left_square_bracket,
    right_square_bracket,
    tilde,
    hash,
    minus,
    plus,
    caret,
    star,
    double_plus,
    comma,
    slash,
    dollar,
    equal,
    less,
    less_equal,
    greater,
    greater_equal,
    logical_and,
    logical_or,
    logical_not,
    def,
    game,
    player,
    board,
    order,
    goal,
    number,
    identifier, // must third-to-last
    quotation
};

class token{
        uint position_in_file;
        token_type type;
        union{
            std::string* contained_string;
            uint number_value;
        };
        token(void)=delete;
        token(uint position, token_type type);
    public:
        token(const token& src);
        token(token&& src);
        token& operator=(const token& src);
        token& operator=(token&& src);
        ~token(void);

        friend std::set<token> create_tokens_set(uint position);
        friend token create_quotation(uint position);
        std::string to_string(void)const;
        uint get_value(void)const;
        token_type get_type(void)const;
        bool could_be_next_character(char next_character, uint position)const;
        bool contains_exactly_n_chars(uint n)const;
        void take_next_character(char next_character, messages_container& msg)throw(message);
        uint get_position(void)const;
        token& operator+=(const token& t)throw(std::string);

        bool operator==(const token& t)const;
        bool operator<(const token& t)const;
};

std::set<token> create_tokens_set(uint position);
token create_quotation(uint position);
uint seek_end_of_quotation(const std::string& input, uint current_char, token& quote, messages_container& msg);
uint seek_end_of_singleline_comment(const std::string& input, uint current_char, messages_container& msg);
uint seek_end_of_multiline_comment(const std::string& input, uint current_char, messages_container& msg);
uint skip_whitespace(const std::string& input, uint current_char, messages_container& msg);
bool filter_set_with_character(std::set<token>& current_set, char next_character, uint position, messages_container& msg)throw(message); // returns whether char should be re-parsed and current_set contains full parsed token
std::vector<token> tokenize(const std::string& input, messages_container& msg)throw(message);

#endif
