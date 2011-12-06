#ifndef LAYOUT_H
#define LAYOUT_H

#include <QObject>

class Layout
{
public:
    enum LayoutType {
        General = 0,
        Url,
        Email,
        Number,
        PhoneNumber,
        Common
    };

    enum LayoutOrientation {
        Landscape,
        Portrait
    };

    Layout(LayoutType type, LayoutOrientation orientation);

    LayoutType type() const;
    LayoutOrientation orientation() const;

    bool operator==(const Layout& other) const;

private:
    Q_DISABLE_COPY(Layout)

    const LayoutType mType;
    const LayoutOrientation mOrientation;
};

#endif // LAYOUT_H
