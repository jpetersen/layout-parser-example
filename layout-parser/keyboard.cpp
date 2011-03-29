#include "keyboard.h"

Keyboard::Keyboard(const QString &version, const QString &title, const QString &language,
                   const QString &catalog, const bool autocapitalization)
    : mVersion(version.isEmpty() ? QString::fromLatin1("1.0") : version),
      mTitle(title),
      mLanguage(language),
      mCatalog(catalog),
      mAutocapitalization(autocapitalization)
{
}

Keyboard::~Keyboard()
{
}

const QString Keyboard::version() const
{
    return mVersion;
}

const QString Keyboard::title() const
{
    return mTitle;
}

const QString Keyboard::language() const
{
    return mLanguage;
}

const QString Keyboard::catalog() const
{
    return mCatalog;
}

bool Keyboard::autocapitalization() const
{
    return mAutocapitalization;
}
