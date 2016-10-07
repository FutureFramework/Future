#include "settings.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QJsonDocument>

#include <QDebug>

class iotlib::SettingsData
{
public:
    SettingsData() : skipReload(false), groupSet(false) { }

    void load();
    void save();

    iotlib::Settings *q;

    QFileSystemWatcher *fileWatcher;
    QVariantMap settings;
    QString fileName;
    bool skipReload;
    bool groupSet;
};

void iotlib::SettingsData::load()
{
    if (skipReload) {
        skipReload = false;
        return;
    }

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << fileName << "removed from disk?";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (doc.isNull()) {
        qWarning() << "JSON parsing error";
        return;
    }

    QVariantMap newSettings = doc.toVariant().toMap();
    if (settings == newSettings)
        return;
    settings = newSettings;
    qInfo() << "Loaded settings from" << fileName;
    emit q->settingsChanged();
}

void iotlib::SettingsData::save()
{
    if (groupSet)
        return;

    QFile f(fileName);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Can't open" << fileName << "for writing";
        return;
    }

    f.write(QJsonDocument::fromVariant(settings).toJson());
    f.close();

    qInfo() << "Saved settings to" << fileName;
    emit q->settingsChanged();
}

iotlib::Settings::Settings(const QString &fileName, QObject *parent) : QObject(parent)
{
    d = new SettingsData;
    d->q = this;

    if (!QFile::exists(fileName)) {
        QFile f(fileName);
        f.open(QIODevice::WriteOnly);
        f.close();
    }

    d->fileName = fileName;

    d->fileWatcher = new QFileSystemWatcher(this);
    d->fileWatcher->addPath(fileName);
    connect(d->fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &Settings::onFileChanged);

    d->load();
}

iotlib::Settings::~Settings()
{
    if (d)
        delete d;
}

QVariant iotlib::Settings::get(const QString &key) const
{
    return d->settings.value(key);
}

void iotlib::Settings::beginGroupSet()
{
    d->groupSet = true;
}

void iotlib::Settings::set(const QString &key, const QVariant &value)
{
    d->settings[key] = value;
    d->save();
}

void iotlib::Settings::endGroupSet()
{
    d->groupSet = false;
}

QVariant iotlib::Settings::operator[](const QString &key)
{
    return d->settings.value(key);
}

void iotlib::Settings::onFileChanged()
{
    d->load();
}
