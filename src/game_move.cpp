#include<cassert>

#include"game_move.hpp"

atomic_move::atomic_move(void)noexcept:
x(0),
y(0),
on(),
off(),
every_on_legal(false),
no_off(false){}

atomic_move::atomic_move(int _x,int _y,std::set<token>&& _on,std::set<token>&& _off):
x(_x),
y(_y),
on(std::move(_on)),
off(std::move(_off)),
every_on_legal(false),
no_off(false){}

atomic_move::atomic_move(int _x,int _y,std::set<token>&& _on_off, bool on_switch):
x(_x),
y(_y),
on(on_switch ? std::move(_on_off) : std::set<token>()),
off(on_switch ? std::set<token>() : std::move(_on_off)),
every_on_legal(!on_switch),
no_off(on_switch){}

atomic_move::atomic_move(int _x,int _y):
x(_x),
y(_y),
on(),
off(),
every_on_legal(true),
no_off(true){}

parser_result<atomic_move> parse_atomic_move(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    messages_container& msg)throw(message){
    if(!it.has_value())
        return failure<atomic_move>();
    int x=0,y=0;
    parser_result<int> delta = parse_int(it,msg);
    if(delta.is_success()){
        x = delta.get_value();
        if(!it.has_value())
            throw msg.build_message("Unexpected end of atomic move");
        if(it.current().get_type() != comma)
            throw msg.build_message(it.create_call_stack("Expected ',', encountered \'"+it.current().to_string()+"\'"));
        if(!it.next(msg))
            throw msg.build_message("Unexpected end of atomic move");
        delta = parse_int(it,msg);
        if(delta.is_success())
            y = delta.get_value();
        else
            throw msg.build_message(it.create_call_stack("Expected integer, encountered \'"+it.current().to_string()+"\'"));
        if(!it.has_value())
            throw msg.build_message("Unexpected end of atomic move");
        if(it.current().get_type() != comma)
            return success(atomic_move(x,y));
        if(!it.next(msg))
            throw msg.build_message("Unexpected end of atomic move");
    }
    else if(it.current().get_type() != slash&&it.current().get_type() != identifier&&it.current().get_type() != left_curly_bracket)
        return failure<atomic_move>();
    bool every_on_legal = false;
    bool no_off = false;
    std::set<token> on,off;
    if(it.current().get_type() == slash){
        every_on_legal = true;
        if(!it.next(msg))
            throw msg.build_message("Unexpected end of atomic move");
        parser_result<std::set<token>> legals = parse_tokens_set(it,msg);
        if(legals.is_success())
            off = legals.move_value();
        else
            throw msg.build_message(it.create_call_stack("Expected set of identifiers, encountered \'"+it.current().to_string()+"\'"));
    }
    else{
        parser_result<std::set<token>> legals = parse_tokens_set(it,msg);
        if(legals.is_success())
            on = legals.move_value();
        else
            throw msg.build_message(it.create_call_stack("Expected set of identifiers or '/', encountered \'"+it.current().to_string()+"\'"));
        if(it.has_value()&&it.current().get_type() == slash){
            if(!it.next(msg))
                throw msg.build_message("Unexpected end of atomic move");
            legals = parse_tokens_set(it,msg);
            if(legals.is_success())
                off = legals.move_value();
            else
                throw msg.build_message(it.create_call_stack("Expected set of identifiers, encountered \'"+it.current().to_string()+"\'"));
        }
        else
            no_off = true;
    }
    encountered_pieces.insert(on.begin(),on.end());
    encountered_pieces.insert(off.begin(),off.end());
    return success(
        every_on_legal ?
            atomic_move(x,y,std::move(off),false) :
            (no_off ?
                atomic_move(x,y,std::move(on),true) :
                atomic_move(x,y,std::move(on),std::move(off))));
}

bool atomic_move::operator<(const atomic_move& m)const{
    return x<m.x
        || (x==m.x && y<m.y)
        || (x==m.x && y==m.y && every_on_legal<m.every_on_legal)
        || (x==m.x && y==m.y && every_on_legal==m.every_on_legal && no_off<m.no_off)
        || (x==m.x && y==m.y && every_on_legal==m.every_on_legal && no_off==m.no_off && on<m.on)
        || (x==m.x && y==m.y && every_on_legal==m.every_on_legal && no_off==m.no_off && on==m.on && off<m.off);
}

