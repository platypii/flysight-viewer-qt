#include "dataplot.h"
#include "mainwindow.h"

DataPlot::DataPlot(QWidget *parent) :
    QCustomPlot(parent),
    mMainWindow(0),
    m_dragging(false)
{
    setMouseTracking(true);
}

void DataPlot::mousePressEvent(
        QMouseEvent *event)
{
    if (axisRect()->rect().contains(event->pos()))
    {
        m_beginPos = event->pos();
        m_dragging = true;
        update();
    }

    QCustomPlot::mousePressEvent(event);
}

void DataPlot::mouseReleaseEvent(
        QMouseEvent *event)
{
    QPoint endPos = event->pos();

    MainWindow::Tool tool = mMainWindow->tool();
    if (m_dragging && tool == MainWindow::Zoom)
    {
        QCPRange range(qMin(xAxis->pixelToCoord(m_beginPos.x()),
                            xAxis->pixelToCoord(endPos.x())),
                       qMax(xAxis->pixelToCoord(m_beginPos.x()),
                            xAxis->pixelToCoord(endPos.x())));

        mMainWindow->setRange(range);
    }
    if (m_dragging && tool == MainWindow::Zero)
    {
        mMainWindow->setZero(xAxis->pixelToCoord(endPos.x()));
    }
    if (m_dragging && tool == MainWindow::Ground)
    {
        mMainWindow->setGround(xAxis->pixelToCoord(endPos.x()));
    }

    if (m_dragging)
    {
        m_dragging = false;
        replot();
    }

    QCustomPlot::mouseReleaseEvent(event);
}

void DataPlot::mouseMoveEvent(
        QMouseEvent *event)
{
    m_cursorPos = event->pos();

    MainWindow::Tool tool = mMainWindow->tool();
    if (m_dragging && tool == MainWindow::Pan)
    {
        QCPRange range = xAxis->range();

        double diff = xAxis->pixelToCoord(m_beginPos.x())
                - xAxis->pixelToCoord(m_cursorPos.x());
        range = QCPRange(range.lower + diff, range.upper + diff);

        mMainWindow->setRange(range);

        m_beginPos = m_cursorPos;
    }

    if (axisRect()->rect().contains(event->pos()))
    {
        if (m_dragging && tool == MainWindow::Measure)
        {
            emit measure(xAxis->pixelToCoord(m_beginPos.x()),
                         xAxis->pixelToCoord(m_cursorPos.x()));
        }
        else
        {
            emit mark(xAxis->pixelToCoord(m_cursorPos.x()));
        }
    }
    else
    {
        emit clear();
    }

    update();

    QCustomPlot::mouseMoveEvent(event);
}

void DataPlot::wheelEvent(
        QWheelEvent *event)
{
    if (axisRect()->rect().contains(event->pos()))
    {
        double multiplier = exp((double) -event->angleDelta().y() / 500);

        double x = xAxis->pixelToCoord(event->pos().x());

        QCPRange range = xAxis->range();

        range = QCPRange(
                    x + (range.lower - x) * multiplier,
                    x + (range.upper - x) * multiplier);

        mMainWindow->setRange(range);
    }
}

void DataPlot::leaveEvent(
        QEvent *)
{
    emit clear();
}

void DataPlot::paintEvent(
        QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);

    MainWindow::Tool tool = mMainWindow->tool();
    if (m_dragging && (tool == MainWindow::Zoom || tool == MainWindow::Measure))
    {
        QPainter painter(this);

        painter.setPen(QPen(Qt::black));
        painter.drawLine(m_beginPos.x(), axisRect()->rect().top(), m_beginPos.x(), axisRect()->rect().bottom());
        if (axisRect()->rect().left() <= m_cursorPos.x() && m_cursorPos.x() <= axisRect()->rect().right())
        {
            painter.drawLine(m_cursorPos.x(), axisRect()->rect().top(), m_cursorPos.x(), axisRect()->rect().bottom());
        }

        QRect shading(
                    qMin(m_beginPos.x(), m_cursorPos.x()),
                    axisRect()->rect().top(),
                    qAbs(m_beginPos.x() - m_cursorPos.x()),
                    axisRect()->rect().height());

        painter.fillRect(shading & axisRect()->rect(), QColor(181, 217, 42, 64));
    }
    else
    {
        if (axisRect()->rect().contains(m_cursorPos))
        {
            QPainter painter(this);

            painter.setPen(QPen(Qt::black));
            painter.drawLine(m_cursorPos.x(), axisRect()->rect().top(), m_cursorPos.x(), axisRect()->rect().bottom());
            painter.drawLine(axisRect()->rect().left(), m_cursorPos.y(), axisRect()->rect().right(), m_cursorPos.y());
        }
    }
}

void DataPlot::setRange(
        const QCPRange &range)
{
    xAxis->setRange(range);
    updateYRanges();
}

void DataPlot::updateYRanges()
{
    const QCPRange &range = xAxis->range();

    int k = 0;
    for (int j = 0; j < MainWindow::yaLast; ++j)
    {
        if (!mMainWindow->yValue(j)->visible()) continue;

        double yMin, yMax;
        bool first = true;

        for (int i = 0; i < mMainWindow->dataSize(); ++i)
        {
            const DataPoint &dp = mMainWindow->dataPoint(i);

            if (range.contains(mMainWindow->xValue()->value(dp, mMainWindow->units())))
            {
                double y = mMainWindow->yValue(j)->value(dp, mMainWindow->units());

                if (first)
                {
                    yMin = yMax = y;
                    first = false;
                }
                else
                {
                    if (y < yMin) yMin = y;
                    if (y > yMax) yMax = y;
                }
            }
        }

        if (!first)
            axisRect()->axis(QCPAxis::atLeft, k++)->setRange(yMin, yMax);
    }

    replot();
}
