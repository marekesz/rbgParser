#include"assignments.hpp"

assignment::assignment(token&& left_side, token&& right_side):
left_side(std::move(left_side)),
right_side(std::move(right_side)){
}

parser_result<token> parse_left_side_variable(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message){
    parsing_context_string_guard g(&it, "Unexpected end of input while parsing variable");
    if(!it.has_value() || it.current(msg).get_type()!=dollar)
        return failure<token>();
    it.next(msg);
    if(it.current(msg).get_type()!=identifier && it.current(msg).get_type()!=player)
        throw msg.build_message(it.create_call_stack("Expected variable or \'player\' token, encountered \'"+it.current(msg).to_string()+"\'"));
    if(it.current(msg).get_type()!=player && decls.get_legal_variables().count(it.current(msg))==0 && decls.get_legal_players().count(it.current(msg))==0)
        throw msg.build_message(it.create_call_stack("Undeclared variable or player \'"+it.current(msg).to_string()+"\'"));
    token result = it.current(msg);
    it.next(msg);
    return success(std::move(result));
}

parser_result<assignment> parse_single_assignment(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message){
    auto variable_result = parse_left_side_variable(it,decls,msg);
    if(!variable_result.is_success())
        return failure<assignment>();
    if(it.current(msg).get_type() != equal)
        throw msg.build_message(it.create_call_stack("Expected \'=\' after variable name, encountered \'"+it.current(msg).to_string()+"\'"));
    it.next(msg);
    if(variable_result.get_value().get_type() == identifier){
        if(it.current(msg).get_type() != number)
            throw msg.build_message(it.create_call_stack("Expected non-negative number after variable name, encountered \'"+it.current(msg).to_string()+"\'"));
        token num = it.current(msg);
        it.next(msg);
        return success(assignment(variable_result.move_value(),std::move(num)));
    }
    else{ // player assignment
        if(it.current(msg).get_type() != identifier)
            throw msg.build_message(it.create_call_stack("Expected player name after variable name, encountered \'"+it.current(msg).to_string()+"\'"));
        token name = it.current(msg);
        if(decls.get_legal_players().count(name)==0)
            throw msg.build_message(it.create_call_stack("Undeclared player \'"+it.current(msg).to_string()+"\'"));
        it.next(msg);
        return success(assignment(variable_result.move_value(),std::move(name)));
    }
}

parser_result<concatenation> parse_assignments(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message){
    parsing_context_string_guard g(&it, "Unexpected end of input while parsing assignemnts");
    concatenation result;
    auto assignment_result = parse_single_assignment(it,decls,msg);
    if(!assignment_result.is_success())
        return failure<concatenation>();
    result.add_move(std::unique_ptr<game_move>(new assignment(assignment_result.move_value())));
    while(it.has_value() && it.current(msg).get_type() == comma){
        it.next(msg);
        auto assignment_result = parse_single_assignment(it,decls,msg);
        if(!assignment_result.is_success())
            throw msg.build_message(it.create_call_stack("Expected variable assignment after comma, encountered \'"+it.current(msg).to_string()+"\'"));
        result.add_move(std::unique_ptr<game_move>(new assignment(assignment_result.move_value())));
    }
    return success(std::move(result));
}