bool atomic_move::operator==(const atomic_move& m)const{
    return (x==m.x && y==m.y && every_on_legal==m.every_on_legal && no_off==m.no_off && on==m.on && off==m.off);
}

bool atomic_move::is_goal_eligible(void)const{
    return no_off;
}

std::ostream& operator<<(std::ostream& out,const atomic_move& m){
    out<<m.x<<','<<m.y;
    if(!m.every_on_legal||!m.no_off)
        out<<',';
    if(!m.every_on_legal)
        out<<m.on;
    if(!m.no_off)
        out<<'/'<<m.off;
    return out;
}

std::ostream& operator<<(std::ostream& out,const std::set<token>& s){
    out<<'{';
    if(!s.empty()){
        auto it = s.begin();
        out<<it->to_string();
        while((++it)!=s.end())
            out<<','<<it->to_string();
    }
    out<<'}';
    return out;
}

atomic_move atomic_move::flatten(void){
    auto result = std::move(*this);
    on.clear();
    off.clear();
    return result;
}

turn_change_indicator::turn_change_indicator(const token& name)noexcept:
player(name){
}

turn_change_indicator::turn_change_indicator(void)noexcept:
player(){}//dummy

parser_result<turn_change_indicator> parse_turn_change_indicator(
    slice_iterator& it,
    const game_order& players,
    int player_number,
    messages_container& msg)throw(message){
    if(!it.has_value())
        return failure<turn_change_indicator>();
    if(it.current().get_type() != left_square_bracket)
        return failure<turn_change_indicator>();
    if(!it.next(msg))
        throw msg.build_message("Unexpected end of square brackets enclosed player name (player \'"+players.get_player_name(player_number,0).to_string()+"\')");
    turn_change_indicator result;
    if(it.current().get_type() == identifier){
        if(!players.exists(it.current()))
            throw msg.build_message(it.create_call_stack("There is no player \'"+it.current().to_string()+"\'"));
        result = turn_change_indicator(it.current());
        it.next(msg);
    }
    else{
        parser_result<int> delta_result = parse_int(it,msg);
        if(delta_result.is_success())
            result = turn_change_indicator(players.get_player_name(player_number,delta_result.get_value()));
        else
            throw msg.build_message(it.create_call_stack("Expected player name or integer, encountered \'"+it.current().to_string()+"\'"));
    }
    if(!it.has_value())
        throw msg.build_message("Unexpected end of square brackets enclosed player name (player \'"+players.get_player_name(player_number,0).to_string()+"\')");
    if(it.current().get_type() != right_square_bracket)
        throw msg.build_message(it.create_call_stack("Expected \']\', encountered \'"+it.current().to_string()+"\'"));
    it.next(msg);
    return success<turn_change_indicator>(std::move(result));
}

bool turn_change_indicator::operator<(const turn_change_indicator& m)const{
    return player<m.player;
}

bool turn_change_indicator::operator==(const turn_change_indicator& m)const{
    return player==m.player;
}

std::ostream& operator<<(std::ostream& out,const turn_change_indicator& m){
    out<<'['<<m.player.to_string()<<']';
    return out;
}

turn_change_indicator turn_change_indicator::flatten(void){
    auto result = std::move(*this);
    player = token();
    return result;
}

bracketed_move::bracketed_move(void)noexcept:
repetition_number(1),
sum(nullptr),
tag(0){}

bracketed_move::bracketed_move(const bracketed_move& src)noexcept:
repetition_number(src.repetition_number),
sum(nullptr),
tag(src.tag){
    switch(src.tag){
    case 0:
        sum = new moves_sum(*src.sum);
        break;
    case 1:
        atomic = new atomic_move(*src.atomic);
        break;
    default:
        turn_changer = new turn_change_indicator(*src.turn_changer);
    }
}

