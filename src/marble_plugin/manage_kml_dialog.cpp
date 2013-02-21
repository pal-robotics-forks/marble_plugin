#include "manage_kml_dialog.h"

#include "qt4/Qt/qcheckbox.h"
#include "qt4/Qt/qpushbutton.h"
#include "qt4/Qt/qfiledialog.h"
#include <iostream>

#include "ros/package.h"

#include <marble_plugin/ui_manage_kml_dialog.h>

namespace marble_plugin{

ManageKmlDialog::ManageKmlDialog(std::map< QString, bool>& kml_files, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manageKmlDialog)
{
    ui->setupUi(this);

    ui->kml_treeWidget->header()->show();
    ui->kml_treeWidget->header()->resizeSection(0, 630);
    ui->kml_treeWidget->header()->resizeSection(1, 45);

    for(std::map< QString, bool>::iterator it = kml_files.begin(); it!=kml_files.end();it++)
    {
        QString filename = it->first;
        QFileInfo fi;
        fi.setFile(filename);
        bool show = it->second;

        addKMLToTreeWiev(fi, show);
    }

    m_kml_files = kml_files;

    connect( ui->addButton , SIGNAL(clicked()) , this, SLOT( SetKMLFile() ));
    connect( ui->removeButton, SIGNAL(clicked()), this, SLOT(deleteKML()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(cancelButtonClicked()));
}

ManageKmlDialog::~ManageKmlDialog()
{
    delete ui;
}


void ManageKmlDialog::SetKMLFile( bool envoke_file_dialog )
{
    QFileInfo fi;

    if(envoke_file_dialog )
    {
        QString fn = QFileDialog::getOpenFileName( 0 ,
                                     tr("Open Geo Data File"), tr("") , tr("Geo Data Files (*.kml)"));
        fi.setFile( fn );
    }

    if( fi.isFile() )
    {
        bool show = true;
        addKMLToTreeWiev(fi, show);
        m_kml_files[fi.absoluteFilePath()] = show;
    }

}

void ManageKmlDialog::addKMLToTreeWiev(QFileInfo& kmlFile, bool show)
{
    QList<QTreeWidgetItem*> items = ui->kml_treeWidget->findItems( kmlFile.absoluteFilePath() , Qt::MatchExactly | Qt::MatchRecursive , 0 );
    if(items.size()>0)
    {
        return; // no doubles
    }

    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0, kmlFile.absoluteFilePath() );
    item->setCheckState(1, Qt::Checked);
    ui->kml_treeWidget->addTopLevelItem( item );

}

void ManageKmlDialog::deleteKML()
{

    QList<QTreeWidgetItem*> items = ui->kml_treeWidget->selectedItems();

    if(items.size()==1)
    {
        QTreeWidgetItem* item = items.at(0);
        m_kml_files.erase(item->text(0));
        delete items.at(0);
    }
}

void ManageKmlDialog::checkBoxClicked(int state)
{

}

std::map< QString, bool> ManageKmlDialog::getKmlFiles()
{
    return m_kml_files;
}

void ManageKmlDialog::cancelButtonClicked()
{
    m_kml_files.clear();
}


}
