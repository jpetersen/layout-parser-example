#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QString>

class Keyboard
{
public:
    Keyboard(const QString &version, const QString &title, const QString &language);
    ~Keyboard();

    const QString version() const;
    const QString title() const;
    const QString language() const;

private:
    const QString mVersion;
    const QString mTitle;
    const QString mLanguage;
};

#endif // KEYBOARD_H