bracketed_move& bracketed_move::operator=(const bracketed_move& src)noexcept{
    if(this != &src){
        repetition_number = src.repetition_number;
        switch(tag){
        case 0:
            delete sum;
            break;
        case 1:
            delete atomic;
            break;
        default:
            delete turn_changer;
        }
        tag = src.tag;
        switch(src.tag){
        case 0:
            sum = new moves_sum(*src.sum);
            break;
        case 1:
            atomic = new atomic_move(*src.atomic);
            break;
        default:
            turn_changer = new turn_change_indicator(*src.turn_changer);
        }
    }
    return *this;
}

bracketed_move::bracketed_move(bracketed_move&& src)noexcept:
repetition_number(src.repetition_number),
sum(),
tag(src.tag){
    switch(src.tag){
    case 0:
        sum = src.sum;
        src.sum = nullptr;
        break;
    case 1:
        atomic = src.atomic;
        src.atomic = nullptr;
        break;
    default:
        turn_changer = src.turn_changer;
        src.turn_changer = nullptr;
    }
}

bracketed_move& bracketed_move::operator=(bracketed_move&& src)noexcept{
    if(this != &src){
        repetition_number = src.repetition_number;
        switch(tag){
        case 0:
            delete sum;
            break;
        case 1:
            delete atomic;
            break;
        default:
            delete turn_changer;
        }
        tag = src.tag;
        switch(src.tag){
        case 0:
            sum = src.sum;
            break;
        case 1:
            atomic = src.atomic;
            break;
        default:
            turn_changer = src.turn_changer;
        }
        src.tag = 0;
        src.sum = nullptr;
    }
    return *this;
}

bracketed_move::bracketed_move(moves_sum&& src)noexcept:
repetition_number(1),
tag(0){
    sum = new moves_sum(std::move(src));
}

bracketed_move::bracketed_move(atomic_move&& src)noexcept:
repetition_number(1),
tag(1){
    atomic = new atomic_move(std::move(src));
}

bracketed_move::bracketed_move(turn_change_indicator&& src)noexcept:
repetition_number(1),
tag(2){
    turn_changer = new turn_change_indicator(std::move(src));
}

bracketed_move::~bracketed_move(void)noexcept{
    switch(tag){
    case 0:
        delete sum;
        break;
    case 1:
        delete atomic;
        break;
    default:
        delete turn_changer;
    }
}

parser_result<bracketed_move> parse_bracketed_move(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const game_order& players,
    int player_number,
    bool& contains_turn_changer,
    messages_container& msg)throw(message){
    if(!it.has_value())
        return failure<bracketed_move>();
    parser_result<turn_change_indicator> turn_changer_result = parse_turn_change_indicator(it,players,player_number,msg);
    if(turn_changer_result.is_success()){
        contains_turn_changer = true;
        return success(bracketed_move(turn_changer_result.move_value()));
    }
    else if(it.current().get_type() == left_round_bracket){
        if(!it.next(msg))
            throw msg.build_message("Unexpected end of brackets enclosed move (player \'"+players.get_player_name(player_number,0).to_string()+"\')");
        bracketed_move result;
        bool sum_contains_turn_changer = false;
        parser_result<moves_sum> sum_result = parse_moves_sum(it,encountered_pieces,players,player_number,sum_contains_turn_changer,msg);
        if(sum_result.is_success())
            result = sum_result.move_value();
        else{
            parser_result<atomic_move> atomic_result = parse_atomic_move(it,encountered_pieces,msg);
            if(atomic_result.is_success())
                result = atomic_result.move_value();
            else
                throw msg.build_message(it.create_call_stack("Expected atomic move or moves sum, encountered \'"+it.current().to_string()+"\'"));
        }
        if(!it.has_value())
            throw msg.build_message("Unexpected end of brackets enclosed move");
        if(it.current().get_type() != right_round_bracket)
            throw msg.build_message(it.create_call_stack("Expected \')\', encountered \'"+it.current().to_string()+"\'"));
        if(it.next(msg)&&it.current().get_type()==caret){
            if(!it.next(msg))
                throw msg.build_message("Unexpected end of repetition indicator (player \'"+players.get_player_name(player_number,0).to_string()+"\')");
            slice_iterator fallback_it(it);
            if(it.current().get_type()==star){
                if(sum_contains_turn_changer)
                    throw msg.build_message(fallback_it.create_call_stack("Moves containing turn changer cannot be repeated more than once"));
                result.set_star();
                it.next(msg);
            }
            else{
                slice_iterator before_int_it(it);
                parser_result<int> rn_result = parse_int(it,msg);
                if(!rn_result.is_success())
                    throw msg.build_message(it.create_call_stack("Expected \'*\' or positive number, encountered \'"+it.current().to_string()+"\'"));
                if(rn_result.get_value()<=0)
                    throw msg.build_message(before_int_it.create_call_stack("Number of repetition must be positive"));
                if(rn_result.get_value()>1&&sum_contains_turn_changer)
                    throw msg.build_message(fallback_it.create_call_stack("Moves containing turn changer cannot be repeated more than once"));
                result.set_repetition_number(rn_result.get_value());
            }
        }
        contains_turn_changer = sum_contains_turn_changer;
        return success(std::move(result));
    }
    else
        return failure<bracketed_move>();
}

