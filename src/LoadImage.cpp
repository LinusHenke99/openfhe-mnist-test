#include "LoadImage.h"


std::vector<double> load_image(std::string pathToFile) {
    std::fstream fin(pathToFile);

    std::vector<double> img;
    std::string line, temp, entry;

    while (fin >> temp) {
        getline(fin, line);

        std::stringstream s(line);

        while (getline(s, entry, ';')) {
            img.push_back(std::stod(entry));
        }

    }

    fin.close();
    return img;
}
