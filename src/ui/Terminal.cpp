#include "Terminal.h"
#include <iostream>

namespace ui {
Terminal::Terminal() {}
void Terminal::init() {}
void Terminal::update(float) {}
void Terminal::render() {}

void Terminal::print(const std::string& text) {
    lines_.push_back(text);
    history_.push_back(text);
}

void Terminal::clear() {
    lines_.clear();
}

void Terminal::executeCommand(const std::string& command) {
    history_.push_back("> " + command);
    print("Command executed: " + command);
}
}