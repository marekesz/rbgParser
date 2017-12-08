#include"negatable_condition.hpp"
#include"move_condition.hpp"
#include"alternative.hpp"

negatable_condition::negatable_condition(std::unique_ptr<condition> content, bool modifier=false):
content(std::move(content)),
modifier(modifier){
}

parser_result<negatable_condition> parse_negatable_condition(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message){
    parsing_context_string_guard g(&it, "Unexpected end of input while parsing condition");
    if(!it.has_value())
        return failure<negatable_condition>();
    bool started = false;
    bool modifier = true;
    std::unique_ptr<condition> result;
    if(it.current(msg).get_type() == logical_not){
        started = true;
        modifier = false;
        it.next(msg);
    }
    if(it.current(msg).get_type() == left_round_bracket){
        it.next(msg);
        auto move_result = parse_move_condition(it,decls,msg);
        if(move_result.is_success())
            result = std::unique_ptr<condition>(new move_condition(move_result.move_value()));
        else{
        auto alternative_result = parse_alternative(it,decls,msg);
        if(alternative_result.is_success())
            result = std::unique_ptr<condition>(new alternative(alternative_result.move_value()));
        else
            throw msg.build_message(it.create_call_stack("Expected move pattern (starting with \'!\') or alternative, encountered \'"+it.current(msg).to_string()+"\'"));
        }
        if(it.current(msg).get_type() != right_round_bracket)
            throw msg.build_message(it.create_call_stack("Expected \')\', encountered \'"+it.current(msg).to_string()+"\'"));
        it.next(msg);
        return success(negatable_condition(std::move(result),modifier));
    }
    else{
        throw msg.build_message(it.create_call_stack("Not implemented yet"));
        // TODO: porownania
        return success(negatable_condition(std::move(result),modifier));
    }
}