bool bracketed_move::operator<(const bracketed_move& m)const{
    if(tag<m.tag)
        return true;
    else if(tag>m.tag)
        return false;
    else if(repetition_number<m.repetition_number)
        return true;
    else if(repetition_number>m.repetition_number)
        return false;
    switch(tag){
    case 0:
        return sum==nullptr||(sum!=m.sum&&m.sum!=nullptr&&*sum<*m.sum);
    case 1:
        return atomic==nullptr||(atomic!=m.atomic&&m.atomic!=nullptr&&*atomic<*m.atomic);
    default:
        return turn_changer==nullptr||(turn_changer!=m.turn_changer&&m.turn_changer!=nullptr&&*turn_changer<*m.turn_changer);
    }
}

bool bracketed_move::operator==(const bracketed_move& m)const{
    if(tag!=m.tag||repetition_number!=m.repetition_number)
        return false;
    switch(tag){
    case 0:
        return sum==m.sum || (sum!=nullptr&&m.sum!=nullptr&&*sum==*m.sum);
    case 1:
        return atomic==m.atomic || (atomic!=nullptr&&m.atomic!=nullptr&&*atomic==*m.atomic);
    default:
        return turn_changer==m.turn_changer || (turn_changer!=nullptr&&m.turn_changer!=nullptr&&*turn_changer==*m.turn_changer);
    }
}

bool bracketed_move::is_goal_eligible(void)const{
    switch(tag){
    case 0:
        return sum->is_goal_eligible();
    case 1:
        return atomic->is_goal_eligible();
    default:
        return false;
    }
}

void bracketed_move::set_repetition_number(uint rn){
    assert(rn>0);
    repetition_number = rn;
}

void bracketed_move::set_star(void){
    repetition_number = 0;
}

void bracketed_move::print_rbg(std::ostream& out,uint recurrence_depth)const{
    switch(tag){
    case 0:
        out<<"(\n    ";
        sum->print_rbg(out,recurrence_depth+1);
        print_spaces(out,4*recurrence_depth);
        out<<')';
        break;
    case 1:
        out<<"("<<(*atomic)<<')';
        break;
    default:
        out<<(*turn_changer);
    }
    if(repetition_number==0)
        out<<"^*";
    else if(repetition_number>1)
        out<<'^'<<repetition_number;
}

bracketed_move bracketed_move::flatten(void){
    switch(tag){
    case 0:
        {auto result = sum->flatten();
        delete sum;
        sum = nullptr;
        return moves_sum(std::move(result));}
    case 1:
        {auto result = atomic->flatten();
        delete atomic;
        atomic = nullptr;
        return atomic_move(std::move(result));}
    default:
        {auto result = turn_changer->flatten();
        delete turn_changer;
        turn_changer = nullptr;
        return turn_change_indicator(std::move(result));}
    }
}

bool bracketed_move::is_single_concatenation(void)const{
    if(tag!=0||sum==nullptr)
        return false;
    return sum->is_single_concatenation();
}

moves_concatenation bracketed_move::give_single_concatenation(void){
    assert(is_single_concatenation());
    return sum->give_single_concatenation();
}

