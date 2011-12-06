#include "layout.h"

Layout::Layout(LayoutType type, LayoutOrientation orientation)
    : mType(type),
      mOrientation(orientation)
{
}

Layout::LayoutType Layout::type() const
{
    return mType;
}

Layout::LayoutOrientation Layout::orientation() const
{
    return mOrientation;
}

bool Layout::operator==(const Layout& other) const
{
    return mType == other.mType && mOrientation == other.mOrientation;
}
