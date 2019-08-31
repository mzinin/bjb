#include "fast_allocator.h"
#include "launcher.h"
#include "monom.h"
#include "resource_counter.h"
#include "version.h"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>


namespace
{
    const char OPTION_SHORT_PREFIX[] = "-";
    const char OPTION_LONG_PREFIX[] = "--";
    const char OPTION_VALUE_DELIMITER = '=';

    const char VERSION_SHORT_KEY   = 'v';
    const char VERSION_LONG_KEY[]  = "version";
    const char VERSION_USAGE_COMMENT[] = "Print version";

    const char HELP_SHORT_KEY  = 'h';
    const char HELP_LONG_KEY[] = "help";
    const char HELP_USAGE_COMMENT[] = "Print this message";

    const char STATISTICS_SHORT_KEY  = 's';
    const char STATISTICS_LONG_KEY[] = "collect-statistics";
    const char STATISTICS_USAGE_COMMENT[] = "Collect and print out statistics";

    const char PRINT_ANSWER_SHORT_KEY  = 'a';
    const char PRINT_ANSWER_LONG_KEY[] = "print-answer";
    const char PRINT_ANSWER_USAGE_COMMENT[] = "Print out constructed Groebner Basis";

    struct OptionValue
    {
        const std::string value;
        const std::string comment;
        void (SettingsManager::* const action)();

        OptionValue(const std::string& v, const std::string& c, void (SettingsManager::* const a)())
            : value(v)
            , comment(c)
            , action(a)
        {
        }
    };

    class CommandLineOption
    {
    public:
        CommandLineOption(const char shortKey,
                          const std::string& longKey,
                          const std::string& comment,
                          void (SettingsManager::* const action)())
            : shortKey_(shortKey)
            , longKey_(longKey)
            , comment_(comment)
            , action_(action)
        {
        }

        CommandLineOption(const std::string& longKey,
                          const std::string& comment,
                          const std::list<OptionValue>& values)
            : longKey_(longKey)
            , comment_(comment)
            , values_(values)
        {
        }

        bool detectKey(const std::string& option) const
        {
            if (values_.empty())
            {
                return longKey_ == option || (option.size() == 1 && option[0] == shortKey_);
            }

            std::size_t position = option.find(OPTION_VALUE_DELIMITER);
            std::string givenOption = option.substr(0, position);
            if (longKey_ != givenOption)
            {
                return false;
            }

            if (option.size() < position + 2)
            {
                std::cerr << "Option '" << OPTION_LONG_PREFIX << longKey_ << "' requires value, but didn't get one" << std::endl;
                return false;
            }

            std::string givenValue = option.substr(position + 1);
            chosenValue_ = values_.begin();
            for (; chosenValue_ != values_.end(); ++chosenValue_)
            {
                if (chosenValue_->value == givenValue)
                {
                    break;
                }
            }

            if (chosenValue_ == values_.end())
            {
                std::cerr << "Option '" << OPTION_LONG_PREFIX << longKey_ << "' got an unknown value '" << givenValue << "'." << std::endl;
                return false;
            }

            return true;
        }

        void activate() const
        {
            if (action_ && values_.empty())
            {
                (getSettingsManager().*action_)();
            }
            if (!values_.empty() && chosenValue_ != values_.end())
            {
                (getSettingsManager().*(chosenValue_->action))();
            }
        }

        void printHelp() const
        {
            if (values_.empty())
            {
                std::cout << OPTION_SHORT_PREFIX << shortKey_ << ", " << OPTION_LONG_PREFIX << longKey_ << std::endl;
                std::cout << "\t" << comment_ << ";" << std::endl;
            }
            else
            {
                std::cout << OPTION_LONG_PREFIX << longKey_ << OPTION_VALUE_DELIMITER << "<value>" << std::endl;
                std::cout << "\t" << comment_ << "," << std::endl;
                std::cout << "\tAdmissible values are:" << std::endl;

                size_t valueFieldWidth = 0;
                for (auto i = values_.begin(); i != values_.end(); ++i)
                {
                    if (i->comment.empty())
                    {
                        continue;
                    }
                    if (i->value.size() > valueFieldWidth)
                    {
                        valueFieldWidth = i->value.size();
                    }
                }
                for (auto i = values_.begin(); i != values_.end(); ++i)
                {
                    if (i->comment.empty())
                    {
                        continue;
                    }
                    std::cout << "\t\t" << std::setw(static_cast<int>(valueFieldWidth)) << std::left << i->value << " - ";
                    std::cout  << i->comment << (i == values_.end()-- ? "," : ";") << std::endl;
                }
            }
        }