bool bracketed_move::is_single_sum(void)const{
    return tag==0 && sum!=nullptr;
}

moves_sum bracketed_move::give_single_sum(void){
    assert(is_single_sum());
    moves_sum result = std::move(*sum);
    delete sum;
    sum = nullptr;
    return result;
}

moves_concatenation::moves_concatenation(void)noexcept:
content(){}

moves_concatenation::moves_concatenation(std::vector<bracketed_move>&& src)noexcept:
content(std::move(src)){}

parser_result<moves_concatenation> parse_moves_concatenation(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const game_order& players,
    int player_number,
    bool& contains_turn_changer,
    messages_container& msg)throw(message){
    if(!it.has_value())
        return failure<moves_concatenation>();
    std::vector<bracketed_move> result;
    bool brackets_contains_turn_changer = false;
    parser_result<bracketed_move> brackets_result = parse_bracketed_move(it,encountered_pieces,players,player_number,brackets_contains_turn_changer,msg);
    contains_turn_changer |= brackets_contains_turn_changer;
    if(brackets_result.is_success())
        result.push_back(brackets_result.move_value());
    else
        return failure<moves_concatenation>();
    while(true){
        brackets_contains_turn_changer = false;
        slice_iterator fallback_it(it);
        brackets_result = parse_bracketed_move(it,encountered_pieces,players,player_number,brackets_contains_turn_changer,msg);
        if(!brackets_result.is_success())
            return success(moves_concatenation(std::move(result)));
        else{
            if(contains_turn_changer)
                throw msg.build_message(fallback_it.create_call_stack("No move can appear after turn changer in concatenation"));
            result.push_back(brackets_result.move_value());
            contains_turn_changer |= brackets_contains_turn_changer;
        }
    }
}

bool moves_concatenation::operator<(const moves_concatenation& m)const{
    if(content.size()<m.content.size())
        return true;
    else if(content.size()>m.content.size())
        return false;
    else{
        for(uint i=0;i<content.size();++i){
            if(content[i]<m.content[i])
                return true;
            else if(m.content[i]<content[i])
                return false;
        }
        return false;
    }
}

bool moves_concatenation::operator==(const moves_concatenation& m)const{
    if(content.size()!=m.content.size())
        return false;
    else{
        for(uint i=0;i<content.size();++i)
            if(!(content[i]==m.content[i]))
                return false;
        return true;
    }
}

bool moves_concatenation::is_goal_eligible(void)const{
    for(const auto& el: content)
        if(!el.is_goal_eligible())
            return false;
    return true;
}

void moves_concatenation::print_rbg(std::ostream& out,uint recurrence_depth)const{
    for(const auto& el: content)
        el.print_rbg(out,recurrence_depth);
}

moves_concatenation moves_concatenation::flatten(void){
    std::vector<std::pair<std::vector<bracketed_move>::iterator,std::vector<bracketed_move>>> moves_stack;
    moves_stack.push_back(make_pair(content.begin(),std::move(content)));
    moves_stack.back().first = moves_stack.back().second.begin(); // inelegant way to avoid undefined behavior
    content.clear();
    std::vector<bracketed_move> result;
    while(!moves_stack.empty()){
        if(moves_stack.back().first==moves_stack.back().second.end())
            moves_stack.pop_back();
        else if(moves_stack.back().first->is_single_concatenation()){
            moves_concatenation next_level = moves_stack.back().first->give_single_concatenation();
            ++moves_stack.back().first;
            moves_stack.push_back(make_pair(next_level.content.begin(),std::move(next_level.content)));
            moves_stack.back().first = moves_stack.back().second.begin();
        }
        else{
            result.push_back(moves_stack.back().first->flatten());
            ++moves_stack.back().first;
        }
    }
    return moves_concatenation(std::move(result));
}

bool moves_concatenation::is_single_sum(void)const{
    if(content.size()!=1)
        return false;
    return content[0].is_single_sum();
}
moves_sum moves_concatenation::give_single_sum(void){
    assert(is_single_sum());
    return content[0].give_single_sum();
}

moves_sum::moves_sum(void)noexcept:
content(){}

moves_sum::moves_sum(std::set<moves_concatenation>&& src)noexcept:
content(std::move(src)){}

