#ifndef PLOTVALUE_H
#define PLOTVALUE_H

#include <QColor>
#include <QSettings>
#include <QString>

#include "datapoint.h"
#include "qcustomplot.h"

#define METERS_TO_FEET 3.28084
#define MPS_TO_MPH     2.23694
#define MPS_TO_KMH     3.6

class PlotValue: public QObject
{
    Q_OBJECT

public:
    typedef enum
    {
        Metric = 0,
        Imperial
    } Units;

    PlotValue(bool visible, QColor color): mVisible(visible), mColor(color) {}

    virtual const QString title() const = 0;
    virtual const QString title(Units units) const = 0;

    void setColor(const QColor &color) { mColor = color; }
    const QColor &color() const { return mColor; }

    virtual double value(const DataPoint &dp, Units units) const = 0;

    QCPAxis *addAxis(QCustomPlot *plot, Units units)
    {
        mAxis = plot->axisRect()->addAxis(QCPAxis::atLeft);
        mAxis->setLabelColor(color());
        mAxis->setTickLabelColor(color());
        mAxis->setBasePen(QPen(color()));
        mAxis->setTickPen(QPen(color()));
        mAxis->setSubTickPen(QPen(color()));
        mAxis->setLabel(title(units));
        return mAxis;
    }
    QCPAxis *axis() const { return mAxis; }

    bool visible() const { return mVisible; }
    void setVisible(bool visible) { mVisible = visible; }

    void readSettings()
    {
        QSettings settings("FlySight", "Viewer");
        settings.beginGroup("plotValue/" + key());
        mVisible = settings.value("visible", mVisible).toBool();
        mColor = settings.value("color", mColor).value<QColor>();
        settings.endGroup();
    }

    void writeSettings() const
    {
        QSettings settings("FlySight", "Viewer");
        settings.beginGroup("plotValue/" + key());
        settings.setValue("visible", mVisible);
        settings.setValue("color", mColor);
        settings.endGroup();
    }

    virtual bool hasOptimal() const { return false; }

private:
    bool     mVisible;
    QColor   mColor;
    QCPAxis *mAxis;

    const QString key() const
    {
        return metaObject()->className();
    }
};