    private:
        const char shortKey_ = 0;
        const std::string longKey_;
        const std::string comment_;
        void (SettingsManager::* const action_)() = nullptr;

        const std::list<OptionValue> values_;
        mutable std::list<OptionValue>::const_iterator chosenValue_ = values_.end();
    };

    std::list<CommandLineOption> commandLineOptions;
}

Launcher::Launcher()
{
    fillOptions();
}

bool Launcher::init(int argc, char* argv[])
{
    applicationName_ = argv[0];

    try
    {
        return analizeArguments(argc, argv);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Launcher::Init caught exception: " << error.what() << std::endl;
    }

    return false;
}

bool Launcher::run()
{
    try
    {
        if (getSettingsManager().printHelp())
        {
            printUsage();
            return true;
        }

        if (getSettingsManager().printVersion())
        {
            printVersion();
            return true;
        }

        if (inputFileName_.empty())
        {
            std::cerr << "Task file name is not defined.\n" << std::endl;
            printUsage();
            return false;
        }

        std::list<Polynom*> initialSet;
        std::list<Polynom*> initialAnswer;

        if (!getTaskFromFile(initialSet, initialAnswer))
        {
            return false;
        }

        GroebnerBasis gBasis;
        getResourceCounter().groebnerBasisTimer.start();
        gBasis.construct(initialSet);
        getResourceCounter().groebnerBasisTimer.stop();

        printResult(gBasis, initialAnswer);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Launcher::Run caught exception: " << error.what() << std::endl;
    }

    return true;
}

void Launcher::fillOptions()
{
    if (!commandLineOptions.empty())
    {
        return;
    }

    commandLineOptions.emplace_back(HELP_SHORT_KEY, HELP_LONG_KEY, HELP_USAGE_COMMENT, &SettingsManager::setPrintHelpEnabled);
    commandLineOptions.emplace_back(VERSION_SHORT_KEY, VERSION_LONG_KEY, VERSION_USAGE_COMMENT, &SettingsManager::setPrintVersionEnabled);
    commandLineOptions.emplace_back(STATISTICS_SHORT_KEY, STATISTICS_LONG_KEY, STATISTICS_USAGE_COMMENT, &SettingsManager::setCollectStatisticsEnabled);
    commandLineOptions.emplace_back(PRINT_ANSWER_SHORT_KEY, PRINT_ANSWER_LONG_KEY, PRINT_ANSWER_USAGE_COMMENT, &SettingsManager::setPrintAnswerEnabled);
}

void Launcher::printUsage() const
{
    std::cout << "Usage:" << std::endl;
    std::cout << "\t" << applicationName_ << " [options] <file_name.gnv> - execute given task;" << std::endl << std::endl;

    std::cout << "Options:" << std::endl;
    for (const auto& option : commandLineOptions)
    {
        option.printHelp();
        std::cout << std::endl;
    }
}

void Launcher::printVersion() const
{
    std::cout << "\t" << applicationName_ << std::endl;
    std::cout << "\tVersion " << currentVersion().major() << "." << currentVersion().minor() << "." << currentVersion().revision() << std::endl;
}

