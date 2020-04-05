#ifndef ENVIRONMENTPROXY_H_
#define ENVIRONMENTPROXY_H_

#include "Common.h"
#include <algorithm>
#include <tuple>
#include <vector>

class EnvironmentProxy {
public:
    EnvironmentProxy(int numberOfArgs, char** args);
    virtual ~EnvironmentProxy();

    void printUsage();
    bool numberOfBitsExtractedOK(const std::string&& numberOfBits);
    bool parseInputArguments();

    std::string getInFile();
    std::string getOutFile();
    std::string getType();
    std::string getMethod();
    std::string getSecret();
    int getUsedBits();

    void showImages();
    void addImageWindow(CImg<unsigned char>& image, std::string title);
    void printArgs();

    void addToJson(const std::string& name, const std::string& entry);
private:
    int argc;
    char** argv;
    std::string inputFile;
    std::string outputFile;
    std::string operation;
    std::string method;
    std::string secret;
    int usedBits;

    std::vector<std::tuple<CImg<unsigned char>&, std::string>> windows;
};

#endif /* ENVIRONMENTPROXY_H_ */
