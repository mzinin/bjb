#pragma once


class Launcher;

class SettingsManager
{
public:
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    bool collectStatistics() const;
    bool printAnswer() const;
    bool printVersion() const;
    bool printHelp() const;

private:
    SettingsManager() = default;

    void setCollectStatisticsEnabled();
    void setCollectStatisticsDisabled();

    void setPrintAnswerEnabled();
    void setPrintAnswerDisabled();

    void setPrintVersionEnabled();
    void setPrintVersionDisabled();

    void setPrintHelpEnabled();
    void setPrintHelpDisabled();

    friend SettingsManager& getSettingsManager();
    friend class Launcher;

private:
    bool collectStatistics_ = false;
    bool printAnswer_ = false;
    bool printVersion_ = false;
    bool printHelp_ = false;
};

SettingsManager& getSettingsManager();


inline bool SettingsManager::collectStatistics() const
{
    return collectStatistics_;
}

inline bool SettingsManager::printAnswer() const
{
    return printAnswer_;
}

inline bool SettingsManager::printVersion() const
{
    return printVersion_;
}

inline bool SettingsManager::printHelp() const
{
    return printHelp_;
}
