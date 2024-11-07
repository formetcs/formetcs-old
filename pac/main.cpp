#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>

#include "codegenerator.h"

//#include "abstypes.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("pac");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCommandLineParser clp;
    clp.setApplicationDescription("PlanPro-to-ABS compiler");
    clp.addHelpOption();
    clp.addVersionOption();
    clp.addPositionalArgument("file", "PlanPro XML input file.");

    QCommandLineOption verboseOption("verbose", "Verbose mode.");
    clp.addOption(verboseOption);

    QCommandLineOption skipOption("skip-objects", "Skip all PlanPro object types unless not explicitly added.");
    clp.addOption(skipOption);

    QCommandLineOption addOption("a", "Add PlanPro object type <type>.", "type");
    clp.addOption(addOption);

    QCommandLineOption removeOption("r", "Remove PlanPro object type <type>.", "type");
    clp.addOption(removeOption);

    QCommandLineOption outputFileOption("o", "Write generated ABS data into <file>.", "file");
    clp.addOption(outputFileOption);

    QCommandLineOption levelOption({"l", "level"}, "Generate code for ETCS Level <lv>.", "lv");
    clp.addOption(levelOption);

    clp.process(app);
    const QStringList posArgs = clp.positionalArguments();
    QString inputFile = QString();
    if(posArgs.length() < 1)
    {
        std::cerr << "Error: No input file" << std::endl;
        return 1;
    }
    else if(posArgs.length() > 1)
    {
        std::cerr << "Error: More than one input file" << std::endl;
        return 1;
    }
    else
    {
        inputFile = posArgs[0];
    }
    QString outputFile = clp.value(outputFileOption);
    if(outputFile.isEmpty())
    {
        outputFile = "output.abs";
    }
    QString etcsLevel = clp.value(levelOption);
    if(etcsLevel.isEmpty())
    {
        etcsLevel = "2";
    }

    QStringList addList = clp.values(addOption);
    QStringList removeList = clp.values(removeOption);
    bool skipDefault = clp.isSet(skipOption);

    if(clp.isSet(verboseOption))
    {
        std::cout << "inputFile=" <<  qPrintable(inputFile) <<  std::endl;
        std::cout << "outputFile=" << qPrintable(outputFile) << std::endl;
        std::cout << "skipDefault=" << skipDefault << std::endl;
        std::cout << "addList:" << std::endl;
        for(int i = 0; i < addList.count(); i++)
        {
            std::cout << "   " << qPrintable(addList.at(i)) << std::endl;
        }
        std::cout << "removeList:" << std::endl;
        for(int i = 0; i < removeList.count(); i++)
        {
            std::cout << "   " << qPrintable(removeList.at(i)) << std::endl;
        }
    }

    bool resultcode = false;
    CodeGenerator gen;
    resultcode = gen.loadInputFile(inputFile);
    if(!resultcode)
    {
        std::cerr << "Error: Input file " << qPrintable(inputFile) << " could not be loaded" << std::endl;
        return 1;
    }
    resultcode = gen.generateCode(etcsLevel, addList, removeList, skipDefault);
    if(!resultcode)
    {
        std::cerr << "Error: ABS code could not be generated" << std::endl;
        return 1;
    }
    resultcode = gen.writeOutputFile(outputFile);
    if(!resultcode)
    {
        std::cerr << "Error: Output file " << qPrintable(outputFile) << " could not be written" << std::endl;
        return 1;
    }

    //return app.exec();
    return 0;
}