class PlotElevation: public PlotValue
{
    Q_OBJECT

public:
    PlotElevation(): PlotValue(true, Qt::black) {}
    const QString title() const
    {
        return tr("Elevation");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (m)");
        else                 return title() + tr(" (ft)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::elevation(dp);
        else                 return DataPoint::elevation(dp) * METERS_TO_FEET;
    }

    bool hasOptimal() const { return true; }
};

class PlotVerticalSpeed: public PlotValue
{
    Q_OBJECT

public:
    PlotVerticalSpeed(): PlotValue(false, Qt::green) {}
    const QString title() const
    {
        return tr("Vertical Speed");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (km/h)");
        else                 return title() + tr(" (mph)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::verticalSpeed(dp) * MPS_TO_KMH;
        else                 return DataPoint::verticalSpeed(dp) * MPS_TO_MPH;
    }

    bool hasOptimal() const { return true; }
};

class PlotHorizontalSpeed: public PlotValue
{
    Q_OBJECT

public:
    PlotHorizontalSpeed(): PlotValue(false, Qt::red) {}
    const QString title() const
    {
        return tr("Horizontal Speed");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (km/h)");
        else                 return title() + tr(" (mph)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::horizontalSpeed(dp) * MPS_TO_KMH;
        else                 return DataPoint::horizontalSpeed(dp) * MPS_TO_MPH;
    }

    bool hasOptimal() const { return true; }
};

class PlotTotalSpeed: public PlotValue
{
    Q_OBJECT

public:
    PlotTotalSpeed(): PlotValue(false, Qt::blue) {}
    const QString title() const
    {
        return tr("Total Speed");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (km/h)");
        else                 return title() + tr(" (mph)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::totalSpeed(dp) * MPS_TO_KMH;
        else                 return DataPoint::totalSpeed(dp) * MPS_TO_MPH;
    }

    bool hasOptimal() const { return true; }
};

class PlotDiveAngle: public PlotValue
{
    Q_OBJECT

public:
    PlotDiveAngle(): PlotValue(false, Qt::magenta) {}
    const QString title() const
    {
        return tr("Dive Angle");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (deg)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::diveAngle(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotCurvature: public PlotValue
{
    Q_OBJECT

public:
    PlotCurvature(): PlotValue(false, Qt::darkYellow) {}
    const QString title() const
    {
        return tr("Dive Rate");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (deg/s)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::curvature(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotGlideRatio: public PlotValue
{
    Q_OBJECT

public:
    PlotGlideRatio(): PlotValue(false, Qt::darkCyan) {}
    const QString title() const
    {
        return tr("Glide Ratio");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title();
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::glideRatio(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotHorizontalAccuracy: public PlotValue
{
    Q_OBJECT

public:
    PlotHorizontalAccuracy(): PlotValue(false, Qt::darkRed) {}
    const QString title() const
    {
        return tr("Horizontal Accuracy");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (m)");
        else                 return title() + tr(" (ft)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return dp.DataPoint::horizontalAccuracy(dp);
        else                 return dp.DataPoint::horizontalAccuracy(dp) * METERS_TO_FEET;
    }
};

class PlotVerticalAccuracy: public PlotValue
{
    Q_OBJECT

public:
    PlotVerticalAccuracy(): PlotValue(false, Qt::darkGreen) {}
    const QString title() const
    {
        return tr("Vertical Accuracy");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (m)");
        else                 return title() + tr(" (ft)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::verticalAccuracy(dp);
        else                 return DataPoint::verticalAccuracy(dp) * METERS_TO_FEET;
    }
};

class PlotSpeedAccuracy: public PlotValue
{
    Q_OBJECT

public:
    PlotSpeedAccuracy(): PlotValue(false, Qt::darkBlue) {}
    const QString title() const
    {
        return tr("Speed Accuracy");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (km/h)");
        else                 return title() + tr(" (mph)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::speedAccuracy(dp) * MPS_TO_KMH;
        else                 return DataPoint::speedAccuracy(dp) * MPS_TO_MPH;
    }
};

class PlotNumberOfSatellites: public PlotValue
{
    Q_OBJECT

public:
    PlotNumberOfSatellites(): PlotValue(false, Qt::darkMagenta) {}
    const QString title() const
    {
        return tr("Number of Satellites");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title();
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::numberOfSatellites(dp);
    }
};

class PlotTime: public PlotValue
{
    Q_OBJECT

public:
    PlotTime(): PlotValue(false, Qt::black) {}
    const QString title() const
    {
        return tr("Time");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (s)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::time(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotDistance2D: public PlotValue
{
    Q_OBJECT

public:
    PlotDistance2D(): PlotValue(false, Qt::black) {}
    const QString title() const
    {
        return tr("Horizontal Distance");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (m)");
        else                 return title() + tr(" (ft)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::distance2D(dp);
        else                 return DataPoint::distance2D(dp) * METERS_TO_FEET;
    }

    bool hasOptimal() const { return true; }
};

class PlotDistance3D: public PlotValue
{
    Q_OBJECT

public:
    PlotDistance3D(): PlotValue(false, Qt::black) {}
    const QString title() const
    {
        return tr("Total Distance");
    }
    const QString title(Units units) const
    {
        if (units == Metric) return title() + tr(" (m)");
        else                 return title() + tr(" (ft)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        if (units == Metric) return DataPoint::distance3D(dp);
        else                 return DataPoint::distance3D(dp) * METERS_TO_FEET;
    }

    bool hasOptimal() const { return true; }
};

class PlotAcceleration: public PlotValue
{
    Q_OBJECT

public:
    PlotAcceleration(): PlotValue(false, Qt::darkRed) {}
    const QString title() const
    {
        return tr("Acceleration");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (m/s^2)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::acceleration(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotTotalEnergy: public PlotValue
{
    Q_OBJECT

public:
    PlotTotalEnergy(): PlotValue(false, Qt::darkGreen) {}
    const QString title() const
    {
        return tr("Total Energy");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (J/kg)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::totalEnergy(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotEnergyRate: public PlotValue
{
    Q_OBJECT

public:
    PlotEnergyRate(): PlotValue(false, Qt::darkBlue) {}
    const QString title() const
    {
        return tr("Energy Rate");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title() + tr(" (J/kg/s)");
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::energyRate(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotLift: public PlotValue
{
    Q_OBJECT

public:
    PlotLift(): PlotValue(false, Qt::darkGreen) {}
    const QString title() const
    {
        return tr("Lift Coefficient");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title();
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::liftCoefficient(dp);
    }

    bool hasOptimal() const { return true; }
};

class PlotDrag: public PlotValue
{
    Q_OBJECT

public:
    PlotDrag(): PlotValue(false, Qt::darkBlue) {}
    const QString title() const
    {
        return tr("Drag Coefficient");
    }
    const QString title(Units units) const
    {
        Q_UNUSED(units);
        return title();
    }
    double value(const DataPoint &dp, Units units) const
    {
        Q_UNUSED(units);
        return DataPoint::dragCoefficient(dp);
    }

    bool hasOptimal() const { return true; }
};

#endif // PLOTVALUE_H
