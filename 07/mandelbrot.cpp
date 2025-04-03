#include <ncurses.h>
#include <complex>
#include <vector>

const int MAX_ITER = 1000;
const std::string charset = " .:-=+*#%@"; 

int mandelbrot(std::complex<double> c) {
    std::complex<double> z = 0;
    int iter = 0;
    while (std::abs(z) <= 2.0 && iter < MAX_ITER) {
        z = z * z + c;
        ++iter;
    }
    return iter;
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    cols /= 3;

    double min_re = -2.0, max_re = 1.0;
    double min_im = -1.0;
    double aspect_ratio = static_cast<double>(cols) / rows;
    double max_im = min_im + (max_re - min_re) / aspect_ratio;
    double re_step = (max_re - min_re) / cols;
    double im_step = (max_im - min_im) / rows;

    for (int y = 0; y < rows; ++y) {
        double im = max_im - y * im_step;
        int px = 0;
        for (int x = 0; x < cols; x++) {
            double re = min_re + x * re_step;
            int value = mandelbrot(std::complex<double>(re, im));
            int char_idx = static_cast<int>((static_cast<double>(value) / MAX_ITER) * (charset.size() - 1));
            mvaddch(y, px++, charset[char_idx]);
            mvaddch(y, px++, charset[char_idx]);
        }
    }
    refresh();
    getch();
    endwin();
}
