//
// Created by frehml on 30/04/2021.
//

#include "l_system_2d.h"

img::EasyImage LSystem::generateImage(const ini::Configuration &configuration, img::Color &background_color, int &size) {
    img::Color line_color;
    make_color(line_color, configuration["2DLSystem"]["color"].as_double_tuple_or_die());

    LParser::LSystem2D l_system;
    std::ifstream input_stream(configuration["2DLSystem"]["inputfile"]);
    input_stream >> l_system;
    input_stream.close();

    Lines2D lines = drawLSystem(l_system, line_color);
    return draw2DLines(lines, size, background_color);
}

Lines2D LSystem::drawLSystem(LParser::LSystem2D &l_system, img::Color &color){
    Lines2D lines;
    Point2D point1(0, 0);
    Point2D point2(0, 0);
    std::list<std::pair<std::pair<Point2D, Point2D>, double>> bracket_stack;

    double cur_angle = l_system.get_starting_angle();
    double add_angle = l_system.get_angle();
    std::string sequence = calculateSequence(l_system, l_system.get_initiator(), l_system.get_nr_iterations());

    for (char c : sequence){
        if (c == '+')
            cur_angle += add_angle;
        else if (c == '-')
            cur_angle -= add_angle;
        else if (c == '(')
            bracket_stack.push_front({{point1, point2}, cur_angle});
        else if (c == ')'){
            point1 = bracket_stack.front().first.first;
            point2 = bracket_stack.front().first.second;
            cur_angle = bracket_stack.front().second;
            bracket_stack.pop_front();
        }
        else{
            point1 = point2;
            point2 = Point2D(point2.x+cos(degToRad(cur_angle)), point2.y+sin(degToRad(cur_angle)));
            if(l_system.draw(c))
                lines.push_back(Line2D(point1, point2, color));
        }
    }

    return lines;
}

std::string LSystem::calculateSequence(LParser::LSystem2D &l_system, std::string sequence, unsigned int itterations){
    if(itterations == 0){return sequence;}
    std::string new_sequence;

    for (char c : sequence){
        if (c == '+' || c == '-' || c == '(' || c == ')')
            new_sequence += c;
        else{
            new_sequence += l_system.get_replacement(c);
        }
    }
    return calculateSequence(l_system, new_sequence, itterations-1);
}