parser_result<moves_sum> parse_moves_sum(
    slice_iterator& it,
    std::set<token>& encountered_pieces,
    const game_order& players,
    int player_number,
    bool& contains_turn_changer,
    messages_container& msg)throw(message){
    if(!it.has_value())
        return failure<moves_sum>();
    std::set<moves_concatenation> result;
    bool concatenation_contains_turn_changer = false;
    parser_result<moves_concatenation> concat_result = parse_moves_concatenation(it,encountered_pieces,players,player_number,concatenation_contains_turn_changer,msg);
    contains_turn_changer |= concatenation_contains_turn_changer;
    if(concat_result.is_success())
        result.insert(concat_result.move_value());
    else
        return failure<moves_sum>();
    while(it.has_value() && it.current().get_type() == plus){
        if(!it.next(msg))
            throw msg.build_message("Unexpected end of move sum (player \'"+players.get_player_name(player_number,0).to_string()+"\')");
        concatenation_contains_turn_changer = false;
        concat_result = parse_moves_concatenation(it,encountered_pieces,players,player_number,concatenation_contains_turn_changer,msg);
        contains_turn_changer |= concatenation_contains_turn_changer;
        if(!concat_result.is_success())
            throw msg.build_message(it.create_call_stack("Expected moves concatenation, encountered \'"+it.current().to_string()+"\'"));
        else
            result.insert(concat_result.move_value());
    }
    return success(moves_sum(std::move(result)));
}

bool moves_sum::operator<(const moves_sum& m)const{
    if(content.size()<m.content.size())
        return true;
    else if(content.size()>m.content.size())
        return false;
    else{
        for(auto i1=content.begin(), i2=m.content.begin();i1!=content.end();++i1,++i2){
            if(*i1 < *i2)
                return true;
            if(*i2 < *i1)
                return false;
        }
        return false;
    }
}

bool moves_sum::operator==(const moves_sum& m)const{
    if(content.size() != m.content.size())
        return false;
    else{
        for(auto i1=content.begin(), i2=m.content.begin();i1!=content.end();++i1,++i2)
            if(!(*i1 == *i2))
                return false;
        return true;
    }
}

bool moves_sum::is_goal_eligible(void)const{
    for(const auto& el: content)
        if(!el.is_goal_eligible())
            return false;
    return true;
}

void moves_sum::print_rbg(std::ostream& out,uint recurrence_depth)const{
    if(!content.empty()){
        auto it = content.begin();
        print_spaces(out,4*(recurrence_depth-1));
        it->print_rbg(out,recurrence_depth);
        out<<'\n';
        while((++it)!=content.end()){
            print_spaces(out,4*recurrence_depth-2);
            out<<"+ ";
            it->print_rbg(out,recurrence_depth);
            out<<'\n';
        }
    }
}

moves_sum moves_sum::flatten(void){
    std::vector<std::set<moves_concatenation>> moves_stack;
    moves_stack.push_back(std::move(content));
    content.clear();
    std::set<moves_concatenation> result;
    while(!moves_stack.empty()){
        auto it = moves_stack.back().begin();
        if(moves_stack.back().empty())
            moves_stack.pop_back();
        else if(it->is_single_sum()){
            moves_concatenation mc = std::move(*it); // IS IT SAFE?!? DOESN'T SEEM SO
            moves_stack.back().erase(it);
            moves_sum next_level = mc.give_single_sum();
            moves_stack.push_back(std::move(next_level.content));
        }
        else{
            moves_concatenation mc = std::move(*it); // IS IT SAFE?!? DOESN'T SEEM SO
            moves_stack.back().erase(it);
            result.insert(mc.flatten());
        }
    }
    return moves_sum(std::move(result));
}

bool moves_sum::is_single_concatenation(void)const{
    return content.size()==1;
}

moves_concatenation moves_sum::give_single_concatenation(void){
    assert(is_single_concatenation());
    auto it = content.begin();
    auto result = std::move(*it);
    content.clear();
    return result;
}

void print_spaces(std::ostream& out,uint n){
    for(uint i=0;i<n;++i)
        out<<' ';
}
