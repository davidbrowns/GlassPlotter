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

#ifndef GLASSMAP_FORM_H
#define GLASSMAP_FORM_H

#include <QWidget>
#include "qcpscatterchart.h"
#include "glass_catalog.h"


namespace Ui {
class GlassMapForm;
}

class GlassMapForm : public QWidget
{
    Q_OBJECT

public:
    explicit GlassMapForm(QString xdataname, QString ydataname, QCPRange xrange, QCPRange yrange, bool xreversed = true, QMdiArea *parent = nullptr);
    ~GlassMapForm();

private slots:
    void setLegendVisible();
    void showCurveFittingDlg();
    void showNeighbors(QCPAbstractItem* item);
    void clearNeighbors();
    void showGlassDataSheet();
    void update();
    void setDefault();
    void showPresetDlg();
    void showContextMenu();
    void exportImage();

private:
    /**
     * @brief The GlassMapCtrl class
     * @details Inner class to contain the checkbox pair
     */
    class GlassMapCtrl{
    public:
        GlassMapCtrl(QLabel* l, QCheckBox* checkPlot, QCheckBox* checkLabel);
        QLabel*    labelSupplier;
        QCheckBox* checkBoxPlot;
        QCheckBox* checkBoxLabel; //text label
    };

    Ui::GlassMapForm *ui;

    QMdiArea*    m_parentMdiArea;
    QCustomPlot* m_customPlot;
    QCheckBox*   m_checkBoxLegend;
    QCheckBox*   m_checkBoxCurve;
    QListWidget* m_listWidgetNeighbors;

    QList<GlassMapCtrl>  m_glassMapCtrlList;
    QList<QLineEdit*>    m_lineEditList;
    QList<QGridLayout*>  m_gridLayoutList;

    QSettings* m_settings;
    QString    m_settingFile;

    QString m_xDataName;
    QString m_yDataName;

    QCPRange m_defaultXrange;
    QCPRange m_defaultYrange;

    bool m_xReversed;

    bool m_draggingLegend;
    QPointF m_dragLegendOrigin;

    void   setGlassmapData(QCPScatterChart* glassmap, GlassCatalog* catalog, QString xlabel, QString ylabel, QColor color);
    void   setUpScrollArea();
    void   saveSetting();
    QList<double> getCurveCoefs();
    void setCurveCoefsToUI(const QList<double>& coefs);
    void setCurveData(QCPGraph* curveGraph, const QList<double>& coefs);

    QColor getColorFromIndex(int index, int maxIndex);

private slots:
    void mouseMoveSignal(QMouseEvent *event);
    void mousePressSignal(QMouseEvent *event);
    void mouseReleaseSignal(QMouseEvent *event);
    void beforeReplot();
};

#endif // GLASSMAP_FORM_H
