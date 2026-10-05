#include "app.h"

int main() {
    SimpleShapeApplication app(WINDOW_WIDTH, WINDOW_HEIGHT, PROJECT_NAME, true, 1);
    app.run(1);
    return 0;
}