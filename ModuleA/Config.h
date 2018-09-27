#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

#include <QtCore/QObject>
#include <QtCore/QtGlobal>

class Config : public QObject
{
    Q_OBJECT

  public:
    Config() { }
    static Config *instance();

    void init(std::string file);

    bool lookup(std::string key, std::string& val);
    std::string get(std::string key);

  private:
    std::map<std::string, std::string> m_configParams;
};

#endif
