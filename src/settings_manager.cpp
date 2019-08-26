#include "settings_manager.h"


void SettingsManager::setCollectStatisticsEnabled()
{
    collectStatistics_ = true;
}

void SettingsManager::setCollectStatisticsDisabled()
{
    collectStatistics_ = false;
}

void SettingsManager::setPrintAnswerEnabled()
{
    printAnswer_ = true;
}

void SettingsManager::setPrintAnswerDisabled()
{
    printAnswer_ = false;
}

void SettingsManager::setPrintVersionEnabled()
{
    printVersion_ = true;
}

void SettingsManager::setPrintVersionDisabled()
{
    printVersion_ = false;
}

void SettingsManager::setPrintHelpEnabled()
{
    printHelp_ = true;
}

void SettingsManager::setPrintHelpDisabled()
{
    printHelp_ = false;
}

SettingsManager& getSettingsManager()
{
    static SettingsManager manager;
    return manager;
}
