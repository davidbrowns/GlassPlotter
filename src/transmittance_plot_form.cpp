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


#include "transmittance_plot_form.h"
#include "ui_transmittance_plot_form.h"

#include "glass_selection_dialog.h"
#include "glass_catalog_manager.h"

TransmittancePlotForm::TransmittancePlotForm(QWidget *parent) :
    PropertyPlotForm(parent),
    ui(new Ui::TransmittancePlotForm)
{
    ui->setupUi(this);
    this->setWindowTitle("Transmittance Plot");

    // plot widget
    m_customPlot = ui->widget;
    m_customPlot->setInteractions(QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    m_customPlot->xAxis->setLabel("Wavelength(nm)");
    m_customPlot->yAxis->setLabel("Internal Transmittance");
    m_customPlot->legend->setVisible(true);
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom); // Legend position

    m_maxGraphCount = 5;

    // table
    m_plotDataTable = ui->tableWidget;

    // buttons ,legend checkbox
    m_chkLegend = ui->checkBox_Legend;
    QList<QPushButton*> buttons({ui->pushButton_AddGraph ,ui->pushButton_DeleteGraph , ui->pushButton_SetAxis , ui->pushButton_Clear});
    setupFundamentalUi(buttons, m_chkLegend);

    // legend draggable
    m_customPlot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
    m_draggingLegend = false;

    connect(m_customPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveSignal(QMouseEvent*)));
    connect(m_customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressSignal(QMouseEvent*)));
    connect(m_customPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseReleaseSignal(QMouseEvent*)));
    connect(m_customPlot, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));


    // thickness
    m_editThickness = ui->lineEdit_Thickness;
    m_editThickness->setText(QString::number(25));

    // plot step
    m_editPlotStep = ui->lineEdit_PlotStep;
    m_editPlotStep->setValidator(new QDoubleValidator(0, 100, 2, this));
    m_editPlotStep->setText(QString::number(5));

    // axis
    m_defaultXrange = QCPRange(300, 2000);
    m_defaultYrange = QCPRange(0.0, 1.2);

    m_editXmin = ui->lineEdit_Xmin;
    m_editXmax = ui->lineEdit_Xmax;
    m_editYmin = ui->lineEdit_Ymin;
    m_editYmax = ui->lineEdit_Ymax;


    setDefault();
}

TransmittancePlotForm::~TransmittancePlotForm()
{
    m_glassList.clear();

    m_customPlot->clearGraphs();
    m_customPlot->clearItems();
    m_customPlot->clearPlottables();
    m_customPlot = nullptr;

    m_plotDataTable->clear();
    m_plotDataTable = nullptr;

    delete ui;
}


void TransmittancePlotForm::addGraph()
{
    if(m_customPlot->graphCount() >= m_maxGraphCount) // 5 glass
    {
        QString message = "Up to " + QString::number(m_maxGraphCount) + " graphs can be plotted";
        QMessageBox::information(this,tr("Error"), message);
        return;
    }

    GlassSelectionDialog *dlg = new GlassSelectionDialog(this);

    if(dlg->exec() == QDialog::Accepted)
    {
        Glass* newGlass = dlg->getSelectedGlass();
        m_glassList.append(newGlass);
        updateAll();
    }

    delete dlg;
    dlg = nullptr;

}


void TransmittancePlotForm::updateAll()
{
    m_customPlot->clearGraphs();
    m_customPlot->clearItems();
    m_customPlot->clearPlottables();
    m_plotDataTable->clear();

    double          thickness     = m_editThickness->text().toDouble();
    double          plotStep      = m_editPlotStep->text().toDouble();
    QVector<double> vLambdanano   = getVectorFromRange(m_customPlot->xAxis->range(), plotStep); // unit:nm
    QVector<double> vLambdamicron = scaleVector(vLambdanano,0.001);
    QVector<double> ydata;
    QCPGraph*       graph;
    QCPItemTracer*  upperTracer;
    QCPItemTracer*  lowerTracer;

    int i,j;
    int rowCount    = vLambdanano.size();
    int columnCount = m_glassList.size() + 1; // lambda + glasses
    m_plotDataTable->setRowCount(rowCount);
    m_plotDataTable->setColumnCount(columnCount);

    QStringList header = QStringList() << "Wavelength(nm)";

    int    glassCount = m_glassList.size();
    Glass* currentGlass;

    int digit = ui->spinBox_Digit->value();

    // replot all graphs and recreate tables
    for(i = 0; i < glassCount; i++)
    {
        currentGlass = m_glassList[i];

        // graphs
        ydata = currentGlass->transmittance(vLambdamicron, thickness);
        graph = m_customPlot->addGraph();
        graph->setName(currentGlass->fullName());
        graph->setData(vLambdanano, ydata);
        graph->setPen(QPen(getColorFromIndex(i, m_maxGraphCount)));
        graph->setVisible(true);

        // tracer
        upperTracer = new QCPItemTracer(m_customPlot);
        upperTracer->setGraph(graph);
        upperTracer->setInterpolating(true);
        upperTracer->setStyle(QCPItemTracer::tsCircle);
        upperTracer->setSize(7);
        upperTracer->setPen(graph->pen());
        upperTracer->setGraphKey(currentGlass->lambdaMax()*(double)1000);
        upperTracer->updatePosition();

        lowerTracer = new QCPItemTracer(m_customPlot);
        lowerTracer->setGraph(graph);
        lowerTracer->setInterpolating(true);
        lowerTracer->setStyle(QCPItemTracer::tsCircle);
        lowerTracer->setSize(7);
        lowerTracer->setPen(graph->pen());
        lowerTracer->setGraphKey(currentGlass->lambdaMin()*(double)1000);
        lowerTracer->updatePosition();

        // table
        header << currentGlass->productName();
        for(j = 0; j< rowCount; j++)
        {
            setValueToCell(j, 0,   vLambdanano[j], digit ); // wavelength
            setValueToCell(j, i+1, ydata[j],       digit ); // transmittance
        }
    }

    m_customPlot->replot();
    m_plotDataTable->setHorizontalHeaderLabels(header);

}

void TransmittancePlotForm::deleteGraph()
{
    if(m_customPlot->selectedGraphs().size() > 0)
    {
        QCPGraph* selectedGraph = m_customPlot->selectedGraphs().at(0);
        QString graphName = selectedGraph->name();
        QStringList glass_supplier = graphName.split("_");
        int glassCount = m_glassList.size();
        for(int i = 0;i < glassCount; i++){
            if(m_glassList[i]->productName() == glass_supplier[0] && m_glassList[i]->supplier() == glass_supplier[1]){
                m_glassList.removeAt(i);
                break;
            }
        }
        updateAll();
    }
}


void TransmittancePlotForm::clearAll()
{
    m_glassList.clear();
    m_customPlot->clearGraphs();
    m_customPlot->clearItems();
    m_customPlot->replot();
    m_plotDataTable->clear();
    m_plotDataTable->update();
}

