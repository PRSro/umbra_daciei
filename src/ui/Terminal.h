#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <vector>

namespace ui {

class Terminal {
public:
    Terminal();
    ~Terminal() = default;

    void init();
    void update(float dt);
    void render();

    void print(const std::string& text);
    void clear();

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    void toggle() { visible_ = !visible_; }

    void executeCommand(const std::string& command);
    std::vector<std::string> getHistory() const { return history_; }

private:
    std::vector<std::string> lines_;
    std::vector<std::string> history_;
    std::string currentInput_;
    bool visible_ = false;
};

}

#endif