bool Launcher::analizeArguments(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cerr << "Arguments are missing." << std::endl << std::endl;
        printUsage();
        return false;
    }

    std::list<std::string> arguments;
    for (int i = 1; i < argc; ++i)
    {
        std::string currentArgument = std::string(argv[i]);

        if (currentArgument.size() > 2 && currentArgument.find(OPTION_LONG_PREFIX) == 0)
        {
            arguments.push_back(currentArgument.substr(strlen(OPTION_LONG_PREFIX)));
        }
        else if (currentArgument.size() > 1 && currentArgument.find(OPTION_SHORT_PREFIX) == 0)
        {
            for (auto p = strlen(OPTION_SHORT_PREFIX); p < currentArgument.size(); ++p)
            {
                arguments.push_back(std::string(1, currentArgument[p]));
            }
        }
        else if (i == argc - 1)
        {
            inputFileName_ = argv[i];
        }
        else
        {
            std::cerr << "'" << argv[i] << "' - task file name should be the last argument." << std::endl << std::endl;
            printUsage();
            return false;
        }
    }

    for (const auto& argument : arguments)
    {
        bool found = false;
        for (const auto& option : commandLineOptions)
        {
            if (option.detectKey(argument))
            {
                option.activate();
                found = true;
                break;
            }
        }

        if (!found)
        {
            std::cerr << "Unknown option: '" << argument << "'." << std::endl << std::endl;
            printUsage();
            return false;
        }
    }

    return true;
}

bool Launcher::getTaskFromFile(std::list<Polynom*>& initialSet, std::list<Polynom*>& initialAnswer) const
{
    std::ifstream inputFileStream(inputFileName_);
    if (!inputFileStream)
    {
        std::cerr << "No such file: '" << inputFileName_ << "'." << std::endl;
        return false;
    }

    readVariables(inputFileStream);
    readPolynomList(inputFileStream, initialSet);
    readPolynomList(inputFileStream, initialAnswer);

    return true;
}

void Launcher::readVariables(std::ifstream& inputFileStream) const
{
    char c = '0';
    std::string tmpString;
    while (c != ';' && !inputFileStream.eof())
    {
        inputFileStream >> c;
        if (c == ',' || c == ';')
        {
            Monom::addVariable(tmpString);
            tmpString.clear();
        }
        else
        {
            tmpString += c;
        }
    }
}

void Launcher::readPolynomList(std::ifstream& inputFileStream, std::list<Polynom*>& list) const
{
    clearPolynomList(list);

    std::stringstream tmpStream;
    char c = '0';
    std::string tmpString;
    while (c != ';' && !inputFileStream.eof())
    {
        inputFileStream >> c;
        if (c == ',' || c == ';')
        {
            tmpStream << tmpString << std::endl;
            tmpString.clear();
        }
        else
        {
            tmpString += c;
        }
    }

    Polynom tmpPolynom;
    while (!tmpStream.eof())
    {
        tmpStream >> tmpPolynom;
        list.push_back(new Polynom(tmpPolynom));
    }
}

void Launcher::clearPolynomList(std::list<Polynom*>& list) const
{
    for (auto* polynom : list)
    {
        delete polynom;
    }
    list.clear();
}

void Launcher::printResult(GroebnerBasis& gBasis, std::list<Polynom*>& initialAnswer) const
{
    if (!gBasis.length())
    {
        return;
    }

    if (getSettingsManager().printAnswer())
    {
        std::cout << gBasis << std::endl;
    }

    if (getSettingsManager().collectStatistics())
    {
        getResourceCounter().printFullStatistics(std::cout);
    }
    else
    {
        getResourceCounter().printBriefStatistics(std::cout);
    }

    if (checkAnswer(gBasis, initialAnswer))
    {
        std::cout << "The answer is CORRECT" << std::endl << std::endl;
    }
    else
    {
        std::cout << "The answer is WRONG" << std::endl << std::endl;
    }
}

bool Launcher::checkAnswer(GroebnerBasis& gBasis, std::list<Polynom*>& initialAnswer) const
{
    if (gBasis.length() != initialAnswer.size())
    {
        return false;
    }
    else
    {
        for (const auto* asnwerItem : initialAnswer)
        {
            bool foundMatch = false;
            for (unsigned i = 0; i < gBasis.length(); ++i)
            {
                if (gBasis[i] == *asnwerItem)
                {
                    foundMatch = true;
                    break;
                }
            }
            if (!foundMatch)
            {
                std::cout << "The mistake is here " << *asnwerItem << std::endl;
                return false;
            }
        }
    }
    return true;
}
