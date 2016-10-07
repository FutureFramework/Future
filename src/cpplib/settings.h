#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QFileSystemWatcher;

namespace iotlib {

class SettingsData;
/**
 * @brief The Settings class provides access to JSON config file with live reload
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(const QString &fileName, QObject *parent = 0);
    ~Settings();

    QVariant get(const QString &key) const;
    /**
     * @brief beginGroupSet stops writing data to json file
     * Do not forget to call @a endGroup()
     */
    void beginGroupSet();
    void set(const QString &key, const QVariant &value);
    void endGroupSet();

    QVariant operator[] (const QString &key);

signals:
    void settingsChanged();

private slots:
    void onFileChanged();

private:
    SettingsData *d;
};

} // iotlib

#endif // SETTINGS_H
