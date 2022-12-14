/*****************************************************************************
 **                                                                         **
 **  This file is part of GlassPlotter.                                     **
 **                                                                         **
 **  GlassPlotter is free software: you can redistribute it and/or modify   **
 **  it under the terms of the GNU General Public License as published by   **
 **  the Free Software Foundation, either version 3 of the License, or      **
 **  (at your option) any later version.                                    **
 **                                                                         **
 **  GlassPlotter is distributed in the hope that it will be useful,        **
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of         **
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          **
 **  GNU General Public License for more details.                           **
 **                                                                         **
 **  You should have received a copy of the GNU General Public License      **
 **  along with GlassPlotter.  If not, see <http://www.gnu.org/licenses/>.  **
 **                                                                         **
 *****************************************************************************
 **  Author  : Hiiragi                                                      **
 **  Contact : heterophyllus.work@gmail.com                                 **
 **  Website : https://github.com/heterophyllus/glassplotter                **
 **  Date    : 2020-1-25                                                    **
 *****************************************************************************/


#ifndef QCPSCATTERCHART_H
#define QCPSCATTERCHART_H

#include "qcustomplot.h"

/** Class for scatter chart using QCustomPlot */
class QCPScatterChart
{

public:
    QCPScatterChart(QCustomPlot *customPlot);
    ~QCPScatterChart();
    QCPScatterChart(QCPScatterChart &other);

    QCustomPlot*        parentPlot() const;
    QCPCurve*           graphPoints() const;
    QList<QCPItemText*> textLabels() const;
    QString             name() const;

    void setData(const QVector<double>& x, const QVector<double>& y, const QVector<QString>& label_texts);
    void setName(QString name);
    void setColor(QColor color);
    void setVisiblePointSeries(bool state);
    void setVisibleTextLabels(bool state);
    void setAxis(QCPRange xrange, QCPRange yrange);
    int  dataCount() const;

private:
    QCustomPlot*        m_customPlot;
    QCPCurve*           m_graphPoints; //points
    QList<QCPItemText*> m_textlabels; // text label for each point

};

#endif // QCPSCATTERCHART_H
