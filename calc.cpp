#include <gtkmm.h>
#include <sstream>
#include <cmath>

typedef std::string str;

str toDisplay(double n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

str formatResult(double result) {
    std::ostringstream oss;
    double abs_result = std::abs(result);
    if (abs_result >= 1000000000) oss << "Sending: " << result / 1000000000 << "B$ to Israel";
    else if (abs_result >= 1000000) oss << "Sending: " << result / 1000000 << "M$ to Israel";
    else if (abs_result >= 1000) oss << "Sending: " << result / 1000 << "K$ to Israel";
    else oss << "Sending: " << result << "$ to Israel";
    return oss.str();
}

class CalcWindow : public Gtk::Window {
public:
    CalcWindow() {
        set_title("Freedom Calculator");
        set_default_size(420, 500);

        grid.set_row_spacing(8);
        grid.set_column_spacing(8);
        grid.set_border_width(12);

        exprLabel.set_xalign(1.0);
        resultLabel.set_text("0");
        resultLabel.set_xalign(1.0);

        grid.attach(exprLabel,   0, 0, 4, 1);
        grid.attach(resultLabel, 0, 1, 4, 1);

        addBtn("AC",  0, 2); addBtn("+/-", 1, 2);
        addBtn("%",   2, 2); addBtn("/",   3, 2);

        addBtn("7", 0, 3); addBtn("8", 1, 3);
        addBtn("9", 2, 3); addBtn("*", 3, 3);

        addBtn("4", 0, 4); addBtn("5", 1, 4);
        addBtn("6", 2, 4); addBtn("-", 3, 4);

        addBtn("1", 0, 5); addBtn("2", 1, 5);
        addBtn("3", 2, 5); addBtn("+", 3, 5);

        auto* btn0 = Gtk::make_managed<Gtk::Button>("0");
        btn0->set_size_request(160, 70);
        btn0->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &CalcWindow::onButton), str("0")));
        grid.attach(*btn0, 0, 6, 2, 1);

        addBtn(".", 2, 6);
        addBtn("=", 3, 6);

        add(grid);
        show_all_children();
    }

private:
    Gtk::Grid  grid;
    Gtk::Label exprLabel, resultLabel;

    str    currentInput = "";
    double storedNumber = 0;
    str    pendingOp    = "";
    bool   freshResult  = false;

    void addBtn(const str& label, int col, int row) {
        auto* btn = Gtk::make_managed<Gtk::Button>(label);
        btn->set_size_request(80, 70);
        btn->signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this, &CalcWindow::onButton), label)
        );
        grid.attach(*btn, col, row, 1, 1);
    }

    void onButton(str label) {
        if (label == "AC") {
            currentInput = ""; storedNumber = 0;
            pendingOp = ""; freshResult = false;
            exprLabel.set_text("");
            resultLabel.set_text("0");
            return;
        }
        if (label == "+/-") {
            if (!currentInput.empty()) {
                if (currentInput[0] == '-') currentInput = currentInput.substr(1);
                else currentInput = "-" + currentInput;
                resultLabel.set_text(currentInput);
            }
            return;
        }
        if (label == "%") {
            if (!currentInput.empty()) {
                currentInput = std::to_string(std::stod(currentInput) / 100.0);
                resultLabel.set_text(currentInput);
            }
            return;
        }
        if (label == "+" || label == "-" || label == "*" || label == "/") {
            if (!currentInput.empty()) storedNumber = std::stod(currentInput);
            pendingOp   = label;
            freshResult = true;
            exprLabel.set_text(toDisplay(storedNumber) + " " + label);
            return;
        }
        if (label == "=") {
            if (pendingOp.empty() || currentInput.empty()) return;
            double n2 = std::stod(currentInput), result = 0;
            if      (pendingOp == "+") result = storedNumber + n2;
            else if (pendingOp == "-") result = storedNumber - n2;
            else if (pendingOp == "*") result = storedNumber * n2;
            else if (pendingOp == "/") {
                if (n2 == 0) { resultLabel.set_text("Error: div by 0"); return; }
                result = storedNumber / n2;
            }
            exprLabel.set_text(toDisplay(storedNumber) + " " + pendingOp + " " + toDisplay(n2) + " =");
            resultLabel.set_text(formatResult(result));
            currentInput = std::to_string(result);
            pendingOp = ""; freshResult = true;
            return;
        }
        if (label == ".") {
            if (freshResult) { currentInput = "0"; freshResult = false; }
            if (currentInput.find('.') == str::npos) currentInput += ".";
            resultLabel.set_text(currentInput);
            return;
        }
        if (freshResult) { currentInput = ""; freshResult = false; }
        if (currentInput == "0") currentInput = label;
        else currentInput += label;
        resultLabel.set_text(currentInput);
    }
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.calc");
    CalcWindow window;
    return app->run(window);
}
