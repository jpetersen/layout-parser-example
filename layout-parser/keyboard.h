#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QString>

class Keyboard
{
public:
    Keyboard(const QString &version, const QString &title, const QString &language, const QString &catalog, const bool autocapitalization);
    ~Keyboard();

    const QString version() const;
    const QString title() const;
    const QString language() const;
    const QString catalog() const;
    bool autocapitalization() const;

private:
    const QString mVersion;
    const QString mTitle;
    const QString mLanguage;
    const QString mCatalog;
    const bool mAutocapitalization;
};

#endif // KEYBOARD_H
