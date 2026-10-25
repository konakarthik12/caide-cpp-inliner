//                        Caide C++ inliner
//
// This file is distributed under the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at your
// option) any later version. See LICENSE.TXT for details.

#include "../optimizer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::ifstream;
using std::istringstream;
using std::ofstream;
using std::string;
using std::vector;

static vector<string> readNonEmptyLines(const string& filePath) {
    vector<string> lines;
    ifstream file{filePath.c_str()};
    if (!file)
        throw std::runtime_error("File not found: " + filePath);

    string line;
    while (std::getline(file, line)) {
        if (line.find_first_not_of(" \t\r\n") != string::npos)
            lines.push_back(line);
    }
    return lines;
}

static bool isWhitespaceOnly(const string& text) {
    return text.find_first_not_of(" \t\r") == string::npos;
}

static void removeEmptyLines(const string& textInBinaryMode,
                             int maxConsequentEmptyLines,
                             const string& outputFilePath)
{
    if (maxConsequentEmptyLines < 0)
        maxConsequentEmptyLines = std::numeric_limits<int>::max();

    istringstream in{textInBinaryMode};
    ofstream out{outputFilePath, std::ios::binary};
    if (!out)
        throw std::runtime_error("Could not write output file: " + outputFilePath);

    int currentConsequentEmptyLines = 0;
    bool readNonEmptyLine = false;
    string line;
    while (std::getline(in, line)) {
        if (isWhitespaceOnly(line))
            ++currentConsequentEmptyLines;
        else {
            currentConsequentEmptyLines = 0;
            readNonEmptyLine = true;
        }

        if (readNonEmptyLine && currentConsequentEmptyLines <= maxConsequentEmptyLines)
            out << line << '\n';
    }
}

static void appendOption(vector<string>& options, const string& option) {
    if (!option.empty() && option[0] == '@') {
        vector<string> fileOptions = readNonEmptyLines(option.substr(1));
        options.insert(options.end(), fileOptions.begin(), fileOptions.end());
    } else {
        options.push_back(option);
    }
}

static vector<string> defaultMacrosToKeep() {
    return {
        "__cplusplus", "__STDC_VERSION__",
        "__ANDROID__", "__MINGW32__", "__MINGW64__",
        "_WIN32", "_WIN64", "_M_AMD64", "__linux", "__linux__", "__APPLE__",
        "__GNUC__", "__GLIBC__", "__clang__", "_MSC_VER"
    };
}

static void printUsage(const char* argv0) {
    std::cerr
        << "Usage: " << argv0 << " [options] <inlined.cpp> <result.cpp> [-- <clang option>...]\n"
        << "Options:\n"
        << "  --clang-options-file <file>  Read clang options, one per non-empty line\n"
        << "  --keep-macro <name>          Preserve macro in optimizer output\n"
        << "  --keep-identifier <name>     Preserve fully-qualified identifier\n"
        << "  --max-empty-lines <n>        Maximum consecutive empty lines, default 2\n";
}

int main(int argc, char* argv[]) {
    try {
        vector<string> positional;
        vector<string> clangOptions;
        vector<string> macrosToKeep = defaultMacrosToKeep();
        vector<string> identifiersToKeep;
        int maxConsequentEmptyLines = 2;

        int i = 1;
        for (; i < argc; ++i) {
            const string arg = argv[i];
            if (arg == "--") {
                ++i;
                break;
            } else if (arg == "--clang-options-file") {
                if (++i >= argc)
                    throw std::runtime_error("--clang-options-file requires a file path");
                vector<string> fileOptions = readNonEmptyLines(argv[i]);
                clangOptions.insert(clangOptions.end(), fileOptions.begin(), fileOptions.end());
            } else if (arg == "--keep-macro") {
                if (++i >= argc)
                    throw std::runtime_error("--keep-macro requires a macro name");
                macrosToKeep.push_back(argv[i]);
            } else if (arg == "--keep-identifier") {
                if (++i >= argc)
                    throw std::runtime_error("--keep-identifier requires an identifier");
                identifiersToKeep.push_back(argv[i]);
            } else if (arg == "--max-empty-lines") {
                if (++i >= argc)
                    throw std::runtime_error("--max-empty-lines requires a number");
                maxConsequentEmptyLines = std::strtol(argv[i], nullptr, 10);
            } else {
                positional.push_back(arg);
            }
        }

        for (; i < argc; ++i)
            appendOption(clangOptions, argv[i]);

        if (positional.size() != 2) {
            printUsage(argv[0]);
            return 1;
        }

        caide::internal::Optimizer optimizer{clangOptions, macrosToKeep, identifiersToKeep};
        string onlyReachableCode = optimizer.doOptimize(positional[0]);
        removeEmptyLines(onlyReachableCode, maxConsequentEmptyLines, positional[1]);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
