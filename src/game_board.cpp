#include"game_board.hpp"
#include"gdl_constants.hpp"

game_board::game_board(uint w, uint h)noexcept:
width(w),
height(h),
currently_filled(0),
initial_position(h,std::vector<token>(w)){
}

bool game_board::is_completely_filled(void)const{
    return width*height==currently_filled;
}

void game_board::add_piece(const token& piece){
    //if(currently_filled%width==0)
    //    initial_position.push_back(std::vector<token>());
    //initial_position.back().push_back(piece);
    initial_position[height-(currently_filled/width)-1][currently_filled%width] = piece;
    ++currently_filled;
}

void game_board::fill_with_slice(slice_iterator& it, std::set<token>& encountered_pieces, messages_container& msg)throw(message){
    while(!is_completely_filled()){
        if(!it.has_value()){
            int left = width*height-currently_filled;
            throw msg.build_message("Unexpected end of \'board\' directive (expected "+std::to_string(left)+" token"+(left>1?"s":"")+" more)");
        }
        if(it.current().get_type() != identifier)
            throw msg.build_message(it.create_call_stack("Expected identifier, encountered \'"+it.current().to_string()+"\'"));
        else{
            add_piece(it.current());
            encountered_pieces.insert(it.current());
        }
        it.next(msg);
    }
}

uint game_board::get_height(void)const{
    return height;
}

uint game_board::get_width(void)const{
    return width;
}

std::ostream& operator<<(std::ostream& out,const game_board& brd){
    out<<"#board "<<brd.width<<' '<<brd.height<<'\n';
    for(uint i=brd.height;i>0;--i){
        out<<"    ";
        for(uint j=0;j<brd.width;++j){
            out<<brd.initial_position[i-1][j].to_string();
            if(j<brd.width)
                out<<' ';
        }
        out<<'\n';
    }
    return out;
}

void game_board::print_files_and_ranks(std::ostream& out,const options& o)const{
    if(o.printing_comments())
        out<<section_title("Board definition")<<'\n';
    for(uint i=0;i<width;++i)
        out<<"(file "+std::to_string(i)+")\n";
    out<<'\n';
    for(uint i=0;i<height;++i)
        out<<"(rank "+std::to_string(i)+")\n";
    out<<'\n';
}

void game_board::print_initial_setting(std::ostream& out)const{
    for(uint i=0;i<height;++i)
        for(uint j=0;j<width;++j)
            out<<"(init (cell "<<j<<' '<<i<<' '<<initial_position[i][j].to_string()<<"))\n";
    out<<'\n';
}
